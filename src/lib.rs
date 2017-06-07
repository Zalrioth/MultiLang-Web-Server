#![crate_type = "staticlib"]

use std::io::prelude::*;
use std::fs::File;
use std::str;
use std::io::BufReader;
use std::ffi::CString;
use std::ffi::CStr;
use std::os::raw::c_char;

//https://stackoverflow.com/questions/24145823/rust-ffi-c-string-handling
//https://jvns.ca/blog/2016/01/18/calling-c-from-rust/

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

    let f = File::open(file).expect("Unable to open file");
    let mut data = BufReader::new(f);
    let mut contents = Vec::new();
    let _ = data.read_to_end(&mut contents).expect("Unable to read string");
    // send http version
    let http_version = CString::new("HTTP/1.0 200 OK\r\n").unwrap();
    unsafe {
        sendMessage(client, http_version.as_ptr());
    }

    // Server
    let http_version = CString::new("Server: Ingot\r\n").unwrap();
    unsafe {
        sendMessage(client, http_version.as_ptr());
    }

    // send content length
    let mut foo = "Content-length: ".to_string();
    foo.push_str(&contents.len().to_string());
    foo.push_str("\r\n");
    //let content_length = CString::from(foo.as_str().unwrap()).unwrap();
    let content_length = CString::new(foo).unwrap();
    unsafe {
        sendMessage(client, content_length.as_ptr());
    }

    // send content type
    // TODO: Get filetype by copying from buffer, won't need to do checks
    if seek_file == "/favicon.ico" {
        let http_version = CString::new("Content-Type: image/x-icon\r\n").unwrap();
        unsafe {
            sendMessage(client, http_version.as_ptr());
        }
        //t_stream.write(&http_version.into_bytes()).unwrap();
    } else {
        let http_version = CString::new("Content-Type: text/html\r\n").unwrap();
        unsafe {
            sendMessage(client, http_version.as_ptr());
        }
        //t_stream.write(&http_version.into_bytes()).unwrap();
    }

    let http_version = CString::new("Content-Type: text/html\r\n").unwrap();
    unsafe {
        sendMessage(client, http_version.as_ptr());
    }

    // send connection close
    let http_version = CString::new("Connection: close\r\n\r\n").unwrap();
    unsafe {
        sendMessage(client, http_version.as_ptr());
    }
    let contents_num = contents.len();
    unsafe {
        let send_test = CString::from_vec_unchecked(contents);

        // send data
        sendData(client, send_test.as_ptr(), contents_num);
    }
    unsafe {
        shutdownClient(client);
    }
}
