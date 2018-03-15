extern crate clap;

use std::error::Error;
use std::io::{Read, Write};

static EXIT_SUCCESS: i32 = 0;
static EXIT_FAILURE: i32 = 1;

struct NamedFile {
    file: std::fs::File,
    filename: String,
}

fn open_files(input_filename: &str, output_filename: &str)
-> (NamedFile, NamedFile) {
    let input_file = match std::fs::File::open(input_filename) {
        Ok(file) => file,
        Err(error) => {
            eprintln!("error while opening file '{}': {}", input_filename,
                      error.description());
            std::process::exit(EXIT_FAILURE);
        }
    };

    let output_file = match std::fs::File::create(output_filename) {
        Ok(file) => file,
        Err(error) => {
            eprintln!("error while creating file '{}': {}", output_filename,
                      error.description());
            std::process::exit(EXIT_FAILURE);
        }
    };

    (NamedFile{ file: input_file, filename: String::from(input_filename) },
     NamedFile{ file: output_file, filename: String::from(output_filename) })
}

fn read_contents(mut file: NamedFile) -> String {
    let mut contents = String::new();
    match file.file.read_to_string(&mut contents) {
        Ok(count) => println!("read {} bytes from '{}'", count,
                              file.filename),
        Err(error) => {
            eprintln!("error while reading from '{}': {}", file.filename,
                      error.description());
            std::process::exit(EXIT_FAILURE);
        }
    };

    contents
}

fn write_buffer(mut file: NamedFile, buffer: &[u8]) {
    match file.file.write_all(buffer) {
        Ok(_) => println!("wrote {} bytes to '{}'", buffer.len(),
                          file.filename),
        Err(error) => {
            eprintln!("error while writing to '{}': {}", file.filename,
                      error.description());
            std::process::exit(EXIT_FAILURE);
        }
    };
}

fn replace(text: &str, to_find: &str, replacement: &str) -> String {
    let mut replaced = String::with_capacity(text.len() * 3 / 2);

    for (index, substring) in text.split(to_find).enumerate() {
        if index != 0 {
            replaced.push_str(replacement);
        }

        replaced.push_str(substring);
    }

    replaced
}

fn main() {
    let parser = clap::App::new("regex-replace")
        .version("0.1.0")
        .author("Gregory Meyer <gregjm@umich.edu>")
        .about("Replace all instances of ',' in an input file with ',\\n'")
        .arg(clap::Arg::with_name("INPUT")
                 .help("the file to read from")
                 .required(true)
                 .index(1))
        .arg(clap::Arg::with_name("OUTPUT")
                 .help("the file to write to")
                 .required(true)
                 .index(2))
        .get_matches();

    let input_filename = parser.value_of("INPUT").unwrap();
    let output_filename = parser.value_of("OUTPUT").unwrap();

    let (input_file, output_file) = open_files(input_filename, output_filename);

    let input_contents = read_contents(input_file);

    let start = std::time::Instant::now();
    let output_contents = replace(input_contents.as_str(), ",", ",\n");
    let elapsed = start.elapsed();

    let seconds = elapsed.as_secs();
    let nanos = elapsed.subsec_nanos();

    let elapsed_seconds = seconds as f64 + (nanos as f64) * 1e-9;

    write_buffer(output_file, output_contents.as_bytes());

    println!("took {} seconds", elapsed_seconds);
}
