/// A multilevel bit map that keeps track of which blocks are used and which aren't
/// Each level uses a 32bit bitvec. 1 means free or one of children is free. 0 means all of children or self
/// is full
pub struct FreeMap(pub Vec<u32>);

impl FreeMap {
    // NOTE: Word *not byte* indexes
    const LVL_0: u32 = 0;
    const LVL_1: u32 = (1 << 20) / 32 + FreeMap::LVL_0; // 1M entries
    const LVL_2: u32 = (1 << 20) / 32 / 32 + FreeMap::LVL_1; // 32768 entries
    const LVL_3: u32 = (1 << 20) / 32 / 32 / 32 + FreeMap::LVL_2; // 1024 entries

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
        let size: u8 = 2u8.pow(this_lvl_block as u32);
        let mask: u32 = 2u32.pow(size as u32) - 1;
        let alloc_word = self.0[(start_ind + word) as usize];
        for i in (0..32).step_by(size as usize) {
            let mask = mask << i;
            if (alloc_word & mask) == mask {
                let sub_ind = i + word * 32;
                let ret_ind = if lvl == 0 {
                    self.0[(start_ind + word) as usize] &= !mask;
                    sub_ind
                } else {
                    let r = self._req(lvl - 1, sub_ind, block);
                    if let Some((ret, full)) = r {
                        if full {
                            self.0[(start_ind + word) as usize] &= !mask;
                        }
                        ret
                    } else {
                        continue;
                    }
                };
                let full = self.0[(start_ind + word) as usize] == 0;
                return Some((ret_ind, full));
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
