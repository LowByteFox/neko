#include "print.hpp"
#include "../api/neko.hpp"

#include <cstdio>
#include <v8.h>

void __recurse_print(v8::Isolate *isolate,
        v8::Local<v8::Value> value,
        int depth = 0,
        bool isNext = false)
{
    v8::Local<v8::Context> ctx = isolate->GetCurrentContext();
    v8::Local<v8::Value> out = v8::JSON::Stringify(ctx, value,
            v8::String::NewFromUtf8(isolate, "  ").ToLocalChecked()).ToLocalChecked();
    v8::String::Utf8Value str(isolate, out);
    printf("%s\n", *str);
}

void print(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    v8::Isolate::Scope isolateScope(isolate);
    int length = args.Length();

    for (int i = 0; i < length; i++) {
        __recurse_print(isolate, args[i]);
    }
}
