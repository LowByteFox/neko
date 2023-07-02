#ifndef TYPES_HPP
#define TYPES_HPP

#include "api/neko.hpp"
#include <map>
#include <memory>

class ModuleWrap;

typedef struct _SharedGlobals {
    mutable int lastScriptId = 0;
    mutable std::map<int, std::shared_ptr<ModuleWrap>> modules;
    mutable std::map<std::string, int> cachedModules;
} SharedGlobals;

extern const SharedGlobals globals;

typedef struct {
    char *ckey;
    v8::Local<v8::Name> key;
    v8::Local<v8::Value> value;
} Metadata;

class ModuleWrap {
    public:
        v8::Local<v8::Module> GetModule();
        int GetModuleId();
        void SetModule(v8::Local<v8::Module> mod);
        std::vector<Metadata> GetMeta();
        Metadata GetMetaValue(const char *key);
        void SetMeta(v8::Isolate *isolate, const char *key, v8::Local<v8::Value> value);

        static std::shared_ptr<ModuleWrap> CompileModule(v8::Isolate *isolate, 
                const char *code)
        {
            auto mod = std::make_shared<ModuleWrap>();

            v8::Isolate::Scope isolatescope(isolate);

            v8::ScriptOrigin origin(isolate,
                    v8::Local<v8::Integer>(),
                    0, 0, false, globals.lastScriptId, v8::Local<v8::Value>(),
                    false, false, true);
            globals.lastScriptId++;

            v8::ScriptCompiler::Source source(v8::String::NewFromUtf8(
                        isolate, code).ToLocalChecked(), origin
                    );

            {
                v8::TryCatch tryCatch(isolate);
                v8::MaybeLocal<v8::Module> ret = v8::ScriptCompiler::CompileModule(
                        isolate, &source);
                if (tryCatch.HasCaught()) {
                    // handle exception
                }

                mod->SetModule(ret.ToLocalChecked());
            }

            return mod;
        }

    private:
        std::vector<Metadata> meta;
        v8::Local<v8::Module> mod;
        int modId;
};

#endif
