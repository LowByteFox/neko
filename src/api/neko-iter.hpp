#ifndef NEKO_ITER_HPP
#define NEKO_ITER_HPP

#include <v8.h>
#include <vector>

namespace neko {
    class V8ArrayIterator: public std::vector<v8::Local<v8::Value>>  {
        public:
            V8ArrayIterator(const v8::Local<v8::Array>& in, 
                    const v8::Local<v8::Context>& ctx,
                    int lenMod = 0) {
                int len = in->Length() - lenMod;
                for (int i = 0; i < len; i++) {
                    this->push_back(in->Get(ctx, i).ToLocalChecked());
                }
            }
    };
}

#endif
