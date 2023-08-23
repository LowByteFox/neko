use super::ModuleWrapper;

impl<'a> super::ModuleWrapper<'a> {
    pub fn get_module(&self) -> Option<v8::Local<'a, v8::Module>> {
        return self.module;
    }

    pub fn get_module_id(&self) -> i32 {
        return self.id;
    }

    pub fn set_module(&mut self, module: v8::Local<'a, v8::Module>) {
        self.module = Some(module);
    }

    pub fn get_meta(&self) -> &Vec<super::ModuleMetadata<'a>> {
        return &self.meta;
    }

    pub fn get_metadata(&self, key: &str) -> Option<&super::ModuleMetadata<'a>> {
        let cached = String::from(key);
        for m in self.meta.iter() {
            if m.string_key == cached {
                return Some(m);
            }
        }

        return None;
    }

    pub fn set_metadata(&mut self, scope: &mut v8::HandleScope<'a>, key: &str, value: v8::Local<'_, v8::Value>) {
        let val = super::ModuleMetadata {
            string_key: String::from(key),
            key: v8::String::new(scope, key).unwrap().into(),
            value: v8::Global::new(scope, value)
        };

        self.meta.push(val);
    }

    pub fn compile_module(scope: &mut v8::HandleScope<'a>, code: String) -> ModuleWrapper<'a> {
        let mut module = ModuleWrapper {
            meta: Vec::new(),
            module: None,
            id: 0
        };

        let empty_int = v8::Integer::new(scope, 0).into();
        let v8_null = v8::null(scope).into();

        let origin = &mut v8::ScriptOrigin::new(scope,
                                                empty_int,
                                                0, 0, false, 1, v8_null,
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
