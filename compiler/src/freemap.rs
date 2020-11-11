/// A multilevel bit map that keeps track of which blocks are used and which aren't
/// Each level uses a 32bit bitvec. 1 means free or one of children is free. 0 means all of children or self
/// is full
pub struct FreeMap(Vec<u32>);

impl FreeMap {
    // NOTE: Word *not byte* indexes
    const LVL_0: u32 = 0;
    const LVL_1: u32 = (1 << 20) / 32 + FreeMap::LVL_0; // 1M entries
    const LVL_2: u32 = (1 << 20) / 32 / 32 + FreeMap::LVL_1; // 32768 entries
    const LVL_3: u32 = (1 << 20) / 32 / 32 / 32 + FreeMap::LVL_2; // 1024 entries

    fn find_free(word: u32, start: u8, block: u8) -> Option<u32> {
        let size: u8 = 2u8.pow(block as u32);
        let mut mask: u32 = 2u32.pow(size as u32) - 1;
        for i in (0..32).step_by(size as usize) {
            if i < start as u32 {
                mask <<= size;
                continue;
            }
            if (word & mask) == mask {
                return Some(i);
            }
            mask <<= size;
        }
        return None;
    }

    pub fn new() -> FreeMap {
        FreeMap(vec![0xFFFF_FFFF; 1 + 32 + 32usize.pow(2) + 32usize.pow(3)])
    }

    /// Request a 16 byte block, returning block number
    pub fn req(&mut self, block: u8) -> Option<u32> {
        if let Some((ind, _full)) = self._req(3, 0, block) {
            return Some(ind);
        }
        None
    }

    fn get_start(lvl: u8) -> u32 {
        match lvl {
            0 => FreeMap::LVL_0,
            1 => FreeMap::LVL_1,
            2 => FreeMap::LVL_2,
            3 => FreeMap::LVL_3,
            _ => panic!(),
        }
    }

    fn _req(&mut self, lvl: u8, word: u32, block: u8) -> Option<(u32, bool)> {
        let this_lvl_block = if lvl == 0 { block } else { 0 };
        let start_ind = FreeMap::get_start(lvl);
        let alloc_word = self.0[(start_ind + word) as usize];
        let size: u8 = 2u8.pow(this_lvl_block as u32);
        let mask: u32 = 2u32.pow(size as u32) - 1;
        for start in 0..32 {
            let res = FreeMap::find_free(alloc_word, start, this_lvl_block);
            dbg!((lvl, res));
            if let Some(mut ind) = res {
                if lvl != 0 {
                    println!("_req({}, {}, {})", lvl - 1, ind, block);
                    if let Some((i, full)) = self._req(lvl - 1, ind, block) {
                        ind = i;
                        if full {
                            self.0[(start_ind + word) as usize] &= !(mask << ind);
                        }
                    } else {
                        continue;
                    }
                } else {
                    self.0[(start_ind + word) as usize] &= !(mask << ind);
                }
                println!("mask: 0x{:08x}, ind: {}", mask, ind);
                let full = self.0[(start_ind + word) as usize] == 0;
                return Some((word * 32 + ind, full));
            }
        }
        None
    }
}

#[test]
fn test_alloc() {
    let mut map = FreeMap::new();     // 0|----|----+----|---------|----+----|----|31
    assert_eq!(map.req(0), Some(0));  //  |x   |    +    |    -    |    +    |    |
    assert_eq!(map.req(1), Some(2));  //  |  xx|    +    |    -    |    +    |    |
    assert_eq!(map.req(2), Some(4));  //  |    |xxxx+    |    -    |    +    |    |
    assert_eq!(map.req(2), Some(8));  //  |    |    +xxxx|    -    |    +    |    |
    assert_eq!(map.req(3), Some(16)); //  |    |    +    |    -xxxx|xxxx+    |    |
    assert_eq!(map.req(2), Some(12)); //  |    |    +    |xxxx-    |    +    |    |
    assert_eq!(map.req(2), Some(24)); //  |    |    +    |    -    |    +xxxx|    |
    assert_eq!(map.req(0), Some(1));  //  | x  |    +    |    -    |    +    |    |
    assert_eq!(map.req(1), Some(28)); //  |    |    +    |    -    |    +    |xx  |
    assert_eq!(map.req(0), Some(30)); //  |    |    +    |    -    |    +    |  x |
                                      // 1 block in the first word is free here
    assert_eq!(map.req(3), Some(32)); //  |    |    +    |    -    |    +    |    |
}

#[test]
fn test_word_alloc() {
    assert_eq!(Some(0), FreeMap::find_free(0xFFFF_FFFF, 0, 0));
    assert_eq!(Some(1), FreeMap::find_free(0xFFFF_FFFE, 0, 0));
    assert_eq!(Some(2), FreeMap::find_free(0xFFFF_FFFC, 0, 0));
    assert_eq!(Some(2), FreeMap::find_free(0xFFFF_FFFE, 0, 1));
    assert_eq!(Some(2), FreeMap::find_free(0xFFFF_FFFC, 0, 1));
    assert_eq!(Some(2), FreeMap::find_free(0xFFFF_FFFD, 0, 1));
    assert_eq!(Some(4), FreeMap::find_free(0xFFFF_FFFE, 0, 2));
    assert_eq!(Some(4), FreeMap::find_free(0xFFFF_FFFC, 0, 2));
    assert_eq!(Some(4), FreeMap::find_free(0xFFFF_FFF8, 0, 2));
    assert_eq!(Some(4), FreeMap::find_free(0xFFFF_FFF0, 0, 2));
    assert_eq!(Some(8), FreeMap::find_free(0xFFFF_FFAC, 0, 2));

    assert_eq!(Some(5), FreeMap::find_free(0xFFFF_FFE0, 0, 0));
    assert_eq!(Some(6), FreeMap::find_free(0xFFFF_FFE0, 0, 1));
    assert_eq!(Some(6), FreeMap::find_free(0xFFFF_FFC0, 0, 1));
    assert_eq!(Some(6), FreeMap::find_free(0xFFFF_FFD0, 0, 1));
    assert_eq!(Some(8), FreeMap::find_free(0xFFFF_FFE0, 0, 2));
    assert_eq!(Some(8), FreeMap::find_free(0xFFFF_FFC0, 0, 2));
    assert_eq!(Some(8), FreeMap::find_free(0xFFFF_FF80, 0, 2));
    assert_eq!(Some(8), FreeMap::find_free(0xFFFF_FF00, 0, 2));
    assert_eq!(Some(12), FreeMap::find_free(0xFFFF_FAC0, 0, 2));

    assert_eq!(None, FreeMap::find_free(0, 0, 0));
    assert_eq!(None, FreeMap::find_free(0x8000_0000, 0, 1));
    assert_eq!(None, FreeMap::find_free(0x4000_0000, 0, 1));
    assert_eq!(None, FreeMap::find_free(0x2000_0000, 0, 1));
    assert_eq!(None, FreeMap::find_free(0x1000_0000, 0, 1));
    assert_eq!(None, FreeMap::find_free(0x8000_0000, 0, 2));
    assert_eq!(None, FreeMap::find_free(0xC000_0000, 0, 2));
    assert_eq!(None, FreeMap::find_free(0xE000_0000, 0, 2));
    assert_eq!(None, FreeMap::find_free(0x0007_0000, 0, 2));
}
