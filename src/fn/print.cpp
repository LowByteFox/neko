#include "print.hpp"
#include "../api/neko.hpp"

#include <cstdio>
#include <v8.h>

void __recurse_print(v8::Isolate *isolate,
        v8::Local<v8::Value> value,
        int depth = 0,
        bool isNext = false)
{
    if (value->IsString()) {
        char *out = neko::jsToString(isolate, value);
        if (depth) printf("\"%s\"", out);
        else printf("%s\n", out);
    } else if (value->IsNumber()) {
        char *out = neko::jsToString(isolate, value);
        if (depth) printf("\"%s\"", out);
        else printf("%s\n", out);
    }
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
