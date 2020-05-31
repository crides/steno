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
mod flash;
mod bar;

use std::fs::File;
use std::io::{Seek, SeekFrom};
use std::io::prelude::*;

use clap::{App, Arg, SubCommand};
use chrono::{Local, DateTime};

use compile::IR;
use dict::Dict;
use rule::{apply_rules, Dict as RuleDict, Rules};
use stroke::Stroke;
use flash::{Device, OUTMessage, INMessage};
use bar::progress_bar;

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
        .subcommand(
            SubCommand::with_name("flash-write")
                .arg(Arg::with_name("addr").required(true))
                .arg(Arg::with_name("data").required(true)),
        )
        .subcommand(
            SubCommand::with_name("flash-read")
                .arg(Arg::with_name("addr").required(true))
                .arg(Arg::with_name("len").required(true)),
        )
        .subcommand(
            SubCommand::with_name("flash-dump")
                .arg(Arg::with_name("addr").required(true))
                .arg(Arg::with_name("len").required(true)),
        )
        .subcommand(
            SubCommand::with_name("download")
                .arg(Arg::with_name("file").required(true)),
        )
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
        ("flash-write", Some(m)) => {
            let addr: u32 = m.value_of("addr").unwrap().parse().unwrap();
            let data = m.value_of("data").unwrap().into();
            let manager = hid::init().unwrap();
            for device in manager.find(Some(0xFEED), Some(0x6061)) {
                if device.usage_page() == 0xFF60 && device.usage() == 0x61 {
                    let device = Device::new(device);
                    println!("Reply: {:?}", device.send_message(OUTMessage::Write { addr, data }));
                    break;
                }
            }
        }
        ("flash-read", Some(m)) => {
            let addr: u32 = m.value_of("addr").unwrap().parse().unwrap();
            let len: u8 = m.value_of("len").unwrap().parse().unwrap();

            let manager = hid::init().unwrap();
            for device in manager.find(Some(0xFEED), Some(0x6061)) {
                if device.usage_page() == 0xFF60 && device.usage() == 0x61 {
                    let device = Device::new(device);
                    println!("Reply: {:?}", device.send_message(OUTMessage::Read { addr, len }));
                    break;
                }
            }
        }
        ("flash-dump", Some(m)) => {
            let mut addr: u32 = m.value_of("addr").unwrap().parse().unwrap();
            let mut len: usize = m.value_of("len").unwrap().parse().unwrap();

            let manager = hid::init().unwrap();
            for device in manager.find(Some(0xFEED), Some(0x6061)) {
                if device.usage_page() == 0xFF60 && device.usage() == 0x61 {
                    let device = Device::new(device);
                    while len > 0 {
                        let msg_len = std::cmp::min(len, 16);
                        let in_msg = device.send_message(OUTMessage::Read { addr, len: msg_len as u8 });
                        if let INMessage::Read { data, .. } = in_msg {
                            for b in data {
                                print!("{:02x} ", b);
                            }
                        }
                        len -= msg_len;
                        addr += msg_len as u32;
                    }
                    break;
                }
            }
        }
        ("download", Some(m)) => {
            let file_name = m.value_of("file").unwrap();
            let mut file = File::open(file_name).unwrap();
            let mut file_len = file.seek(SeekFrom::End(0)).unwrap() as usize;
            dbg!(file_len);
            assert!(file_len < 0x1_0000_0000_0000);
            file.seek(SeekFrom::Start(0)).unwrap();
            let manager = hid::init().unwrap();
            for device in manager.find(Some(0xFEED), Some(0x6061)) {
                if device.usage_page() == 0xFF60 && device.usage() == 0x61 {
                    let device = Device::new(device);
                    let bar = progress_bar(file_len, "Erasing");
                    for i in (0..file_len).step_by(65536) {
                        device.send_message(OUTMessage::Erase(i as u32));
                        bar.inc(65536);
                    }
                    bar.finish_with_message("Done erasing");

                    let mut addr = 0;
                    let mut buf = [0; 16];
                    let bar = progress_bar(file_len, "Downloading dictionary");
                    while file_len > 0 {
                        let msg_len = std::cmp::min(file_len, 16);
                        assert_eq!(file.read(&mut buf).unwrap(), msg_len);
                        bar.inc(msg_len as u64);
                        let in_msg = device.send_message(OUTMessage::Write { addr, data: buf.to_vec() });
                        assert!(matches!(in_msg, INMessage::Ack));
                        file_len -= msg_len;
                        addr += msg_len as u32;
                        println!("");
                    }
                    bar.finish_with_message("Downloaded");
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
