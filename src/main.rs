use std::env;
use std::process::exit;

mod api;
mod modules;

const VERSION: &str = env!("CARGO_PKG_VERSION");

fn lol(scope: &mut v8::HandleScope, args: v8::FunctionCallbackArguments, mut retval: v8::ReturnValue) {
    let msg = args.get(0).to_string(scope).unwrap().to_rust_string_lossy(scope);
    println!("Log: {}", msg);
    retval.set(v8::String::new(scope, "haha").unwrap().into());
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
