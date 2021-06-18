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
mod orthography;
#[cfg(feature = "patching")]
mod rule;
mod stroke;

use std::fs::File;
use std::io::{Seek, SeekFrom};

use clap::{App, AppSettings, Arg, SubCommand};

use dict::Dict;
#[cfg(feature = "patching")]
use rule::{apply_rules, Dict as RuleDict, Rules};
use stroke::{Stroke, Strokes};

fn main() {
    let app = App::new("compile-steno")
        .subcommand(SubCommand::with_name("test").arg(Arg::with_name("stroke").required(true)))
        .subcommand(SubCommand::with_name("hash").arg(Arg::with_name("strokes").required(true)))
        .subcommand(SubCommand::with_name("test-parse").arg(Arg::with_name("text").required(true)))
        .subcommand(SubCommand::with_name("hash-str").arg(Arg::with_name("str").required(true)))
        .subcommand(
            SubCommand::with_name("compile")
                .arg(
                    Arg::with_name("input")
                        .required(true)
                        .multiple(true)
                        .min_values(1),
                )
                .arg(Arg::with_name("output").required(true)),
        )
        .setting(AppSettings::SubcommandRequiredElseHelp);
    #[cfg(feature = "patching")]
    let app = app.subcommand(
            SubCommand::with_name("apply-rules")
                .arg(Arg::with_name("rules").required(true))
                .arg(Arg::with_name("from").required(true))
                .arg(Arg::with_name("to").required(true)),
        );
    let matches = app.get_matches();
    match matches.subcommand() {
        ("compile", Some(m)) => {
            let input_files = m.values_of("input").unwrap();
            let output_file = m.value_of("output").unwrap();

            let inputs: Vec<_> = input_files
                .map(|f| {
                    (
                        f,
                        serde_json::from_reader(File::open(f).expect("input file"))
                            .expect("parse json"),
                    )
                })
                .collect();
            let dict = match Dict::parse(Dict::merge_dicts(inputs)) {
                Ok(d) => d,
                Err(e) => {
                    eprintln!("{}", e);
                    return;
                }
            };
            let mut output_file = File::create(output_file).expect("output file");
            if let Err(e) = compile::to_writer(dict, &mut output_file) {
                eprintln!("{}", e);
                return;
            };
            println!("Size: {}", output_file.seek(SeekFrom::Current(0)).unwrap());
        }
        #[cfg(feature = "patching")]
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
            println!("{:x}", stroke::hash_strokes(&Strokes(strokes)));
        }
        ("hash-str", Some(m)) => {
            let s: String = m.value_of("str").unwrap().parse().unwrap();
            dbg!(&s);
            let hash = hash::hash(s.as_bytes(), None);
            println!("{:x}", hash);
            let index = hash as usize % (orthography::BUCKET_CAP - 1);
            println!("{:x}", index);
        }
        ("test-parse", Some(m)) => {
            let input = m.value_of("text").unwrap();
            match dbg!(crate::dict::parse::parse_entry(input)) {
                Err(e) => {
                    println!("{}", e.format_error(input));
                }
                Ok(v) => {
                    println!("{:?}", v);
                }
            }
        }
        _ => panic!(),
    }
}
