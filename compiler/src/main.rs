#[macro_use]
extern crate serde_derive;
#[macro_use]
extern crate lazy_static;
#[macro_use]
extern crate bitfield;

mod compile;
mod dict;
mod hashmap;
mod rule;
mod stroke;

use std::fs::File;
use std::io::{Seek, SeekFrom};
use std::time::Duration;

use clap::{App, Arg, SubCommand};

use compile::IR;
use dict::Dict;
use rule::{apply_rules, Dict as RuleDict, Rules};
use stroke::Stroke;

// use stroke::Stroke;

// fn hash_strokes(input: &str) -> u32 {
//     let bytes = input
//         .split('/')
//         .flat_map(|stroke| stroke.parse::<Stroke>().unwrap().raw().to_le_bytes().to_vec().into_iter())
//         .collect::<Vec<_>>();
//     let mut hash = 0x811c9dc5u32;
//     for byte in bytes.into_iter() {
//         hash *= 0x01000193;
//         hash ^= byte as u32;
//     }
//     hash
// }

fn main() {
    let matches = App::new("compile-steno")
        .subcommand(SubCommand::with_name("test").arg(Arg::with_name("stroke").required(true)))
        .subcommand(SubCommand::with_name("test2").arg(Arg::with_name("input").required(true)))
        .subcommand(
            SubCommand::with_name("compile")
                .arg(Arg::with_name("input").required(true))
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
            let input_file = m.value_of("input").unwrap();
            let output_file = m.value_of("output").unwrap();

            let input_file = File::open(input_file).expect("input file");
            let input = serde_json::from_reader(input_file).expect("read json");
            let dict = Dict::parse_from_json(&input).unwrap();
            let ir = IR::from_dict(dict);
            let mut output_file = File::create(output_file).expect("output file");
            ir.write(&mut output_file).expect("write output");
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
        ("test2", Some(m)) => {
            let input = m.value_of("input").unwrap();
            let manager = hid::init().unwrap();
            for device in manager.find(Some(0xFEED), Some(0x6061)) {
                if device.usage_page() == 0xFF60 && device.usage() == 0x61 {
                    println!("serial: {:?}", device.serial_number());
                    println!("path: {:?}", device.path());
                    println!("manu_string: {:?}", device.manufacturer_string());
                    println!("prod_string: {:?}", device.product_string());
                    println!("release: {:?}", device.release_number());
                    println!("interface: {:?}", device.interface_number());
                    println!("usage_page: {:x}", device.usage_page());
                    println!("usage: {:x}", device.usage());
                    let mut handle = device.open_by_path().unwrap();
                    handle.data().write(input).unwrap();
                    let mut buf = vec![0; 32];
                    handle.data().read(&mut buf, Duration::from_secs(0)).unwrap();
                    dbg!(buf);
                    break;
                }
            }
        }
        (cmd, _) => panic!("{}", cmd),
    }

    // let app = App::new("compile-steno")
    //     .arg(Arg::with_name("test").required(true));
    // let matches = app.get_matches();
    // let test_str = matches.value_of("test").unwrap();
    // let (attr, s) = Dict::parse_entry(test_str);
    // println!("attr: {:?}, str: {}", attr, s);
}
