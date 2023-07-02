#include <cstdio>
#include <cstring>
#include <memory>

#include <v8.h>
#include <libplatform/libplatform.h>

#define VERSION "0.0.1"

void run(char *file, char *argv[]) {
    v8::V8::InitializeICUDefaultLocation(argv[0]);
    v8::V8::InitializeExternalStartupData(argv[0]);
    auto platform = v8::platform::NewDefaultPlatform();
    v8::V8::SetFlagsFromString("--use-strict");
    v8::V8::InitializePlatform(platform.get());
    v8::V8::Initialize();

    v8::Isolate::CreateParams createParams;
    createParams.array_buffer_allocator = v8::ArrayBuffer::Allocator::NewDefaultAllocator();

    v8::Isolate *isolate = v8::Isolate::New(createParams);

    isolate->Dispose();
    v8::V8::Dispose();
    v8::V8::ShutdownPlatform();
    delete createParams.array_buffer_allocator;
}

void create(char *name) {
    (void) name;
    printf("Project creation unavailable now\n");
}

void printHelp() {
    printf(R"(neko - JavaScript runtime for the modern age

Usage: neko [OPTIONS] (ARGS)

OPTIONS:
  help, -h          Display this help message
  version, -v       Display version
  run (SCRIPT)      Execute (SCRIPT)
  create (NAME)     Create a new project (NAME)
)");
}

int main (int argc, char *argv[]) {
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
