mod wrapper;

#[derive(Debug)]
pub struct ModuleMetadata<'a> {
    pub string_key: String,
    pub key: v8::Local<'a, v8::Name>,
    pub value: v8::Global<v8::Value>
}

#[derive(Debug)]
pub struct ModuleWrapper<'a> {
    pub meta: Vec<ModuleMetadata<'a>>,
    pub module: Option<v8::Local<'a, v8::Module>>,
    pub id: i32
}
