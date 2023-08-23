mod wrapper;
mod handlers;

#[derive(Debug)]
pub struct ModuleWrapper<'a> {
    pub module: Option<v8::Local<'a, v8::Module>>,
    pub id: i32
}

pub use handlers::resolver;
