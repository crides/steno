use std::collections::BTreeMap;

use crate::hash;

static RULE: &'static str = include_str!("../simple-rules.json");

#[derive(Deserialize)]
pub struct SimpleRuleEntry {
    /// The full word to be matched
    word: String,
    /// The suffix to be matched
    suffix: String,
    /// Full text replacement for the entry; should be equivalent to the result obtained using the
    /// other way
    full: String,
    /// The number of characters to be backspaced corresponding to the end of the word
    back: u8,
    /// Text to append after the partial word
    extra: String,
}

const BUCKET_SIZE: usize = 3;
const BUCKET_OFFSET_BITS: usize = 18;
const BUCKET_LENGTH_BITS: usize = 6;
const BUCKET_CAP: usize = 0x3C00; // 1463 entries in simple rules; generates 65470 bytes < 64KiB
const EMPTY_BUCKET: u32 = 0xFFFFFF;

pub fn generate() -> Vec<u8> {
    let rules: Vec<SimpleRuleEntry> = serde_json::from_str(RULE).unwrap();
    let mut buckets = vec![EMPTY_BUCKET; BUCKET_CAP];
    let mut value_blocks = Vec::new();
    let mut collisions = BTreeMap::new();
    for rule in rules {
        // Use space as unique seperator
        let hash = hash::hash(format!("{} {}", rule.word, rule.suffix).as_bytes(), None);
        let mut index = hash as usize % (BUCKET_CAP - 1);
        let mut collision = 0;
        while buckets[index] != EMPTY_BUCKET {
            index = (index + 1) % (BUCKET_CAP - 1);
            collision += 1;
        }
        collisions
            .entry(collision)
            .and_modify(|c| *c += 1)
            .or_insert(1);
        let offset = value_blocks.len();
        value_blocks.extend_from_slice(rule.word.as_bytes());
        value_blocks.push(' ' as u8);
        value_blocks.extend_from_slice(rule.suffix.as_bytes());
        value_blocks.push(0);
        value_blocks.push(rule.back as u8);
        value_blocks.extend_from_slice(rule.extra.as_bytes());
        let len = value_blocks.len() - offset;
        assert!(len < 2usize.pow(BUCKET_LENGTH_BITS as u32));
        assert!(offset < 2usize.pow(BUCKET_OFFSET_BITS as u32));
        buckets[index] = (offset | (len << BUCKET_OFFSET_BITS)) as u32;
    }

    dbg!(collisions);
    let mut final_block: Vec<u8> = buckets
        .into_iter()
        .map(|b| b.to_le_bytes()[0..BUCKET_SIZE].to_vec())
        .flatten()
        .collect();
    final_block.extend(value_blocks);
    final_block
}

#[test]
/// Check if the full replacement matches the optimized data
fn validate_simple_rules() {
    let rules: Vec<SimpleRuleEntry> = serde_json::from_str(RULE).unwrap();
    for rule in rules {
        let backed = &rule.word[..(rule.word.len() - rule.back as usize)];
        let constructed = format!("{}{}", backed, rule.extra);
        assert!(
            rule.word.len() + 1 + rule.suffix.len() + 1 + rule.extra.len()
                < 2usize.pow(BUCKET_LENGTH_BITS as u32)
        );
        assert_eq!(rule.full, constructed);
    }
}

#[test]
fn test_gen() {
    let _raw_rules = generate();
    panic!("{}", _raw_rules.len());
}
