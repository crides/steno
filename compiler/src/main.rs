#[macro_use]
extern crate serde_derive;
#[macro_use]
extern crate lazy_static;
#[macro_use]
extern crate bitfield;

mod bar;
mod compile;
mod dict;
mod flash;
mod freemap;
mod keycode;
mod rule;
mod stroke;

use std::cmp::min;
use std::fs::File;
use std::io::prelude::*;
use std::io::{Seek, SeekFrom};

use clap::{App, Arg, SubCommand};

use bar::*;
use compile::RawDict;
use dict::Dict;
use flash::{consts::*, Device, INMessage, OUTMessage};
use rule::{apply_rules, Dict as RuleDict, Rules};
use stroke::Stroke;

fn main() {
    let matches = App::new("compile-steno")
        .subcommand(
            SubCommand::with_name("flash-dump")
                .arg(Arg::with_name("addr").required(true))
                .arg(Arg::with_name("len").required(true)),
        )
        .subcommand(SubCommand::with_name("download").arg(Arg::with_name("file").required(true)))
        .subcommand(SubCommand::with_name("test").arg(Arg::with_name("stroke").required(true)))
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
            let ir = RawDict::from_dict(dict);
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
        ("flash-dump", Some(m)) => {
            let mut addr: u32 = m.value_of("addr").unwrap().parse().unwrap();
            let mut len: usize = m.value_of("len").unwrap().parse().unwrap();

            let manager = hid::init().unwrap();
            // FEED:6061 is the VID:PID in the current firmware
            // TODO: Make it adjustable
            for device in manager.find(Some(0xFEED), Some(0x6061)) {
                if device.usage_page() == 0xff60 && device.usage() == 0x61 {
                    let mut device = Device::new(device.open_by_path().unwrap());
                    while len > 0 {
                        let msg_len = std::cmp::min(len, PAYLOAD_SIZE);
                        let in_msg = device.send_message(OUTMessage::Read {
                            addr,
                            len: msg_len as u8,
                        });
                        if let INMessage::Read { data, .. } = in_msg {
                            for b in data {
                                print!("{:02x} ", b);
                            }
                            println!();
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
                if device.usage_page() == 0xff60 && device.usage() == 0x61 {
                    let mut device = Device::new(device.open_by_path().unwrap());
                    let pbar = progress_bar_bytes(file_len, "Erasing");
                    for i in (0..file_len).step_by(65536) {
                        let in_msg = device.send_message(OUTMessage::Erase(i as u32));
                        assert!(matches!(in_msg, INMessage::Ack));
                        pbar.inc(65536);
                    }
                    pbar.finish_with_message("Done erasing");

                    let mut addr = 0u32;
                    let pbar = progress_bar_bytes(file_len, "Downloading dictionary");
                    while file_len > 0 {
                        let mut data = vec![0; MASSWRITE_MAX_SIZE];
                        let msg_len = min(file_len, MASSWRITE_MAX_SIZE);
                        let read_len = file.read(&mut data).unwrap();
                        assert_eq!(read_len, msg_len);
                        let in_msg = device.send_message(OUTMessage::MassWrite { addr, data });
                        assert!(matches!(in_msg, INMessage::Ack));
                        file_len -= msg_len;
                        addr += msg_len as u32;
                        pbar.inc(msg_len as u64);
                    }
                    pbar.finish_with_message("Downloaded");
                    break;
                }
            }
        }
        (cmd, _) => panic!("{}", cmd),
    }
}
