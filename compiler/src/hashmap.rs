use std::io::{self, prelude::*};
use std::sync::Mutex;

use primes::{PrimeSet, Sieve};

lazy_static! {
    pub static ref PRIMES: Mutex<Sieve> = Mutex::new(Sieve::new());
}

pub const MIN_MAP_SIZE: usize = 8;

/// A fixed size linear-probing HashMap, used only in IR
pub struct LPHashMap {
    map: Vec<(u32, u32)>,
    original_size: usize,
    max_collisions: usize,
}

impl LPHashMap {
    pub fn new(size: usize) -> Self {
        if size < MIN_MAP_SIZE {
            Self {
                map: Vec::new(),
                original_size: size,
                max_collisions: 0,
            }
        } else {
            let (_, map_size) = PRIMES.lock().unwrap().find(size as u64 * 6);
            let map_size = map_size as usize;
            Self {
                map: vec![(0, 0); map_size],
                original_size: size,
                max_collisions: 0,
            }
        }
    }

    pub fn total_size(&self) -> usize {
        if self.original_size < MIN_MAP_SIZE {
            self.original_size
        } else {
            self.map.len()
        }
    }

    pub fn add_child(&mut self, input: u32, addr: u32) {
        if self.original_size < MIN_MAP_SIZE {
            self.map.push((input, addr));
            return;
        }
        let mut ind = LPHashMap::hash(input) as usize % self.map.len();
        let mut collisions = 0;
        while self.map[ind].0 != 0 {
            ind += 1;
            if ind == self.map.len() {
                ind = 0;
            }
            collisions += 1;
        }
        if collisions > self.max_collisions {
            self.max_collisions = collisions;
        }
        self.map[ind] = (input, addr);
    }

    pub fn stats(&self) -> (usize, usize, usize) {
        (self.max_collisions, self.map.len(), self.original_size)
    }

    /// From FNV-1a
    pub fn hash(input: u32) -> u32 {
        let bytes = input.to_le_bytes();
        let mut hash = 0x811c9dc5u32;
        for byte in &bytes {
            hash *= 0x01000193;
            hash ^= *byte as u32;
        }
        hash
    }

    pub fn write_all_to(&self, w: &mut dyn Write) -> io::Result<()> {
        let mut buf = Vec::new();
        for (input, addr) in self.map.iter() {
            buf.clear();
            buf.extend_from_slice(&input.to_le_bytes()[0..3]);
            buf.extend_from_slice(&addr.to_le_bytes()[0..3]);
            w.write_all(&buf)?;
        }
        Ok(())
    }
}
