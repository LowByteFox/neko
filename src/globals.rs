use std::collections::HashMap;

#[derive(Debug)]
pub struct WrappedGlobalModule(pub v8::Global<v8::Module>);
unsafe impl Send for WrappedGlobalModule {}

impl WrappedGlobalModule {
    pub fn get_module(&self) -> &v8::Global<v8::Module> {
        &self.0
    }
}

#[derive(Debug)]
pub struct SharedGlobals {
    pub last_script_id: i32,
    pub module_paths_id: HashMap<i32, String>,
    pub module_cache: HashMap<String, WrappedGlobalModule>
}
