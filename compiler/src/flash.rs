use std::time::Duration;

use hid::Handle;

pub const PAGE_SIZE: u32 = 256;
pub const PACKET_SIZE: usize = 64;
pub const PAYLOAD_SIZE: usize = PACKET_SIZE - 8;
pub const MSG_SIZE: usize = PAYLOAD_SIZE + 8 - 2;

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

pub struct Device(Handle);

impl Device {
    pub fn new(d: Handle) -> Device {
        Device(d)
    }
    
    pub fn send_message(&mut self, msg: OUTMessage) -> INMessage {
        let mut buf = vec![0u8; PACKET_SIZE];
        // Message format:
        //                   | len | addr |
        // | Magic | Command |   Message  | Payload | Checksum |
        //     0        1        2 - 5       6 - 21   22 - 23
        buf[0] = 0xAA;
        // dbg!(Local::now().timestamp_millis());
        match msg.clone() {
            OUTMessage::Write { addr, data } => {
                buf[1] = 0x01;
                assert!(data.len() <= PAYLOAD_SIZE);
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
        // dbg!(Local::now().timestamp_millis());
        let crc = crc16(&buf[..=(MSG_SIZE - 1)]);
        // dbg!(Local::now().timestamp_millis());
        buf[MSG_SIZE..=(MSG_SIZE + 1)].copy_from_slice(&crc.to_le_bytes());
        let mut data_handle = self.0.data();
        data_handle.write(&buf).unwrap();
        // dbg!(Local::now().timestamp_millis());

        let mut buf = vec![0u8; PACKET_SIZE];
        data_handle.read(&mut buf, Duration::from_secs(0)).unwrap();
        // dbg!(Local::now().timestamp_millis());
        assert_eq!(buf[0], 0x55);
        let crc = crc16(&buf[..=(MSG_SIZE - 1)]);
        // dbg!(Local::now().timestamp_millis());
        let mut bytes = [0u8; 2];
        bytes.copy_from_slice(&buf[MSG_SIZE..=(MSG_SIZE + 1)]);
        assert_eq!(crc, u16::from_le_bytes(bytes));
        let ret = match buf[1] {
            0x01 => {
                INMessage::Ack
            }
            0x02 => {
                assert!(matches!(msg, OUTMessage::Read { .. }));
                let len = buf[2];
                assert!(len <= PAYLOAD_SIZE as u8);
                let mut bytes = [0u8; 4];
                bytes[0..3].copy_from_slice(&buf[3..=5]);
                let addr = u32::from_le_bytes(bytes);
                let mut data = Vec::new();
                data.extend(buf[6..=(MSG_SIZE - 1)].iter().take(len as usize));
                INMessage::Read { addr, data }
            }
            _ => panic!(),
        };
        // dbg!(Local::now().timestamp_millis());
        // println!("");
        ret
    }
}
