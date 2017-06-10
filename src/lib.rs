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
//https://en.wikipedia.org/wiki/List_of_HTTP_status_codes
//https://rustbyexample.com/flow_control/match.html
//http://php.net/manual/en/function.mime-content-type.php

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

    let mut build_message = String::from("HTTP/1.0 ");

    let mut file = String::from("html");
    if seek_file == "/" {
        file.push_str("/index.html");
    } else {
        file.push_str(seek_file.as_str());
    }

    let p = Path::new(file.as_str());

    if p.exists() {
        let pl = p.extension()
            .unwrap()
            .to_str()
            .unwrap();

        let f = File::open(file.as_str()).expect("Unable to open file");
        let mut data = BufReader::new(f);
        let mut contents = Vec::new();
        let _ = data.read_to_end(&mut contents).expect("Unable to read string");

        build_message.push_str("200 OK\r\n");
        build_message.push_str("Content-length: ");
        build_message.push_str(&contents.len().to_string());
        build_message.push_str("\r\n");
        build_message.push_str("Server: Ingot\r\n");

        println!("{:?}", p);

         build_message.push_str(match pl {
            "txt" => "text/plain",
            "htm" => "text/html",
            "html" => "text/html",
            "php" => "text/html",
            "css" => "text/css",
            "js" => "application/javascript",
            "json" => "application/json",
            "xml" => "application/xml",
            "swf" => "application/x-shockwave-flash",
            "flv" => "video/x-flv",

            // images
            "png" => "image/png",
            "jpe" => "image/jpeg",
            "jpeg" => "image/jpeg",
            "jpg" => "image/jpeg",
            "gif" => "image/gif",
            "bmp" => "image/bmp",
            "ico" => "image/vnd.microsoft.icon",
            "tiff" => "image/tiff",
            "tif" => "image/tiff",
            "svg" => "image/svg+xml",
            "svgz" => "image/svg+xml",

            // archives
            "zip" => "application/zip",
            "rar" => "application/x-rar-compressed",
            "exe" => "application/x-msdownload",
            "msi" => "application/x-msdownload",
            "cab" => "application/vnd.ms-cab-compressed",

            // audio/video
            "mp3" => "audio/mpeg",
            "qt" => "video/quicktime",
            "mov" => "video/quicktime",

            // adobe
            "pdf" => "application/pdf",
            "psd" => "image/vnd.adobe.photoshop",
            "ai" => "application/postscript",
            "eps" => "application/postscript",
            "ps" => "application/postscript",

            // ms office
            "doc" => "application/msword",
            "rtf" => "application/rtf",
            "xls" => "application/vnd.ms-excel",
            "ppt" => "application/vnd.ms-powerpoint",

            // open office
            "odt" => "application/vnd.oasis.opendocument.text",
            "ods" => "application/vnd.oasis.opendocument.spreadsheet",
            _ => "unknown/unknown",
        });

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
    } else {
        build_message.push_str("404 Not Found\r\n");
        build_message.push_str("Server: Ingot\r\n");
        build_message.push_str("Connection: close\r\n\r\n");

        let send_message = CString::new(build_message).unwrap();
        unsafe {
            sendMessage(client, send_message.as_ptr());
        }
    }

    unsafe {
        shutdownClient(client);
    }
}
