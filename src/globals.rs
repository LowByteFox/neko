use std::collections::HashMap;

#[derive(Debug)]
pub struct SharedGlobals {
    pub last_script_id: i32,
    pub module_paths_id: HashMap<i32, String>,
}
