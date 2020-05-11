#[macro_use]
extern crate lazy_static;
#[macro_use]
extern crate bitfield;
#[allow(dead_code)]

mod dict;
mod stroke;
mod compile;
mod hashmap;

use std::fs::File;

use clap::{App, Arg};
use serde_json::Value;
use primes::{Sieve, PrimeSet};

use dict::Dict;
use compile::IR;
use hashmap::LPHashMap;
use stroke::Stroke;

fn hash_strokes(input: &str) -> u32 {
    let bytes = input
        .split('/')
        .flat_map(|stroke| stroke.parse::<Stroke>().unwrap().raw().to_le_bytes().to_vec().into_iter())
        .collect::<Vec<_>>();
    let mut hash = 0x811c9dc5u32;
    for byte in bytes.into_iter() {
        hash *= 0x01000193;
        hash ^= byte as u32;
    }
    hash
}

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

    // let app = App::new("compile-steno")
    //     .arg(Arg::with_name("test").required(true));
    // let matches = app.get_matches();
    // let test_str = matches.value_of("test").unwrap();
    // let (attr, s) = Dict::parse_entry(test_str);
    // println!("attr: {:?}, str: {}", attr, s);
}
