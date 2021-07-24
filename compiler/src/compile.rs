//! Handles everything related to converting Rust dictionary related values to bytes. Provides raw
//! counterparts for types in `dict`.
use std::collections::BTreeMap;
use std::io::{self, prelude::*};
use std::iter;

use crate::dict::{Attr, Dict, Entry, Input};
use crate::freemap::FreeMap;
use crate::orthography;
use crate::stroke::{hash_strokes, Strokes};

/// Byte level counterpart for `Entry`, is just a wrapper around the actual bytes that would be written to the
/// keyboard storage.
pub struct RawEntry(Vec<u8>);

impl From<Entry> for RawEntry {
    fn from(e: Entry) -> RawEntry {
        let attr: RawAttr = e.attr.into();
        let bytes = iter::once(attr.0)
            .chain(e.inputs.iter().flat_map(|i| match i {
                Input::Keycodes(keycodes) => {
                    assert!(keycodes.len() <= 127);
                    if keycodes.is_empty() {
                        vec![]
                    } else {
                        let mut bytes = vec![0, keycodes.len() as u8];
                        bytes.extend(keycodes);
                        bytes
                    }
                }
                Input::String(s) => s.as_bytes().to_vec(),
                Input::Carry(s) => {
                    let mut bytes = s.as_bytes().to_vec();
                    bytes.insert(0, 4);
                    bytes.insert(1, s.len() as u8);
                    bytes
                }
                Input::Lower => vec![1],
                Input::Upper => vec![2],
                Input::Capital => vec![3],
                Input::ResetFormat => vec![5],
                Input::LowerLast => vec![8],
                Input::UpperLast => vec![9],
                Input::CapitalLast => vec![10],
                Input::Repeat => vec![11],
                Input::ToggleStar => vec![12],
                Input::RetroSpace => vec![13],
                Input::RetroNoSpace => vec![14],
                Input::DictEdit => vec![16],
            }))
            .collect();
        RawEntry(bytes)
    }
}

impl RawEntry {
    fn as_bytes(&self) -> &[u8] {
        &self.0
    }
}

bitfield! {
    /// Bit level counterpart for `Attr`. Using a bitfield for compact representation
    #[derive(PartialEq, Eq, Hash, Clone, Copy)]
    pub struct RawAttr(u8);
    impl Debug;
    space_prev, set_space_prev: 0, 0;
    space_after, set_space_after: 1, 1;
    glue, set_glue: 2, 2;
    present, set_present: 3, 3;
}

impl From<Attr> for RawAttr {
    fn from(a: Attr) -> RawAttr {
        let mut ra = RawAttr(0);
        ra.set_glue(a.glue.into());
        ra.set_space_prev(a.space_prev.into());
        ra.set_space_after(a.space_after.into());
        ra.set_present(a.present.into());
        ra
    }
}

#[allow(dead_code)]
pub const KVPAIR_START: usize = 0x400000;
#[allow(dead_code)]
pub const FREEMAP_START: usize = 0xF00000;
#[allow(dead_code)]
pub const SCRATCH_START: usize = 0xF22000;
#[allow(dead_code)]
pub const ORTHOGRAPHY_START: usize = 0xF30000;

#[derive(Debug)]
pub enum CompileError {
    TooManyStrokes(Strokes),
    LargeEntry(Strokes),
    NoStorage { cur_len: usize, total: usize },
    Io(std::io::Error),
}

impl std::fmt::Display for CompileError {
    fn fmt(&self, f: &mut std::fmt::Formatter) -> std::fmt::Result {
        use CompileError::*;
        match self {
            TooManyStrokes(s) => write!(f, "The entry '{}' has too many (>14) strokes", s),
            LargeEntry(s) => write!(f, "The entry mapped from '{}' is too large", s),
            NoStorage { cur_len, total } => write!(
                f,
                "Storage space runs out for entries. Entries stored {}/{}",
                cur_len, total
            ),
            Io(e) => write!(f, "IO error: {}", e),
        }
    }
}

pub fn to_writer(
    d: Dict,
    mut out: Box<dyn OutputFile>,
    w: &mut dyn Write,
) -> Result<(), CompileError> {
    // Buffering buckets so less book keeping
    // 1M entries of 4 bytes; key length cannot be 15
    let mut buckets = vec![0xFFFFFFFF; 2usize.pow(20)];
    let mut map = FreeMap::new((FREEMAP_START - KVPAIR_START) as u32);
    let mut collisions = BTreeMap::new();
    let total_len = d.0.len();
    for (i, (strokes, entry)) in d.0.into_iter().enumerate() {
        if strokes.len() > 14 {
            return Err(CompileError::TooManyStrokes(strokes));
        }
        let hash = hash_strokes(&strokes);
        let mut index = hash as usize % 2usize.pow(20);
        let mut collision = 0;
        while buckets[index] != 0xFFFFFFFF {
            index = (index + 1) % 2usize.pow(20);
            collision += 1;
        }
        collisions
            .entry(collision)
            .and_modify(|c| *c += 1)
            .or_insert(1);
        let entry_len = entry.byte_len();
        let pair_size = strokes.len() * 3 + 1 + entry_len;
        let block_no = if pair_size <= 16 {
            map.req(0)
        } else if pair_size <= 32 {
            map.req(1)
        } else if pair_size <= 64 {
            map.req(2)
        } else if pair_size <= 128 {
            map.req(3)
        } else if pair_size <= 256 {
            map.req(4)
        } else {
            return Err(CompileError::LargeEntry(strokes));
        };
        assert!(strokes.len() < 15 && strokes.len() > 0);
        let block_offset = block_no.ok_or(CompileError::NoStorage {
            cur_len: i,
            total: total_len,
        })? << 4;
        buckets[index] = (entry_len as u32) << 24 | block_offset | strokes.len() as u32;
        out.seek(KVPAIR_START + block_offset as usize);
        for stroke in strokes.0 {
            out.write_all(&stroke.raw().to_le_bytes()[0..3]);
        }
        let raw_entry: RawEntry = entry.into();
        out.write_all(&raw_entry.as_bytes());
    }

    dbg!(collisions);
    out.seek(0);
    for bucket in buckets {
        out.write_all(&bucket.to_le_bytes());
    }
    out.seek(FREEMAP_START);
    for word in map.map {
        out.write_all(&word.to_le_bytes());
    }
    out.seek(ORTHOGRAPHY_START);
    out.write_all(&orthography::generate());
    out.write_to(w).map_err(CompileError::Io)
}

pub trait OutputFile {
    fn seek(&mut self, addr: usize);
    fn write(&mut self, byte: u8);
    fn write_all(&mut self, bytes: &[u8]) {
        for byte in bytes {
            self.write(*byte);
        }
    }
    fn write_to(self: Box<Self>, w: &mut dyn Write) -> io::Result<()>;
}

pub struct BinFile {
    cursor: usize,
    binary: Vec<u8>,
}

impl BinFile {
    pub fn new() -> Self {
        Self {
            cursor: 0,
            binary: vec![0xFF; 16 * 2usize.pow(20)],
        }
    }
}

impl OutputFile for BinFile {
    fn seek(&mut self, addr: usize) {
        self.cursor = addr;
    }

    fn write(&mut self, byte: u8) {
        self.binary[self.cursor] = byte;
        self.cursor += 1;
    }

    fn write_to(self: Box<Self>, w: &mut dyn Write) -> io::Result<()> {
        w.write_all(&self.binary)
    }
}

/// A lazily filled file containing chunks of data.
struct BlockFile<const B: usize> {
    map: BTreeMap<usize, [u8; B]>,
    cur_block: usize,
    cur_block_ind: usize,
}

impl<const B: usize> BlockFile<B> {
    fn new() -> Self {
        let mut file = Self {
            map: BTreeMap::new(),
            cur_block: 0,
            cur_block_ind: 0,
        };
        file.seek(0);
        file
    }

    fn seek(&mut self, addr: usize) {
        self.cur_block = addr / B;
        self.cur_block_ind = addr % B;
        self.map
            .entry(self.cur_block)
            .or_insert_with(|| [0xFFu8; B]);
    }

    fn write(&mut self, byte: u8) {
        if self.cur_block_ind == B {
            self.cur_block += 1;
            self.cur_block_ind = 0;
            self.map.insert(self.cur_block, [0xFFu8; B]);
        }
        self.map.get_mut(&self.cur_block).unwrap()[self.cur_block_ind] = byte;
        self.cur_block_ind += 1;
    }
}

pub struct HexFile(BlockFile<16>);

impl HexFile {
    pub fn new() -> Self {
        Self(BlockFile::new())
    }
}

impl OutputFile for HexFile {
    fn seek(&mut self, addr: usize) {
        self.0.seek(addr)
    }

    fn write(&mut self, byte: u8) {
        self.0.write(byte)
    }

    fn write_to(self: Box<Self>, w: &mut dyn Write) -> io::Result<()> {
        let filtered: Vec<_> = self
            .0
            .map
            .into_iter()
            .filter(|(_addr, data)| !data.iter().all(|b| *b == 0xFFu8))
            .collect();
        let mut last_upper_addr = 0xff;
        for (block_addr, data) in filtered.iter() {
            let byte_addr = block_addr * 16;
            let lower_addr = byte_addr % 0x10000;
            let upper_addr = byte_addr / 0x10000;
            if upper_addr != last_upper_addr {
                let checksum = (0x100 - (2 + 4 + 0x12 + upper_addr) % 0x100) as u8;
                // NOTE we use 0x12xxxxxx cuz this is what the XIP starts at in nrf52
                writeln!(w, ":0200000412{:02X}{:02X}", upper_addr, checksum)?;
                last_upper_addr = upper_addr;
            }
            let checksum = (0x100
                - (0x10
                    + lower_addr % 0x100
                    + lower_addr / 0x100
                    + data.iter().fold(0, |a, b| a + *b as usize))
                    % 0x100) as u8;
            writeln!(w, ":10{:04X}00{:02X}{:02X}{:02X}{:02X}{:02X}{:02X}{:02X}{:02X}{:02X}{:02X}{:02X}{:02X}{:02X}{:02X}{:02X}{:02X}{:02X}", lower_addr,
                data[0], data[1],  data[2],  data[3],  data[4],  data[5],  data[6],  data[7],
                data[8], data[9], data[10], data[11], data[12], data[13], data[14], data[15],
                checksum,
            )?;
        }
        writeln!(w, ":00000001FF")
    }
}

#[allow(dead_code)]
struct Uf2 {
    magic0: u32,
    magic1: u32,
    flags: u32,
    target_addr: u32,
    payload_size: u32,
    block_no: u32,
    block_num: u32,
    family_id: u32,
    data: Vec<u8>,
    magic_end: u32,
}

pub struct Uf2File(BlockFile<{ Uf2File::DATA_SIZE }>);

impl Uf2File {
    const MAGIC0: u32 = 0x0A324655;
    const MAGIC1: u32 = 0x9E5D5157;
    const MAGIC_END: u32 = 0x0AB16F30;
    // Family ID present
    const FLAGS: u32 = 0x00002000;
    const FAMILY_ID: u32 = 0x00302cc0; // STOEUPB
    const UF2_DATA_SIZE: usize = 476;
    const DATA_SIZE: usize = 256;

    pub fn new() -> Self {
        Self(BlockFile::new())
    }
}

impl OutputFile for Uf2File {
    fn seek(&mut self, addr: usize) {
        self.0.seek(addr)
    }

    fn write(&mut self, byte: u8) {
        self.0.write(byte)
    }

    fn write_to(self: Box<Self>, w: &mut dyn Write) -> io::Result<()> {
        let filtered: Vec<_> = self
            .0
            .map
            .into_iter()
            .filter(|(_addr, data)| !data.iter().all(|b| *b == 0xFFu8))
            .collect();
        let num_blocks = filtered.len() as u32;
        let data_padding_post = [0u8; Uf2File::UF2_DATA_SIZE - Uf2File::DATA_SIZE];
        for (block_no, (block_addr, data)) in filtered.iter().enumerate() {
            w.write_all(&Uf2File::MAGIC0.to_le_bytes())?;
            w.write_all(&Uf2File::MAGIC1.to_le_bytes())?;
            w.write_all(&Uf2File::FLAGS.to_le_bytes())?;
            w.write_all(&((block_addr * Uf2File::DATA_SIZE) as u32).to_le_bytes())?;
            w.write_all(&(Uf2File::DATA_SIZE as u32).to_le_bytes())?;
            w.write_all(&(block_no as u32).to_le_bytes())?;
            w.write_all(&num_blocks.to_le_bytes())?;
            w.write_all(&Uf2File::FAMILY_ID.to_le_bytes())?;
            assert_eq!(data.len(), Uf2File::DATA_SIZE);
            assert_eq!(data.len() + data_padding_post.len(), Uf2File::UF2_DATA_SIZE);
            w.write_all(data)?;
            w.write_all(&data_padding_post)?;
            w.write_all(&Uf2File::MAGIC_END.to_le_bytes())?;
        }
        Ok(())
    }
}

impl From<RawAttr> for Attr {
    fn from(r: RawAttr) -> Attr {
        Attr {
            space_after: r.space_after() != 0,
            space_prev: r.space_prev() != 0,
            present: r.present() != 0,
            glue: r.glue() != 0,
        }
    }
}
