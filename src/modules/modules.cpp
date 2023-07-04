#include "modules.hpp"
#include "../types.hpp"
#include <string>
#include <v8.h>
#include <filesystem>

namespace fs = std::filesystem;

extern const SharedGlobals globals;

namespace modules {
    void metadataHook(v8::Local<v8::Context> ctx,
            v8::Local<v8::Module> mod, v8::Local<v8::Object> meta)
    {
        v8::Isolate *isolate = ctx->GetIsolate();
        int id = mod->ScriptId();
        auto extractedModule = globals.modules[id];
        const auto& metaObj = extractedModule->GetMeta();
        for (const auto& value : metaObj) {
            v8::Local<v8::Name> key = v8::String::NewFromUtf8(isolate, 
                    value->ckey).ToLocalChecked();
            v8::Local<v8::Value> val = value->value.Get(isolate);
            v8::Maybe<bool> ret = meta->CreateDataProperty(ctx, key, val);
            if (!ret.ToChecked()) break;
        }
    }

    v8::MaybeLocal<v8::Module> moduleResolver(
        v8::Local<v8::Context> ctx,
        v8::Local<v8::String> specifier,
        [[maybe_unused]] v8::Local<v8::FixedArray> import_assertions,
        v8::Local<v8::Module> ref
    )
    {
        v8::Isolate *isolate = ctx->GetIsolate();
        const char* path = *neko::jsToString(isolate, specifier).get();
        std::string name(path);

        std::string base = name;

        auto extractedModule = globals.modules[ref->ScriptId()];
        const char* url = *neko::jsToString(isolate, 
                extractedModule->GetMetaValue("url")->value.Get(isolate)).get();

        std::string urlPath(url);

        if (base.c_str()[0] != '/') {
            base = fs::path(urlPath).parent_path();
            base += "/" + name;
            base = fs::path(base).lexically_normal();
        }

        if (globals.cachedModules.contains(base)) {
            int id = globals.cachedModules[base];
            auto cachedModule = globals.modules[id];

            return cachedModule->GetModule();
        }

        std::string code = neko::readFile(base.c_str());

        v8::Local<v8::Value> url2 = v8::String::NewFromUtf8(
                ctx->GetIsolate(), base.c_str()).ToLocalChecked();

        auto mod = ModuleWrap::CompileModule(isolate, code.c_str());
        int id = mod->GetModuleId();
        mod->SetMeta(isolate, "url", url2);

        globals.cachedModules[base] = id;
        globals.modules[id] = mod;

        return mod->GetModule();
    }
}
