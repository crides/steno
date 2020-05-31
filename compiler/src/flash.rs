use std::time::Duration;

use chrono::{Local, DateTime};
use hid::Device as HIDDevice;

pub fn crc16(d: &[u8]) -> u16 {
    let mut crc = 0xffffu16;
    for b in d {
        crc ^= *b as u16;
        for _ in 0..8 {
            if crc & 1 != 0 {
                crc = (crc >> 1) ^ 0xA001;
            } else {
                crc = crc >> 1;
            }
        }
    }
    crc
}

#[derive(Clone, Debug)]
pub enum OUTMessage {
    Write { addr: u32, data: Vec<u8> },
    Read { addr: u32, len: u8 },
    Erase(u32),
}

#[derive(Clone, Debug)]
pub enum INMessage {
    Ack,
    Read { addr: u32, data: Vec<u8> },
}

pub struct Device<'a>(HIDDevice<'a>);

impl<'a> Device<'a> {
    pub fn new(d: HIDDevice<'a>) -> Device<'a> {
        Device(d)
    }
    
    pub fn send_message(&self, msg: OUTMessage) -> INMessage {
        let mut buf = [0u8; 24];
        // Message format:
        //                   | len | addr |
        // | Magic | Command |   Message  | Payload | Checksum |
        //     0        1        2 - 5       6 - 21   22 - 23
        buf[0] = 0xAA;
        dbg!(Local::now().timestamp_millis());
        match msg.clone() {
            OUTMessage::Write { addr, data } => {
                buf[1] = 0x01;
                assert!(data.len() <= 16);
                buf[2] = data.len() as u8;
                assert_eq!(addr & 0xFF000000, 0);
                buf[3..=5].copy_from_slice(&addr.to_le_bytes()[0..3]);
                for (i, byte) in data.iter().enumerate() {
                    buf[6 + i] = *byte;
                }
            }
            OUTMessage::Read { addr, len } => {
                buf[1] = 0x02;
                buf[2] = len;
                assert_eq!(addr & 0xFF000000, 0);
                buf[3..=5].copy_from_slice(&addr.to_le_bytes()[0..3]);
            }
            OUTMessage::Erase(addr) => {
                buf[1] = 0x03;
                assert_eq!(addr & 0xFF000000, 0);
                buf[3..=5].copy_from_slice(&addr.to_le_bytes()[0..3]);
            }
        }
        dbg!(Local::now().timestamp_millis());
        let crc = crc16(&buf[..=21]);
        dbg!(Local::now().timestamp_millis());
        buf[22..=23].copy_from_slice(&crc.to_le_bytes());
        let mut handle = self.0.open_by_path().unwrap();
        let mut data_handle = handle.data();
        data_handle.write(&buf).unwrap();
        dbg!(Local::now().timestamp_millis());

        let mut buf = [0u8; 24];
        data_handle.read(&mut buf, Duration::from_secs(0)).unwrap();
        dbg!(Local::now().timestamp_millis());
        assert_eq!(buf[0], 0x55);
        let crc = crc16(&buf[..=21]);
        dbg!(Local::now().timestamp_millis());
        let mut bytes = [0u8; 2];
        bytes.copy_from_slice(&buf[22..=23]);
        assert_eq!(crc, u16::from_le_bytes(bytes));
        let ret = match buf[1] {
            0x01 => {
                INMessage::Ack
            }
            0x02 => {
                assert!(matches!(msg, OUTMessage::Read { .. }));
                let len = buf[2];
                assert!(len <= 16);
                let mut bytes = [0u8; 4];
                bytes[0..3].copy_from_slice(&buf[3..=5]);
                let addr = u32::from_le_bytes(bytes);
                let mut data = Vec::new();
                data.extend(buf[6..=21].iter().take(len as usize));
                INMessage::Read { addr, data }
            }
            _ => panic!(),
        };
        dbg!(Local::now().timestamp_millis());
        println!("");
        ret
    }
}
