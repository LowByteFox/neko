#ifndef NEKO_HPP
#define NEKO_HPP

#include <v8.h>

namespace neko {
    char* jsToString(v8::Isolate* isolate, v8::Local<v8::Value> v);
}

#endif
