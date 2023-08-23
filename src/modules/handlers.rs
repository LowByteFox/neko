use super::super::GLOBALS;

pub fn resolver<'a>(ctx: v8::Local<'a, v8::Context>,
                specifier: v8::Local<'a, v8::String>,
                assertions: v8::Local<'a, v8::FixedArray>,
                referer: v8::Local<'a, v8::Module>) -> Option<v8::Local<'a, v8::Module>> {
    let isolate = unsafe {
        let isol: *mut v8::OwnedIsolate = GLOBALS.with(|g| {
            g.borrow().isolate_ptr
        });

        &mut *isol
    };

    println!("{}", specifier.to_rust_string_lossy(isolate));
    return None;
}
