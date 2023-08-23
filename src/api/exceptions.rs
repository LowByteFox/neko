use std::path::Path;

use super::super::api;

use super::super::GLOBALS;

pub fn print_exception(scope: &mut v8::HandleScope, exception: v8::Local<v8::Value>, path: &str) {
    if !exception.is_native_error() {
        eprintln!("Error: {}", exception.to_rust_string_lossy(scope));
        return;
    }

    let obj = exception.to_object(scope).unwrap();
    let name_value = v8::String::new(scope, "name").unwrap().into();
    let name = obj.get(scope, name_value).unwrap();

    let msg = v8::Exception::create_message(scope, exception);
    let line = msg.get_line_number(scope).unwrap();
    let col = msg.get_start_column() as i32;
    let file = Path::new(path);
    let filename = file.file_name().unwrap();

    eprintln!("──────────────────────────────────────");
    eprintln!("[{}:{}:{}] {}", filename.to_str().unwrap(), line, col, exception.to_rust_string_lossy(scope));

    let stack = msg.get_stack_trace(scope).unwrap();
    let length = stack.get_frame_count();

    let glob = GLOBALS.lock().unwrap();

    for i in 0..length {
        let frame = stack.get_frame(scope, i).unwrap();
        let func_name = frame.get_function_name(scope);
        let id = frame.get_script_id() as i32;
        let has = glob.module_paths_id.get(&id);
        match has {
            Some(v) => {
                let file = Path::new(v);
                let filename = file.file_name().unwrap();
                match func_name {
                    Some(f) => {
                        eprintln!("    at {}() [{}:{}:{}]", f.to_rust_string_lossy(scope), filename.to_str().unwrap(),
                        frame.get_line_number(), frame.get_column());
                    }
                    None => {
                        eprintln!("    at {} [{}:{}]", filename.to_str().unwrap(), frame.get_line_number(), frame.get_column());
                    }
                }
            }
            None => continue
        }
    }

    if col == -1 {
        eprintln!("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
        return;
    }

    eprintln!("━━━━━━━━━━━━━━━━ code ━━━━━━━━━━━━━━━━");
    let source = api::read_file(path);

    if source.len() == 0 {
        return;
    }

    let mut start_line: i32 = (line as i32) - 5;
    if start_line < 0 {
        start_line = 0;
    }

    let lines: Vec<&str> = source.lines().collect();
    let lines_count = lines.len();

    let line_str = line.to_string();
    let width = line_str.len();

    for i in start_line..lines_count as i32 {
        eprintln!("{:width$} │{}", i, lines[i as usize], width = width);
    }
    eprint!("{:width$} │", line, width = width);

    for _ in 0..col {
        eprint!(" ");
    }

    for _ in col..msg.get_end_column() as i32 {
        eprint!("^");
    }

    eprintln!("");
    eprintln!("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");

    drop(glob);

    if name.to_rust_string_lossy(scope) == "AggregateError" {
        let key = v8::String::new(scope, "errors").unwrap().into();
        let errors = obj.get(scope, key).unwrap().to_object(scope).unwrap();
        let key2 = v8::String::new(scope, "length").unwrap().into();
        let errors_len = errors.get(scope, key2).unwrap();
        for i in 0..errors_len.to_number(scope).unwrap().value() as i32 {
            let key3 = v8::Number::new(scope, i as f64);
            let exc = errors.get(scope, key3.into()).unwrap();
            print_exception(scope, exc, path);
        }
    }
}
