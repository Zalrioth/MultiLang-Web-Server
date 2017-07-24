#![crate_type = "staticlib"]

use std::io::prelude::*;
use std::fs::File;
use std::path::Path;
use std::str;
use std::io::BufReader;
use std::ffi::CString;
use std::ffi::CStr;
use std::os::raw::c_char;

extern "C" {
    pub fn send_message(client: u32, message: *const c_char) -> i32;
    pub fn send_data(client: u32, message: *const c_char, length: usize) -> i32;
    pub fn shutdown_client(client: u32);
}

#[no_mangle]
pub extern "C" fn handle_client(client: u32,
                                command: *const c_char,
                                get_file: *const c_char,
                                host: *const c_char) {

    let seek_file: String =
        unsafe { str::from_utf8(CStr::from_ptr(get_file).to_bytes()).unwrap().to_owned() };

    let host_folder: String =
        unsafe { str::from_utf8(CStr::from_ptr(host).to_bytes()).unwrap().to_owned() };

    //println!("\x1B[36mRead from disk: {:?}\x1B[37m", seek_file);

    let mut build_message = String::new();
    let mut contents = Vec::new();

    let mut file = String::from("html/");

    file.push_str(host_folder.as_str());

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
        let _ = data.read_to_end(&mut contents).expect("Unable to read string");

        build_message.push_str("HTTP/1.0 200 OK\r\n");

        //println!("{:?}", p);

        build_message.push_str(match pl {
                                   // General
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

                                   // Images
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

                                   // Archives
                                   "zip" => "application/zip",
                                   "rar" => "application/x-rar-compressed",
                                   "exe" => "application/x-msdownload",
                                   "msi" => "application/x-msdownload",
                                   "cab" => "application/vnd.ms-cab-compressed",

                                   // Audio/Video
                                   "mp3" => "audio/mpeg",
                                   "qt" => "video/quicktime",
                                   "mov" => "video/quicktime",

                                   // Adobe
                                   "pdf" => "application/pdf",
                                   "psd" => "image/vnd.adobe.photoshop",
                                   "ai" => "application/postscript",
                                   "eps" => "application/postscript",
                                   "ps" => "application/postscript",

                                   // MS Office
                                   "doc" => "application/msword",
                                   "rtf" => "application/rtf",
                                   "xls" => "application/vnd.ms-excel",
                                   "ppt" => "application/vnd.ms-powerpoint",

                                   // Open Office
                                   "odt" => "application/vnd.oasis.opendocument.text",
                                   "ods" => "application/vnd.oasis.opendocument.spreadsheet",
                                   _ => "unknown/unknown",
                               });
    } else {
        build_message.push_str("HTTP/1.0 404 Not Found\r\n");
    }

    build_message.push_str("Content-length: ");
    build_message.push_str(&contents.len().to_string());
    build_message.push_str("\r\n");
    build_message.push_str("Server: Ingot\r\n");
    build_message.push_str("Connection: close\r\n\r\n");

    unsafe {
        let send_message_bits = CString::new(build_message).unwrap();
        if send_message(client, send_message_bits.as_ptr()) == 0 {
            let content_length = contents.len();
            let send_data_bits = CString::from_vec_unchecked(contents);
            if send_data(client, send_data_bits.as_ptr(), content_length) == 0 {
                shutdown_client(client);
            }
        }
    }
}
