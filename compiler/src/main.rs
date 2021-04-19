#[macro_use]
extern crate serde_derive;
#[macro_use]
extern crate lazy_static;
#[macro_use]
extern crate bitfield;

mod bar;
mod compile;
mod dict;
mod freemap;
mod hash;
mod keycode;
mod rule;
mod stroke;

use std::fs::File;
use std::io::{Seek, SeekFrom};

use clap::{App, Arg, SubCommand};

use dict::Dict;
use rule::{apply_rules, Dict as RuleDict, Rules};
use stroke::Stroke;

fn main() {
    let matches = App::new("compile-steno")
        .subcommand(SubCommand::with_name("test").arg(Arg::with_name("stroke").required(true)))
        .subcommand(SubCommand::with_name("hash").arg(Arg::with_name("strokes").required(true)))
        .subcommand(
            SubCommand::with_name("compile")
                .arg(Arg::with_name("input").required(true).multiple(true).min_values(1))
                .arg(Arg::with_name("output").required(true)),
        )
        .subcommand(
            SubCommand::with_name("apply-rules")
                .arg(Arg::with_name("rules").required(true))
                .arg(Arg::with_name("from").required(true))
                .arg(Arg::with_name("to").required(true)),
        )
        .get_matches();
    match matches.subcommand() {
        ("compile", Some(m)) => {
            let input_files = m.values_of("input").unwrap();
            let output_file = m.value_of("output").unwrap();

            let inputs: Vec<_> = input_files
                .map(|f| {
                    (f, serde_json::from_reader(File::open(f).expect("input file")).expect("parse json"))
                })
                .collect();
            let dict = Dict::parse_from_json(inputs).unwrap();
            let mut output_file = File::create(output_file).expect("output file");
            compile::to_writer(dict, &mut output_file).expect("write output");
            println!("Size: {}", output_file.seek(SeekFrom::Current(0)).unwrap());
        }
        ("apply-rules", Some(m)) => {
            let rules: Rules = serde_json::from_reader(
                File::open(m.value_of("rules").unwrap()).expect("Cannot open rules file!"),
            )
            .expect("Rules file not valid JSON!");
            let source: RuleDict = serde_json::from_reader(
                File::open(m.value_of("from").unwrap()).expect("Cannot open dict!"),
            )
            .expect("Dict file not valid JSON!");
            let output_dict = apply_rules(&rules, &source);
            println!("\nSize: {}", output_dict.len());
            let output_file =
                File::create(m.value_of("to").unwrap()).expect("Cannot create output file!");
            serde_json::to_writer(output_file, &output_dict).expect("Write output file");
        }
        ("test", Some(m)) => {
            let stroke: Stroke = m.value_of("stroke").unwrap().parse().unwrap();
            println!("{:x}", stroke.raw());
        }
        ("hash", Some(m)) => {
            let strokes: String = m.value_of("strokes").unwrap().parse().unwrap();
            let strokes = strokes
                .split('/')
                .map(|stroke| stroke.parse().unwrap())
                .collect::<Vec<_>>();
            dbg!(&strokes);
            println!("{:x}", stroke::hash_strokes(&strokes));
        }
        (cmd, _) => panic!("{}", cmd),
    }
}
