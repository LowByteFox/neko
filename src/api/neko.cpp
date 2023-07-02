#include "neko.hpp"

#include <v8.h>
#include <string>
#include <fstream>
#include <sstream>

namespace neko {
    char* jsToString(v8::Isolate* isolate, v8::Local<v8::Value> v)
    {
        v8::String::Utf8Value val(isolate, v);
        char* maybe = *val;
        return maybe ? maybe : (char *) "<Unknown String?>";
    }

    std::string readFile(const char* name)
    {
        std::ifstream file(name);
        if (!file.good())
            return "";

        std::stringstream ss;

        ss << file.rdbuf();

        std::string out = ss.str();

        file.close();

        return out;
    }
}
