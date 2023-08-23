use std::{fs::File, io::Read};

pub fn read_file(path: &str) -> String {
    let mut file = match File::open(path) {
        Ok(v) => v,
        Err(_) => {
            return String::from("");
        }
    };
    let mut contents = String::new();
    let _ = match file.read_to_string(&mut contents) {
        Ok(s) => s,
        Err(_) => 0
    };

    return contents;
}
