use std::time::Duration;

use hid::Handle;

pub mod consts {
    #![allow(dead_code)]
    pub const PAGE_SIZE: u32 = 256;
    pub const PACKET_SIZE: usize = 64;
    pub const PAYLOAD_SIZE: usize = PACKET_SIZE - 8;
    pub const MSG_SIZE: usize = PAYLOAD_SIZE + 8 - 2;
    pub const MASSWRITE_PACKET_NUM: usize = 28;
    pub const MASSWRITE_MAX_SIZE: usize = MASSWRITE_PACKET_NUM * PACKET_SIZE;
}

use consts::*;

pub fn crc8(d: &[u8]) -> u8 {
    let mut crc = 0;
    for b in d {
        crc ^= *b;
        for _ in 0..8 {
            crc >>= 1;
            if crc & 1 != 0 {
                crc ^= 0x8C;
            }
        }
    }
    crc
}

#[derive(Clone, Copy, Debug)]
pub struct MassWritePacketInfo {
    crc: u8,
    len: u8,
}

impl MassWritePacketInfo {
    pub fn new(crc: u8, len: u8) -> Self {
        Self { crc, len }
    }

    pub fn to_bytes(&self) -> Vec<u8> {
        let mut v = Vec::new();
        v.extend_from_slice(&self.crc.to_le_bytes());
        v.push(self.len);
        v
    }
}

#[derive(Clone, Debug)]
#[allow(dead_code)]
pub enum OUTMessage {
    Write {
        addr: u32,
        data: Vec<u8>,
    },
    Read {
        addr: u32,
        len: u8,
    },
    Erase(u32),
    /// Pseudo packet
    MassWrite {
        addr: u32,
        data: Vec<u8>,
    },
    MassWriteHeader {
        addr: u32,
        packet_infos: Vec<MassWritePacketInfo>,
    },
    Raw(Vec<u8>),
}

#[derive(Clone, Debug)]
#[must_use]
pub enum INMessage {
    Ack,
    Nack,
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
        let is_raw = matches!(msg, OUTMessage::Raw(_));
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
            OUTMessage::MassWrite { addr, mut data } => {
                let mut data_packets = Vec::new();
                let mut packet_infos = Vec::new();
                while data.len() >= PACKET_SIZE {
                    let mut front = data.split_off(PACKET_SIZE);
                    std::mem::swap(&mut front, &mut data);
                    let crc = crc8(&front);
                    let len = front.len() as u8;
                    data_packets.push(front);
                    packet_infos.push(MassWritePacketInfo::new(crc, len));
                }
                if !data.is_empty() {
                    let crc = crc8(&data);
                    let len = data.len() as u8;
                    data_packets.push(data);
                    packet_infos.push(MassWritePacketInfo::new(crc, len));
                }
                let mut in_msg =
                    self.send_message(OUTMessage::MassWriteHeader { addr, packet_infos });
                assert!(matches!(in_msg, INMessage::Ack));
                for packet in data_packets {
                    in_msg = self.send_message(OUTMessage::Raw(packet.clone()));
                    match in_msg {
                        INMessage::Nack => {
                            println!("packet: {:?}", packet);
                            in_msg = self.send_message(OUTMessage::Raw(packet));
                            assert!(matches!(in_msg, INMessage::Ack));
                        }
                        INMessage::Ack => (),
                        _ => panic!("???"),
                    }
                }
                return in_msg;
            }
            OUTMessage::MassWriteHeader { addr, packet_infos } => {
                buf[1] = 0x04;
                assert_eq!(addr & 0xFF000000, 0);
                buf[2] = packet_infos.len() as u8;
                buf[3..=5].copy_from_slice(&addr.to_le_bytes()[0..3]);
                for (i, info) in packet_infos.into_iter().enumerate() {
                    let info_size = std::mem::size_of::<MassWritePacketInfo>();
                    buf[(6 + info_size * i)..(6 + info_size * (i + 1))]
                        .copy_from_slice(&info.to_bytes());
                }
            }
            OUTMessage::Raw(data) => {
                buf.copy_from_slice(&data);
            }
        }
        if !is_raw {
            let crc = crc8(&buf[..MSG_SIZE]);
            buf[PACKET_SIZE - 1] = crc;
        }
        let mut data_handle = self.0.data();
        data_handle.write_to(0, &buf).unwrap();

        let mut buf = vec![0u8; PACKET_SIZE];
        data_handle.read(&mut buf, Duration::from_secs(0)).unwrap();
        assert_eq!(buf[0], 0x55);
        let crc = crc8(&buf[..MSG_SIZE]);
        assert_eq!(crc, buf[PACKET_SIZE - 1]);
        match buf[1] {
            0x01 => INMessage::Ack,
            0x02 => {
                assert!(matches!(msg, OUTMessage::Read { .. }));
                let len = buf[2];
                assert!(len <= PAYLOAD_SIZE as u8);
                let mut bytes = [0u8; 4];
                bytes[0..3].copy_from_slice(&buf[3..=5]);
                let addr = u32::from_le_bytes(bytes);
                let mut data = Vec::new();
                data.extend(buf[6..MSG_SIZE].iter().take(len as usize));
                INMessage::Read { addr, data }
            }
            0xFF => {
                dbg!(buf[2]);
                INMessage::Nack
            }
            _ => panic!(),
        }
    }
}
