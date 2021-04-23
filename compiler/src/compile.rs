//! Handles everything related to converting Rust dictionary related values to bytes. Provides raw
//! counterparts for types in `dict`.
use std::collections::BTreeMap;
use std::io::{self, prelude::*};
use std::iter;

use crate::dict::{Attr, Dict, Entry, Input};
use crate::freemap::FreeMap;
use crate::stroke::hash_strokes;
use crate::orthography;

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
                Input::Keep(s) => {
                    let mut bytes = s.as_bytes().to_vec();
                    bytes.insert(0, 4);
                    bytes.insert(1, s.len() as u8);
                    bytes
                }
                Input::Lower => vec![1],
                Input::Upper => vec![2],
                Input::Capitalized => vec![3],
                Input::ResetFormat => vec![5],
                Input::LowerLast => vec![8],
                Input::UpperLast => vec![9],
                Input::CapitalizedLast => vec![10],
                Input::Repeat => vec![11],
                Input::ToggleStar => vec![12],
                Input::AddSpace => vec![13],
                Input::RemoveSpace => vec![14],
                Input::EditDictionary => vec![16],
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

pub fn to_writer(d: Dict, w: &mut dyn Write) -> io::Result<()> {
    let mut file = Uf2File::new();
    // Buffering buckets so less book keeping
    // 1M entries of 4 bytes; key length cannot be 15
    let mut buckets = vec![0xFFFFFFFF; 2usize.pow(20)];
    let mut map = FreeMap::new((FREEMAP_START - KVPAIR_START) as u32);
    let mut collisions = BTreeMap::new();
    for (strokes, entry) in d.0.into_iter() {
        let hash = hash_strokes(&strokes);
        let mut index = hash as usize % 2usize.pow(20);
        let mut collision = 0;
        while buckets[index] != 0xFFFFFFFF {
            index = (index + 1) % 2usize.pow(20);
            collision += 1;
        }
        collisions.entry(collision).and_modify(|c| *c += 1).or_insert(1);
        let entry_len = entry.byte_len();
        assert!(entry_len < 256);
        let pair_size = strokes.len() * 3 + 1 + entry_len;
        let block_no = if pair_size <= 16 {
            map.req(0)
        } else if pair_size <= 32 {
            map.req(1)
        } else if pair_size <= 64 {
            map.req(2)
        } else if pair_size <= 128 {
            map.req(3)
        } else {
            panic!("Entry too big!");
        };
        assert!(strokes.len() < 15 && strokes.len() > 0);
        let block_offset = block_no.unwrap() << 4;
        buckets[index] = (entry_len as u32) << 24 | block_offset | strokes.len() as u32;
        file.seek(KVPAIR_START + block_offset as usize);
        for stroke in strokes {
            file.write_all(&stroke.raw().to_le_bytes()[0..3]);
        }
        let raw_entry: RawEntry = entry.into();
        file.write_all(&raw_entry.as_bytes());
    }

    dbg!(collisions);
    file.seek(0);
    for bucket in buckets {
        file.write_all(&bucket.to_le_bytes());
    }
    file.seek(FREEMAP_START);
    for word in map.map {
        file.write_all(&word.to_le_bytes());
    }
    file.seek(ORTHOGRAPHY_START);
    file.write_all(&orthography::generate());
    file.write_to(w)
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

/// A lazily filled file containing Uf2 chunks. 
struct Uf2File {
    map: BTreeMap<usize, Vec<u8>>,
    cur_block: usize,
    cur_block_ind: usize,
}

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
        let mut file = Self {
            map: BTreeMap::new(),
            cur_block: 0,
            cur_block_ind: 0,
        };
        file.seek(0);
        file
    }

    fn seek(&mut self, addr: usize) {
        self.cur_block = addr / Uf2File::DATA_SIZE;
        self.cur_block_ind = addr % Uf2File::DATA_SIZE;
        if !self.map.contains_key(&self.cur_block) {
            self.map.insert(self.cur_block, vec![0xFFu8; Uf2File::DATA_SIZE]);
        }
    }

    fn write(&mut self, byte: u8) {
        if self.cur_block_ind == Uf2File::DATA_SIZE {
            self.cur_block += 1;
            self.cur_block_ind = 0;
            self.map.insert(self.cur_block, vec![0xFFu8; Uf2File::DATA_SIZE]);
        }
        self.map.get_mut(&self.cur_block).unwrap()[self.cur_block_ind] = byte;
        self.cur_block_ind += 1;
    }

    fn write_all(&mut self, bytes: &[u8]) {
        for byte in bytes {
            self.write(*byte);
        }
    }

    pub fn write_to(self, w: &mut dyn Write) -> io::Result<()> {
        let filtered: Vec<_> = self
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
            // NOTE: Custom UF2 format: Moving the data back by 32 bytes so that it's packet (64 byte)
            // aligned, and that it'll be easier to process
            w.write_all(&data)?;
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
