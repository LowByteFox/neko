#include "neko.hpp"

#include <v8.h>

namespace neko {
    char* jsToString(v8::Isolate* isolate, v8::Local<v8::Value> v)
    {
        v8::String::Utf8Value val(isolate, v);
        char* maybe = *val;
        return maybe ? maybe : (char *) "<Unknown String?>";
    }
}
