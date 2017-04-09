//https://medium.com/@royalstream/rust-development-using-vs-code-on-os-x-debugging-included-bc10c9863777#.kz4b4ykb8
//https://avacariu.me/articles/2015/rust-echo-server-example
//http://stackoverflow.com/questions/17445485/example-tcp-server-written-in-rust
//https://users.rust-lang.org/t/what-is-right-ways-to-concat-strings/3780/4
//http://brandonio21.com/2016/04/rust-you-probably-meant-to-use-chars-count/
//http://stackoverflow.com/questions/24990520/how-do-i-convert-from-an-integer-to-a-string-in-rust
//https://www.digitalocean.com/community/tutorials/apache-vs-nginx-practical-considerations
//http://valve.github.io/blog/2014/08/26/json-serialization-in-rust-part-2/
//https://github.com/PistonDevelopers/VisualRust/issues/3
//http://stackoverflow.com/questions/27844778/what-can-cause-rusts-tcpsocketwrite-to-return-invalid-input
//https://www.reddit.com/r/rust/comments/3t2pmy/how_do_i_get_the_last_two_characters_in_a_string/
//http://stackoverflow.com/questions/1319766/why-wont-favicon-load-in-firefox
//https://geeksww.com/tutorials/servers/apache_web_http_server/tips_and_tricks/giving_apache_web_server_a_different_name_by_changing_source_code_before_installation.php
//http://stackoverflow.com/questions/17445485/example-tcp-server-written-in-rust
//http://stackoverflow.com/questions/2811006/what-is-a-good-buffer-size-for-socket-programming
//http://stackoverflow.com/questions/32956050/how-to-create-a-static-string-at-compile-time
//http://stackoverflow.com/questions/35169259/how-to-make-compiled-regexp-a-global-variable/35169402
//https://doc.rust-lang.org/rustc-serialize/rustc_serialize/json/

extern crate rustc_serialize;

use std::net::{TcpListener, TcpStream};
use std::io::Write;
use std::io::prelude::*;
use std::fs::File;
use std::thread;
use std::str;
use std::env;
use std::io::BufReader;
use rustc_serialize::json;
use rustc_serialize::json::Json;
use std::ffi::OsString;

#[macro_use]
extern crate lazy_static;

struct Settings {
    web_root: String,
    default_index: String,
}

const SETTINGS_RAW: &'static str = include_str!("settings.conf");

lazy_static! {
static ref SETTINGS: Settings = Settings {
    web_root: json::decode(&Json::from_str(SETTINGS_RAW).unwrap().as_object().unwrap().get("WebRoot").unwrap().to_string()).unwrap(),
    default_index: json::decode(&Json::from_str(SETTINGS_RAW).unwrap().as_object().unwrap().get("DefaultIndex").unwrap().to_string()).unwrap(),
};
}

fn main() {
    println!("Starting Rust Web Processor!");

    let listener = TcpListener::bind("0.0.0.0:8810").unwrap();

    for stream in listener.incoming() {
        match stream {
            Ok(stream) => {
                handle_client(stream);
                /*thread::spawn(|| {
                    let stream = stream;
                    handle_client(stream);
                });*/
            }
            Err(e) => {
                println!("Error: {:?}", e);
            }
        }
    }
}

fn handle_client(stream: TcpStream) {
    // NOTE: When firefox is creating a new empty connection, it's only to prehandshake.
    let mut t_stream = stream;
    println!("Connection from front");

    // NOTE: This buffer size is defacto in tcp/ip programming
    //let mut buffer = vec![0; 8192];
    let mut buffer = vec![0; 1024];
    loop {
        let _ = t_stream.read(&mut buffer);

        //let read_string = utf8_to_string(&buffer[0..1024]);

        let read_string = String::from(str::from_utf8(&buffer).unwrap());

        //let vector: Vec<u8> = Vec::from(buffer);
        //let read_string = String::from_utf8(buffer).unwrap()

        //let read_string = OsString::from(buffer);

        println!("{}", read_string);

        let id = &read_string[..3];
        let request = &read_string[4..];

        println!("id: {}", id);
        println!("request: {}", request);

        let split = request.split_whitespace();

        let vec: Vec<&str> = split.collect();

        let command = String::from(vec[0]);
        let seek_file = String::from(vec[1]);

        if command == "GET" {
            let mut file = String::new();
            if seek_file == "/" {
                file.push_str(&SETTINGS.default_index);

            } else {
                let new_file = String::from(seek_file.to_string());
                file = String::from(new_file.to_string());
            }
            let mut file_location = String::new();

            // NOTE: This will add unneeded consider new solution
            if &SETTINGS.web_root == "local" {
                let mut temper_string =
                    String::from(env::current_dir().unwrap().display().to_string());
                temper_string.push_str("/html");
                println!("{:?}", temper_string);
                file_location.push_str(&temper_string);
            } else {
                file_location.push_str(&SETTINGS.web_root);
            }

            file_location.push_str(&file);

            println!("{:?}", file_location);

            let f = File::open(file_location).expect("Unable to open file");
            let mut data = BufReader::new(f);
            let mut contents = Vec::new();
            let _ = data.read_to_end(&mut contents).expect("Unable to read string");
            // send http version
            let http_version = String::from("HTTP/1.0 200 OK\r\n");
            t_stream.write(&http_version.into_bytes()).unwrap();

            // Server
            let http_version = String::from("Server: Ingot\r\n");
            t_stream.write(&http_version.into_bytes()).unwrap();

            // send content length
            let mut foo = "Content-length: ".to_string();
            foo.push_str(&contents.len().to_string());
            foo.push_str("\r\n");
            let content_length = String::from(foo);
            t_stream.write(&content_length.into_bytes()).unwrap();

            // send content type
            // TODO: Get filetype by copying from buffer, won't need to do checks
            if seek_file == "/favicon.ico" {
                let http_version = String::from("Content-Type: image/x-icon\r\n");
                t_stream.write(&http_version.into_bytes()).unwrap();
            } else {
                let http_version = String::from("Content-Type: text/html\r\n");
                t_stream.write(&http_version.into_bytes()).unwrap();
            }

            // send connection close
            let http_version = String::from("Connection: close\r\n\r\n");
            t_stream.write(&http_version.into_bytes()).unwrap();

            // send data
            t_stream.write(&contents).unwrap();
            //println!("{:?}", contents);
        } else if command == "HEAD" {

        }
    }
}
