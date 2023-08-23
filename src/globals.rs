use std::collections::HashMap;

#[derive(Debug)]
pub struct SharedGlobals<'a> {
    pub last_script_id: i32,
    pub modules: HashMap<i32, super::modules::ModuleWrapper<'a>>,
    pub cached_modules: HashMap<String, i32>
}
