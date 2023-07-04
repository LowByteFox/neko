#include "print.hpp"
#include "../api/neko.hpp"
#include "../api/neko-iter.hpp"

#include <algorithm>
#include <cstdio>
#include <v8.h>
#include <vector>

const char* __printNullOrUndefined(v8::Local<v8::Value> val) {
    if (val->IsUndefined()) {
        return "undefined";
    }

    return "null";
}

void __recurse_print(v8::Isolate *isolate,
        const v8::Local<v8::Value>& value,
        int depth = 0,
        bool isNext = false, bool encapsulate_str = true, bool assign_newline = true,
        std::vector<v8::Local<v8::Value>> cached = {})
{
    if (std::find(cached.begin(), cached.end(), value) != cached.end()){
        if (!assign_newline) printf("(Cyclic)");
        else printf("(Cyclic)\n");
        return;
    }
    cached.push_back(value);
    v8::Local<v8::Context> ctx = isolate->GetCurrentContext();
    if (value->IsString()) {
        const char *str = *neko::jsToString(isolate, value);
        if (!assign_newline) {
            if (encapsulate_str) printf("\"%s\"", str);
            else printf("%s", str);
        } else {
            if (encapsulate_str) printf("\"%s\"\n", str);
            else printf("%s\n", str);
        }
    } else if (value->IsNumber()) {
        if (!assign_newline) {
            if (value->IsInt32()) printf("%d", value->Int32Value(ctx).FromJust());
            else printf("%f", value->NumberValue(ctx).FromJust());
        } else {
            if (value->IsInt32()) printf("%d\n", value->Int32Value(ctx).FromJust());
            else printf("%f\n", value->NumberValue(ctx).FromJust());
        }
    } else if (value->IsBoolean()) {
        if (!assign_newline) printf("%s", value->BooleanValue(isolate) ? "true" : "false");
        else printf("%s\n", value->BooleanValue(isolate) ? "true" : "false");
    } else if (value->IsNullOrUndefined()) {
        if (!assign_newline) printf("%s", __printNullOrUndefined(value));
        else printf("%s\n", __printNullOrUndefined(value));
    }  else if (value->IsFunction()) {
        auto fun = v8::Local<v8::Function>::Cast(value);
        v8::Local<v8::Value> funName = fun->GetName();
        std::string name = *neko::jsToString(isolate, funName).get();
        std::string type = "Function";

        if (!name.length()) {
            name = "anonymous";
        }

        if (fun->IsConstructor()) {
            v8::Local<v8::Value> toStr = fun->Get(ctx, 
                    v8::String::NewFromUtf8(isolate, "toString").ToLocalChecked()
                    ).ToLocalChecked();

            auto fn = v8::Local<v8::Function>::Cast(toStr);
            v8::MaybeLocal<v8::Value> rval = fn->Call(ctx, value, 0, nullptr);

            if (!rval.IsEmpty()) {
                v8::Local<v8::Value> ret = rval.ToLocalChecked();
                std::string code = *neko::jsToString(isolate, ret);
                std::string sub = code.substr(0, 5);
                if (sub == "class") type = "Class";
            }
        }

        if (!assign_newline) printf("(%s: %s)", type.c_str(), name.c_str());
        else printf("(%s: %s)\n", type.c_str(), name.c_str());
    } else if (value->IsMap()) {
        auto map = v8::Local<v8::Map>::Cast(value);
        size_t len = map->Size();
        printf("Map(%ld) {\n", len);
        depth += 2;
        int i = 0;
        len = len * 2;
        v8::Local<v8::Array> arr = map->AsArray();
        for (;i < (int) len - 2; i += 2) {
            for (int j = 0; j < depth; j++) {
                printf(" ");
            }
            v8::Local<v8::Value> key = arr->Get(ctx, i).ToLocalChecked();
            v8::Local<v8::Value> mapValue = arr->Get(ctx, i + 1).ToLocalChecked();

            __recurse_print(isolate, key, depth, true, false, false, cached);
            printf(" => ");
            __recurse_print(isolate, mapValue, depth, true, true, false, cached);
            printf(",\n");
        }

        v8::Local<v8::Value> key = arr->Get(ctx, i).ToLocalChecked();
        if (!key->IsUndefined()) {
            v8::Local<v8::Value> mapValue = arr->Get(ctx, i + 1).ToLocalChecked();
            for (int j = 0; j < depth; j++) {
                printf(" ");
            }

            __recurse_print(isolate, key, depth, false, false, false, cached);
            printf(" => ");
            __recurse_print(isolate, mapValue, depth, false, true, true, cached);
            depth -= 2;
            for (int i = 0; i < depth; i++) printf(" ");
        } else depth -= 2;
        printf("}");
        if (!isNext) printf("\n");

    } else if (value->IsArray()) {
        auto arr = v8::Local<v8::Array>::Cast(value);
        auto it = neko::V8ArrayIterator(arr, ctx, 1);

        printf("[");
        for (auto v : it) {
            __recurse_print(isolate, v, depth, true, true, false, cached);
            printf(", ");
        }
        auto last = arr->Get(ctx, arr->Length() - 1).ToLocalChecked();
        __recurse_print(isolate, last, depth, false, true, false, cached);
        printf("]");
        if (!isNext) printf("\n");
    } else if (value->IsObject()) {
        v8::Local<v8::Object> obj = value->ToObject(ctx).ToLocalChecked();
        v8::Local<v8::Array> props = obj->GetOwnPropertyNames(ctx).ToLocalChecked();

        int len = props->Length();
        printf("{\n");
        depth += 2;
        int i = 0;
        for (; i < len - 1; i++) {
            v8::Local<v8::Value> propName = props->Get(ctx, i).ToLocalChecked();
            for (int j = 0; j < depth; j++) {
                printf(" ");
            }

            __recurse_print(isolate, propName, depth, true, false, false, cached);
            printf(": ");
            v8::Local<v8::Value> propValue = obj->Get(ctx, propName).ToLocalChecked();
            __recurse_print(isolate, propValue, depth, false, true, false, cached);
            printf(",\n");
        }

        v8::Local<v8::Value> propName = props->Get(ctx, i).ToLocalChecked();

        if (!propName->IsUndefined()) {
            for (int j = 0; j < depth; j++) {
                printf(" ");
            }
            __recurse_print(isolate, propName, depth, false, false, false, cached);
            printf(": ");
            v8::Local<v8::Value> propValue = obj->Get(ctx, propName).ToLocalChecked();

            __recurse_print(isolate, propValue, depth, false, true, true, cached);
            depth -= 2;
            for (int i = 0; i < depth; i++) printf(" ");
        } else depth -= 2;
        printf("}");
        if (!isNext) printf("\n");
    }

    cached.pop_back();
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
