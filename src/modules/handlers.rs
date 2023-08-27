use std::path::Path;

use crate::{GLOBALS, utils::normalize_path, api::read_file, globals::WrappedGlobalModule};

use super::ModuleWrapper;

pub fn resolver<'a>(ctx: v8::Local<'a, v8::Context>,
                specifier: v8::Local<'a, v8::String>,
                _assertions: v8::Local<'a, v8::FixedArray>,
                referer: v8::Local<'a, v8::Module>) -> Option<v8::Local<'a, v8::Module>> {
    let scope = unsafe { &mut v8::CallbackScope::new(ctx) };

    let mut base = specifier.to_rust_string_lossy(scope);
    let val = GLOBALS.lock().unwrap();
    let original_path = val.module_paths_id.get(&referer.script_id().unwrap()).unwrap();

    let name = base.clone();

    if base.as_bytes()[0] != b'/' {
        base = String::from(Path::new(original_path).parent().unwrap().to_str().unwrap());
        base += "/";
        base += name.as_str();
        base = String::from(normalize_path(Path::new(&String::from(base))).to_str().unwrap());
    }

    match val.module_cache.get(&base) {
        Some(module) => {
            return Some(v8::Local::new(scope, module.get_module()));
        },
        None => ()
    }

    drop(val);

    let code = read_file(base.as_str());
    let module = ModuleWrapper::compile_module(scope, code);

    let mut val = GLOBALS.lock().unwrap();
    val.module_paths_id.insert(module.id, base.clone());
    val.module_cache.insert(base, WrappedGlobalModule(v8::Global::new(scope, module.get_module().unwrap())));
    drop(val);

    return module.get_module();
}
