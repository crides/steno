/// A multilevel bit map that keeps track of which blocks are used and which aren't
/// Each level uses a 32bit bitvec. 1 means free or one of children is free. 0 means all of children or self
/// is full
pub struct FreeMap(Vec<u32>);

fn log2(n: u32, block: u32) -> u32 {
    let mut n = match block {
        0 => n,
        1 => n & (n << 1) & 0xAAAA_AAAA,
        2 => n & (n << 1) & (n << 2) & (n << 3) & 0x8888_8888,
        _ => panic!(),
    };
    let mut r = 0u32;
    for i in (0..5).rev() {
        let mask = 1 << i;
        if n & (u32::MAX << mask) != 0 {
            n >>= mask;
            r += mask;
        }
    }
    r
}

impl FreeMap {
    pub fn new() -> FreeMap {
        FreeMap(vec![0xFFFF_FFFF; 1 + 32 + 32usize.pow(2) + 32usize.pow(3)])
    }

    /// Request a 16 byte block, returning block number
    fn req(&mut self, block: u32) -> u32 {
        // 1 based indexing
        let mut word = 1 + (31 - log2(self.0[0], 0));
        word = 1 + word * 32 + (31 - log2(self.0[word as usize], 0));
        word = 1 + word * 32 + (31 - log2(self.0[word as usize], 0));
        let bit = 31 - log2(self.0[word as usize], block);
        let addr = (word - 1057) * 32 + bit;
        assert!(addr < 2u32.pow(20));
        let thing = 2u32.pow(1 << block) - 1;   // Get block of continous 1's with count 1 << block
        self.0[word as usize] &= !(thing << (31 - bit - ((1 << block) - 1)));   // Move the block to the right, aligning MSB of block to bit
        for _level in 0..3 {
            let (bit, next) = ((word - 1) % 32, (word - 1) / 32);
            if self.0[word as usize] == 0 {
                self.0[next as usize] &= !(1 << (31 - bit));
            }
            word = next;
        }
        addr
    }

    pub fn req_16(&mut self) -> u32 {
        self.req(0)
    }

    pub fn req_32(&mut self) -> u32 {
        let addr = self.req_16();
        self.req_16();
        addr
        // self.req(1)
    }

    pub fn req_64(&mut self) -> u32 {
        let addr = self.req_16();
        self.req_16();
        self.req_16();
        self.req_16();
        addr
        // self.req(2)
    }
}

#[test]
fn test() {
    let mut map = FreeMap::new();
    assert_eq!(map.req_16(), 0);
    println!("{:08x}", map.0[1057]);
    assert_eq!(map.req_32(), 1/*2*/);
    println!("{:08x}", map.0[1057]);
    assert_eq!(map.req_16(), 3/*1*/);
    println!("{:08x}", map.0[1057]);
    assert_eq!(map.req_16(), 4/*4*/);
    println!("{:08x}", map.0[1057]);
    assert_eq!(map.req_64(), 5/*8*/);
    println!("{:08x}", map.0[1057]);
    assert_eq!(map.req_64(), 9/*12*/);
    println!("{:08x}", map.0[1057]);
    assert_eq!(map.req_64(), 13/*16*/);
    println!("{:08x}", map.0[1057]);
    assert_eq!(map.req_64(), 17/*20*/);
    println!("{:08x}", map.0[1057]);
    assert_eq!(map.req_64(), 21/*24*/);
    println!("{:08x}", map.0[1057]);
    assert_eq!(map.req_64(), 25/*28*/);
    println!("{:08x}", map.0[1057]);
    assert_eq!(map.req_64(), 29/*32*/);
    println!("{:08x}", map.0[33]);
    println!("{:08x} {:08x}", map.0[1057], map.0[1058]);
    assert_eq!(map.req_64(), 33/*36*/);
    println!("{:08x}", map.0[33]);
    println!("{:08x} {:08x}", map.0[1057], map.0[1058]);
}

#[test]
fn test_log()  {
    assert_eq!(31, log2(0xFFFF_FFFF, 0));
    assert_eq!(29, log2(0x7FFF_FFFF, 1));
    assert_eq!(29, log2(0x3FFF_FFFF, 1));
    assert_eq!(29, log2(0xBFFF_FFFF, 1));
    assert_eq!(27, log2(0x7FFF_FFFF, 2));
    assert_eq!(27, log2(0x3FFF_FFFF, 2));
    assert_eq!(27, log2(0x1FFF_FFFF, 2));
    assert_eq!(27, log2(0x0FFF_FFFF, 2));
    assert_eq!(23, log2(0xACFF_FFFF, 2));
}
