#include <cstdio>
#include <cstring>

#define VERSION "0.0.1"

void run(char *file) {
    (void) file;
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
            run(val);
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
