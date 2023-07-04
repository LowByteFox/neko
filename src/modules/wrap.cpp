#include "../types.hpp"

#include <v8.h>
#include <vector>
#include <cstring>
#include <memory>

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

std::vector<std::shared_ptr<Metadata>> ModuleWrap::GetMeta()
{
    return this->meta;
}

std::shared_ptr<Metadata> ModuleWrap::GetMetaValue(const char *key)
{
    for (auto meta: this->meta) {
        if (!strcmp(key, meta->ckey)) return meta;
    }
    return {};
}

void ModuleWrap::SetMeta(v8::Isolate *isolate, const char *key, v8::Local<v8::Value> value)
{
    v8::Isolate::Scope isolateScope(isolate);
    v8::Local<v8::Context> ctx = isolate->GetCurrentContext();
    v8::Context::Scope ctxScope(ctx);
    v8::HandleScope HandleScope(isolate);

    auto val = std::make_shared<Metadata>();
    val->ckey = strdup(key);
    val->key = v8::String::NewFromUtf8(isolate, key).ToLocalChecked();
    val->value.Reset(isolate, value);

    this->meta.push_back(val);
}
