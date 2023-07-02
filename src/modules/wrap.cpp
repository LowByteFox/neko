#include "../types.hpp"

#include <v8.h>
#include <vector>
#include <cstring>

v8::Local<v8::Module> ModuleWrap::GetModule()
{
    return this->mod;
}

void ModuleWrap::SetModule(v8::Local<v8::Module> mod)
{
    this->mod = mod;
    this->modId = mod->ScriptId();
}

int ModuleWrap::GetModuleId()
{
    return this->modId;
}

std::vector<Metadata> ModuleWrap::GetMeta()
{
    return this->meta;
}

Metadata ModuleWrap::GetMetaValue(const char *key)
{
    for (auto meta: this->meta) {
        if (!strcmp(key, meta.ckey)) return meta;
    }
    return {};
}

void ModuleWrap::SetMeta(v8::Isolate *isolate, const char *key, v8::Local<v8::Value> value)
{
    v8::Isolate::Scope isolateScope(isolate);
    v8::Local<v8::Context> ctx = isolate->GetCurrentContext();
    v8::Context::Scope ctxScope(ctx);

    this->meta.push_back({
            .ckey = strdup(key),
            .key = v8::String::NewFromUtf8(isolate, key).ToLocalChecked(),
            .value = value
        }
    );
}
