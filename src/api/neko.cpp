#include "neko.hpp"

#include <v8.h>
#include <string>
#include <fstream>
#include <sstream>

namespace neko {
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
