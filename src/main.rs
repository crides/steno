mod dict;
mod stroke;
mod compile;

use std::fs::File;

use clap::{App, Arg};
use serde_json::Value;

use dict::Dict;
use compile::IR;

fn main() {
    let app = App::new("compile-steno")
        .arg(Arg::with_name("input").required(true))
        .arg(Arg::with_name("output").required(true));
    let matches = app.get_matches();
    let input_file = matches.value_of("input").unwrap();
    let output_file = matches.value_of("output").unwrap();

    let input_file = File::open(input_file).expect("input file");
    let input: Value = serde_json::from_reader(input_file).expect("read json");
    let dict = Dict::parse_from_json(input.as_object().expect("json is not map")).unwrap();
    let ir = IR::from_dict(dict);
    ir.write(&mut File::create(output_file).expect("output file"))
        .expect("write output");
}
