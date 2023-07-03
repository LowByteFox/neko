#include <cstdio>
#include <cstring>
#include <memory>
#include <string>
#include <filesystem>

#include <v8.h>
#include <libplatform/libplatform.h>

#include "fn/print.hpp"
#include "api/neko.hpp"
#include "modules/modules.hpp"
#include "types.hpp"

#define VERSION "0.0.1"

namespace fs = std::filesystem;

inline const SharedGlobals globals;

void execute(v8::Isolate *isolate, char *filename)
{
    v8::Isolate::Scope isolateScope(isolate);
    v8::HandleScope handleScope(isolate);

    isolate->SetCaptureStackTraceForUncaughtExceptions(true);

    v8::Local<v8::ObjectTemplate> global = v8::ObjectTemplate::New(isolate);
    global->Set(isolate, "print", v8::FunctionTemplate::New(isolate, print));

    v8::Local<v8::Context> ctx = v8::Context::New(isolate, nullptr, global);
    ctx->AllowCodeGenerationFromStrings(false);
    ctx->SetErrorMessageForCodeGenerationFromStrings(
            v8::String::NewFromUtf8(
                isolate,
                "both 'eval' and 'Function' constructor are disabled!"
                ).ToLocalChecked()
            );
    isolate->SetHostInitializeImportMetaObjectCallback(modules::metadataHook);

    v8::Context::Scope ctxScope(ctx);

    // init builtin modules

    std::string code = neko::readFile(filename);
    if (!code.length()) {
        neko::throwAndPrintException(ctx, "Error: File not found");
        return;
    }

    auto mod = ModuleWrap::CompileModule(isolate, code.c_str());
    v8::Local<v8::Value> url = v8::String::NewFromUtf8(isolate, filename).ToLocalChecked();
    mod->SetMeta(isolate, "url", url);

    globals.modules[mod->GetModuleId()] = mod;

    {
        v8::Isolate::Scope isolateScope(isolate);
        v8::Context::Scope ctxScope(ctx);
        v8::HandleScope handleScope(isolate);

        v8::Local<v8::Module> extractedModule = mod->GetModule();

        v8::TryCatch tryCatch(isolate);
        if (v8::Maybe<bool> out = extractedModule->InstantiateModule(
                    ctx, modules::moduleResolver); out.IsNothing()) {
            if (v8::Module::kUninstantiated == extractedModule->GetStatus()) {
                neko::printException(ctx, tryCatch.Exception());
                return;
            }
        }

        if (v8::MaybeLocal<v8::Value> res = extractedModule->Evaluate(ctx);
                extractedModule->GetStatus() == v8::Module::kErrored && !res.IsEmpty()) {
            if (v8::Module::kErrored == extractedModule->GetStatus()) {
                neko::printException(ctx, extractedModule->GetException());
                return;
            }
        }
    }
}

void run(char *file, char *argv[])
{
    v8::V8::InitializeICUDefaultLocation(argv[0]);
    v8::V8::InitializeExternalStartupData(argv[0]);
    auto platform = v8::platform::NewDefaultPlatform();
    v8::V8::SetFlagsFromString("--use-strict true");
    v8::V8::InitializePlatform(platform.get());
    v8::V8::Initialize();

    v8::Isolate::CreateParams createParams;
    createParams.array_buffer_allocator = v8::ArrayBuffer::Allocator::NewDefaultAllocator();
    v8::Isolate *isolate = v8::Isolate::New(createParams);

    // Runtime spins here
    execute(isolate, file);

    isolate->Dispose();
    v8::V8::Dispose();
    v8::V8::ShutdownPlatform();
    delete createParams.array_buffer_allocator;
}

void create(char *name)
{
    (void) name;
    printf("Project creation unavailable now\n");
}

void printHelp()
{
    printf(R"(neko - JavaScript runtime for the modern age

Usage: neko [OPTIONS] (ARGS)

OPTIONS:
  help, -h          Display this help message
  version, -v       Display version
  run (SCRIPT)      Execute (SCRIPT)
  create (NAME)     Create a new project (NAME)
)");
}

int main (int argc, char *argv[])
{
    // ARG handling
    if (argc == 1) {
        printHelp();
        return 0;
    }

    for (int i = 1; i < argc; i++) {
        char *s = argv[i];
        if (!strcmp(s, "-v") || !strcmp(s, "version")) {
            printf(VERSION);
            break;
        } else if (!strcmp(s, "-h") || !strcmp(s, "help")) {
            printHelp();
            break;
        } else if (!strcmp(s, "run")) {
            char *val = nullptr;
            if (i + 1 < argc) val = argv[i + 1];
            run(val, argv);
            break;
        } else if (!strcmp(s, "create")) {
            char *val = nullptr;
            if (i + 1 < argc) val = argv[i + 1];
            create(val);
            break;
        }
    }
    return 0;
}
