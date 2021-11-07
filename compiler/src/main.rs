#[macro_use]
extern crate serde_derive;
#[macro_use]
extern crate lazy_static;
#[macro_use]
extern crate bitfield;

mod bar;
mod compile;
// mod decompile;
mod dict;
mod freemap;
mod hash;
mod orthography;
#[cfg(feature = "patching")]
mod rule;
mod stroke;

use std::fs::File;
use std::io::{Seek, SeekFrom};
use std::path::Path;

use clap::{App, AppSettings, Arg, ArgGroup, SubCommand};

use compile::{BinFile, HexFile, OutputFile, Uf2File};
use dict::Dict;
#[cfg(feature = "patching")]
use rule::{apply_rules, Dict as RuleDict, Rules};
use stroke::{Stroke, Strokes};

enum OutputFormat {
    Hex,
    Bin,
    Uf2,
}

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
                .arg(Arg::with_name("bin").short("b"))
                .arg(Arg::with_name("uf2").short("u"))
                .group(ArgGroup::with_name("format").args(&["bin", "uf2"]))
                .arg(Arg::with_name("output").required(true)),
        )
        // .subcommand(
        //     SubCommand::with_name("decompile")
        //         .arg(Arg::with_name("input").required(true))
        //         .arg(Arg::with_name("output").required(true)),
        // )
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
            let out_fmt = m
                .value_of("format")
                .map(|s| match s {
                    "bin" => OutputFormat::Bin,
                    "uf2" => OutputFormat::Uf2,
                    "hex" => OutputFormat::Hex,
                    _ => unreachable!(),
                })
                .or_else(|| {
                    Path::new(output_file).extension().and_then(|ext| {
                        if ext.eq_ignore_ascii_case("bin") {
                            Some(OutputFormat::Bin)
                        } else if ext.eq_ignore_ascii_case("uf2") {
                            Some(OutputFormat::Uf2)
                        } else if ext.eq_ignore_ascii_case("hex") {
                            Some(OutputFormat::Hex)
                        } else {
                            None
                        }
                    })
                });
            let out_fmt = if let Some(out_fmt) = out_fmt {
                out_fmt
            } else {
                eprintln!("Cannot detect file output format from the file extension. Please explicitly use one of `--bin`, `--uf2` or `--hex`!");
                return;
            };

            let inputs: Vec<_> = input_files
                .map(|f| {
                    (
                        f,
                        serde_json::from_reader(File::open(f).expect("input file"))
                            .expect("parse json"),
                    )
                })
                .collect();
            let mut merged = Dict::merge_dicts(inputs);
            merged.insert("TKA*EUT".into(), chrono::Local::now().naive_local().format("%Y-%m-%d %H:%M:%S").to_string());
            let dict = match Dict::parse(merged) {
                Ok(d) => d,
                Err(e) => {
                    eprintln!("{}", e);
                    return;
                }
            };
            let mut output_file = File::create(output_file).expect("output file");
            let out_file: Box<dyn OutputFile> = match out_fmt {
                OutputFormat::Bin => Box::new(BinFile::new()),
                OutputFormat::Hex => Box::new(HexFile::new()),
                OutputFormat::Uf2 => Box::new(Uf2File::new()),
            };
            if let Err(e) = compile::to_writer(dict, out_file, &mut output_file) {
                eprintln!("{}", e);
                return;
            };
            println!("Size: {}", output_file.seek(SeekFrom::Current(0)).unwrap());
        }
        // ("decompile", Some(m)) => {
        //     let input_file = m.value_of("input").unwrap();
        //     let output_file = m.value_of("output").unwrap();

        //     let mut input = Vec::new();
        //     File::open(input_file)
        //         .expect("input file")
        //         .read_to_end(&mut input)
        //         .unwrap();
        //     let dict = match decompile::decompile(&input) {
        //         Ok(d) => d,
        //         Err(e) => {
        //             eprintln!("{:?}", e);
        //             return;
        //         }
        //     };
        //     let mut output_file = File::create(output_file).expect("output file");
        //     if let Err(e) = compile::to_writer(dict, &mut output_file) {
        //         eprintln!("{}", e);
        //         return;
        //     };
        //     println!("Size: {}", output_file.seek(SeekFrom::Current(0)).unwrap());
        // }
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
