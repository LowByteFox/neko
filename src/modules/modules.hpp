#ifndef MODULES_HPP
#define MODULES_HPP

#include <v8.h>

namespace modules {
    void metadataHook(v8::Local<v8::Context> ctx,
            v8::Local<v8::Module> mod, v8::Local<v8::Object> meta);

    v8::MaybeLocal<v8::Module> moduleResolver(
        v8::Local<v8::Context> ctx,
        v8::Local<v8::String> specifier,
        [[maybe_unused]] v8::Local<v8::FixedArray> import_assertions,
        v8::Local<v8::Module> ref
    );
}

#endif
