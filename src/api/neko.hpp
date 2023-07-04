#ifndef NEKO_HPP
#define NEKO_HPP

#include <v8.h>
#include <map>
#include <memory>

namespace neko {

    enum class ExceptionType {
        RANGE = 1,
        REFERENCE,
        SYNTAX,
        TYPE,
        ERROR
    };

    std::unique_ptr<const char*> jsToString(v8::Isolate *isolate, v8::Local<v8::Value> val);

    std::string readFile(const char* name);

    v8::Local<v8::Value> throwException(v8::Local<v8::Context> ctx, const char* message,
            ExceptionType type = ExceptionType::ERROR);

    void printException(v8::Local<v8::Context> ctx, v8::Local<v8::Value> exception);

    void throwAndPrintException(v8::Local<v8::Context> ctx, const char* message,
            ExceptionType type = ExceptionType::ERROR);
}

#endif
