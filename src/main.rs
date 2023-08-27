use std::collections::HashMap;
use std::env;
use std::process::exit;
use std::sync::Mutex;

use globals::{SharedGlobals, WrappedGlobalModule};
use lazy_static::lazy_static;

use crate::utils::normalize_path;

mod api;
mod modules;
mod globals;
mod utils;

const VERSION: &str = env!("CARGO_PKG_VERSION");

lazy_static! {
    pub static ref GLOBALS: Mutex<SharedGlobals> = {
        let glob = SharedGlobals {
            last_script_id: 0,
            module_paths_id: HashMap::new(),
            module_cache: HashMap::new()
        };
        Mutex::new(glob)
    };
}

fn evaluate(isolate: &mut v8::Isolate, file: &str) {
    isolate.set_capture_stack_trace_for_uncaught_exceptions(true, 10);
    let handle_scope = &mut v8::HandleScope::new(isolate);
    let global = v8::ObjectTemplate::new(handle_scope);

    let ctx = v8::Context::new_from_template(handle_scope, global);
    ctx.set_allow_generation_from_strings(false);
    let scope = &mut v8::ContextScope::new(handle_scope, ctx);

    let mut out = api::read_file(file);
    out = String::from(out.trim());
    if out.len() == 0 {
        eprintln!("Si v pici");
        return;
    }

    let module = modules::ModuleWrapper::compile_module(scope, out);

    let mut cwd = env::current_dir().unwrap();
    cwd.push(file);
    cwd = normalize_path(cwd.as_path());

    let mut val = GLOBALS.lock().unwrap();

    val.module_paths_id.insert(module.id, String::from(cwd.to_str().unwrap()));
    val.module_cache.insert(String::from(cwd.to_str().unwrap()), WrappedGlobalModule(v8::Global::new(scope, module.get_module().unwrap())));

    drop(val);

    let local_module = &mut module.get_module().unwrap();
    let try_catch = &mut v8::TryCatch::new(scope);
    let _ = local_module.instantiate_module(try_catch, modules::resolver);
    if try_catch.has_caught() {
        let exception = try_catch.exception().unwrap();
        api::print_exception(try_catch, exception, cwd.to_str().unwrap());
        exit(1);
    }
    let _ = local_module.evaluate(try_catch);
    if local_module.get_status() == v8::ModuleStatus::Errored {
        let exception = local_module.get_exception();
        api::print_exception(try_catch, exception, cwd.to_str().unwrap());
        exit(1);
    }
}

fn run(file: &str) {
    let platform = v8::new_default_platform(0, false).make_shared();
    v8::V8::initialize_platform(platform);
    v8::V8::initialize();

    let isolate = &mut v8::Isolate::new(Default::default());

    evaluate(isolate, file);
}

fn print_help() {
    println!(r#"neko - JavaScript runtime for the modern age

Usage: neko [OPTIONS] (ARGS)

OPTIONS:
  help, -h          Display this help message
  version, -v       Display version
  run (SCRIPT)      Execute (SCRIPT)
  create (NAME)     Create a new project (NAME)"#);
}

fn main() {
    let argv: Vec<String> = env::args().collect();
    let argc = argv.len();

    if argc == 1 {
        print_help();
        exit(1);
    }

    for i in 1..argc {
        let arg = &argv[i];
        if arg == "-v" || arg == "version" {
            println!("neko {}", VERSION);
            break;
        } else if arg == "-h" || arg == "help" {
            print_help();
            break;
        } else if arg == "run" {
            if i + 1 < argc {
                let file = &argv[i + 1];
                run(file);
            } else {
                eprintln!("File not provided!");
            }
            break;
        } else if arg == "create" {
            println!("Not yet here hehe");
            break;
        }
    }
}
