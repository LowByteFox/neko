use super::ModuleWrapper;
use super::super::GLOBALS;

impl<'a> super::ModuleWrapper<'a> {
    pub fn get_module(&self) -> Option<v8::Local<'a, v8::Module>> {
        return self.module;
    }

    pub fn set_module(&mut self, module: v8::Local<'a, v8::Module>) {
        self.module = Some(module);
        self.id = module.script_id().unwrap();
    }


    pub fn compile_module(scope: &mut v8::HandleScope<'a>, code: String) -> ModuleWrapper<'a> {
        let mut module = ModuleWrapper {
            module: None,
            id: 0
        };

        let mut val = GLOBALS.lock().unwrap();
        module.id = val.last_script_id;
        val.last_script_id += 1;

        drop(val);

        let empty_int = v8::Integer::new(scope, 0).into();
        let v8_null = v8::null(scope).into();

        let origin = &mut v8::ScriptOrigin::new(scope,
                                                empty_int,
                                                0, 0, false, module.id, v8_null,
                                                false, false, true);

        let source = v8::script_compiler::Source::new(
            v8::String::new(scope, code.trim()).unwrap().into(),
            Some(origin));

        let try_catch = &mut v8::TryCatch::new(scope);

        let ret = v8::script_compiler::compile_module(try_catch, source);

        if try_catch.has_caught() {
            return module;
        }

        module.set_module(ret.unwrap().into());

        return module;
    }
}
