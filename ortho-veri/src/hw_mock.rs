use std::sync::RwLock;
use lazy_static::lazy_static;

pub const ORTHOGRAPHY_START: usize = 0xf30000;
pub const ORTHOGRAPHY_SIZE: usize = 64 * 2usize.pow(10);
#[allow(dead_code)]
pub const ORTHO_BUCKET_NUM: usize = 0x3C00;
#[allow(dead_code)]
pub const ORTHO_BUCKET_SIZE: usize = 3;

lazy_static! {
    pub static ref STORAGE: RwLock<Option<Vec<u8>>> = RwLock::new(None);
}

#[no_mangle]
pub extern "C" fn store_read(offset: u32, buf: *mut u8, len: u8) {
    let storage = STORAGE.read().unwrap();
    if let Some(ref storage) = *storage {
        assert!(storage.len() >= ORTHOGRAPHY_SIZE);
        assert!(offset as usize >= ORTHOGRAPHY_START && offset as usize + len as usize <= ORTHOGRAPHY_START + ORTHOGRAPHY_SIZE);
        unsafe {
            buf.copy_from(storage[offset as usize..].as_ptr(), len as usize);
        }
    } else {
        panic!("storage not inited");
    }
}
