//! Handles everything related to converting Rust dictionary related values to bytes. Provides raw
//! counterparts for types in `dict`.
use std::io::{self, prelude::*};
use std::iter;

use crate::dict::{Attr, Dict, Entry, Input};
use crate::freemap::FreeMap;
use crate::stroke::hash_strokes;

/// Byte level counterpart for `Entry`, is just a wrapper around the actual bytes that would be written to the
/// keyboard storage.
pub struct RawEntry(Vec<u8>);

impl From<Entry> for RawEntry {
    fn from(e: Entry) -> RawEntry {
        let attr: RawAttr = e.attr.into();
        let bytes = iter::once(e.byte_len() as u8)
            .chain(iter::once(attr.0))
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
                Input::AddTranslation => vec![16],
                Input::EditTranslation => vec![17],
                Input::RemoveTranslation => vec![18],
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

pub struct RawDict {
    /// KV-pointer + key length
    buckets: Vec<u32>,
    kvpairs: Vec<Vec<u8>>,
    // TODO bit map
}

impl RawDict {
    pub fn from_dict(d: Dict) -> RawDict {
        // 1M entries of 3 bytes; key length cannot be 15
        let mut buckets = vec![0xFFFFFF; 2usize.pow(20)];
        let mut kvpairs = Vec::with_capacity(d.0.len());
        let mut map = FreeMap::new();
        let mut max_collision = 0;
        for (strokes, entry) in d.0.into_iter() {
            let hash = hash_strokes(&strokes);
            let mut index = hash as usize % 2usize.pow(20);
            let mut collisions = 0;
            while buckets[index] != 0xFFFFFF {
                index = index.wrapping_add(1);
                collisions += 1;
            }
            if collisions > max_collision {
                max_collision = collisions;
            }
            let pair_size = strokes.len() * 3 + 2 + entry.byte_len();
            let (block_no, block_size) = if pair_size <= 16 {
                (map.req_16(), 16)
            } else if pair_size <= 32 {
                (map.req_32(), 32)
            } else if pair_size <= 64 {
                (map.req_64(), 64)
            } else {
                panic!("Entry too big!");
            };
            assert!(strokes.len() < 15);
            buckets[index] = block_no << 4 | strokes.len() as u32;
            let mut block = Vec::with_capacity(block_size);
            for stroke in strokes {
                block.write_all(&stroke.raw().to_le_bytes()[0..3]).unwrap();
            }
            let raw_entry: RawEntry = entry.into();
            block.write_all(&raw_entry.as_bytes()).unwrap();
            for _ in pair_size..block_size {
                block.push(0xFF);
            }
            kvpairs.push(block);
        }
        dbg!(max_collision);
        RawDict { buckets, kvpairs }
    }

    /// Write the RawDict out as bytes
    pub fn write(&self, w: &mut dyn Write) -> io::Result<()> {
        for bucket in &self.buckets {
            w.write_all(&bucket.to_le_bytes()[0..3])?;
        }
        let mut written = 3 * (1usize << 20);
        for pair in &self.kvpairs {
            written += pair.len();
            w.write_all(&pair)?;
        }
        let rem = vec![0xFFu8; 16];
        while written < 16 * (1usize << 20) {
            w.write_all(&rem)?;
            written += 16;
        }
        Ok(())
    }
}
