pub fn resolver<'a>(ctx: v8::Local<'a, v8::Context>,
                specifier: v8::Local<'a, v8::String>,
                assertions: v8::Local<'a, v8::FixedArray>,
                referer: v8::Local<'a, v8::Module>) -> Option<v8::Local<'a, v8::Module>> {
    return None;
}
