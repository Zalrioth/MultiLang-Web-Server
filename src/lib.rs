#![crate_type = "staticlib"]

use std::io::prelude::*;
use std::fs::File;
use std::path::Path;
use std::str;
use std::io::BufReader;
use std::ffi::CString;
use std::ffi::CStr;
use std::os::raw::c_char;

//https://stackoverflow.com/questions/24145823/rust-ffi-c-string-handling
//https://jvns.ca/blog/2016/01/18/calling-c-from-rust/
//https://stackoverflow.com/questions/25383488/how-to-match-a-string-against-string-literals-in-rust
//https://www.reddit.com/r/rust/comments/3t2pmy/how_do_i_get_the_last_two_characters_in_a_string/
//https://users.rust-lang.org/t/how-to-get-a-substring-of-a-string/1351
//https://stackoverflow.com/questions/41037114/why-do-i-get-an-error-about-non-exhaustive-patterns
//https://doc.rust-lang.org/std/option/enum.Option.html

extern "C" {
    pub fn sendMessage(client: u32, message: *const c_char);
    pub fn sendData(client: u32, message: *const c_char, length: usize);
    pub fn shutdownClient(client: u32);
}

#[no_mangle]
pub extern "C" fn handle_client(client: u32, request: *const c_char) {

    let seek_file: String =
        unsafe { str::from_utf8(CStr::from_ptr(request).to_bytes()).unwrap().to_owned() };

    println!("\x1B[36mRead from disk: {:?}\x1B[37m", seek_file);

    let mut file = String::from("html");
    if seek_file == "/" {
        file.push_str("/index.html");
    } else {
        file.push_str(seek_file.as_str());
    }

    let f = File::open(file.as_str()).expect("Unable to open file");
    let mut data = BufReader::new(f);
    let mut contents = Vec::new();
    let _ = data.read_to_end(&mut contents).expect("Unable to read string");

    let mut build_message = String::from("HTTP/1.0 200 OK\r\nServer: Ingot\r\n");
    build_message.push_str("Content-length: ");
    build_message.push_str(&contents.len().to_string());
    build_message.push_str("\r\n");

    //let last_four = &seek_file[..4];

    let p = Path::new(file.as_str()).extension().unwrap().to_str().unwrap();

    println!("{:?}", p);

    match p {
        "png" => build_message.push_str("Content-Type: image/png\r\n"),
        "ico" => build_message.push_str("Content-Type: image/x-icon\r\n"),
        "html" => build_message.push_str("Content-Type: text/html\r\n"),
        _ => println!("Unknown"),
    }

    build_message.push_str("Connection: close\r\n\r\n");

    let send_message = CString::new(build_message).unwrap();
    unsafe {
        sendMessage(client, send_message.as_ptr());
    }

    let data_length = contents.len();
    unsafe {
        let send_data = CString::from_vec_unchecked(contents);
        sendData(client, send_data.as_ptr(), data_length);
    }
    unsafe {
        shutdownClient(client);
    }
}
