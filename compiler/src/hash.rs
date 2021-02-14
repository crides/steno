//! FNV 1a hash
pub fn hash(bytes: &[u8], init: Option<u32>) -> u32 {
    let mut hash = init.unwrap_or(0x811c9dc5u32);
    for byte in bytes {
        hash = hash.wrapping_mul(0x01000193);
        hash ^= *byte as u32;
    }
    hash
}
