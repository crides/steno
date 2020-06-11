use std::collections::HashMap;
use std::hash::{Hash, Hasher};
use std::io::{self, prelude::*};

use crate::dict::{Attr, Caps, Dict, Entry, Input};
use crate::hashmap::LPHashMap;
use crate::stroke::Stroke;

pub struct RawEntry(Vec<u8>);

impl From<Entry> for RawEntry {
    fn from(e: Entry) -> RawEntry {
        let bytes = e
            .input
            .iter()
            .flat_map(|i| match i {
                Input::String(s) => {
                    assert!(s.len() <= 127);
                    if !s.is_empty() {
                        let mut bytes = vec![s.len() as u8];
                        bytes.extend_from_slice(s.as_bytes());
                        bytes
                    } else {
                        vec![]
                    }
                }
                Input::Keycode(k) => {
                    let mut mods = 0x80;
                    if k.ctrl {
                        mods |= 0x08;
                    }
                    if k.shift {
                        mods |= 0x04;
                    }
                    if k.alt {
                        mods |= 0x02;
                    }
                    if k.gui {
                        mods |= 0x01;
                    }
                    vec![mods, k.key]
                }
            })
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
    #[derive(PartialEq, Eq, Hash, Clone, Copy)]
    pub struct RawAttr(u8);
    impl Debug;
    caps, set_caps: 1, 0;
    space_prev, set_space_prev: 2, 2;
    space_after, set_space_after: 3, 3;
    glue, set_glue: 4, 4;
    present, set_present: 5, 5;
    str_only, set_str_only: 6, 6;
}

impl From<Attr> for RawAttr {
    fn from(a: Attr) -> RawAttr {
        let mut ra = RawAttr(0);
        ra.set_caps(match a.caps {
            Caps::Lower => 0,
            Caps::Keep => 1,
            Caps::Caps => 2,
            Caps::Upper => 3,
        });
        ra.set_glue(a.glue.into());
        ra.set_space_prev(a.space_prev.into());
        ra.set_space_after(a.space_after.into());
        ra.set_present(a.present.into());
        ra.set_str_only(a.str_only.into());
        ra
    }
}

/// A hashable counter part for `Dict`, contains only the information needed
#[derive(Debug, Eq, Clone)]
pub struct HashableDict {
    pub entry: Option<Entry>,
    pub children: Vec<(Stroke, Option<Entry>)>,
}

impl PartialEq for HashableDict {
    fn eq(&self, other: &Self) -> bool {
        if self.entry != other.entry {
            return false;
        }
        for ((stroke, entry), (other_stroke, other_entry)) in
            self.children.iter().zip(other.children.iter())
        {
            if stroke != other_stroke {
                return false;
            }
            if entry.is_none() || other_entry.is_none() || entry != other_entry {
                return false;
            }
        }
        true
    }
}

impl HashableDict {
    pub fn from_dict(d: &Dict) -> Self {
        let Dict { entry, children } = d;
        let mut children: Vec<(Stroke, Option<_>)> = children
            .iter()
            .map(|(k, v)| (*k, v.entry.clone()))
            .collect();
        children.sort_by_key(|(k, _v)| *k);
        Self {
            entry: entry.clone(),
            children,
        }
    }
}

impl Hash for HashableDict {
    fn hash<H: Hasher>(&self, state: &mut H) {
        self.entry.hash(state);
        self.children.hash(state);
    }
}

struct IRNode {
    entry: Entry,
    children: LPHashMap,
}

impl IRNode {
    fn new(node_num: u32, entry: Entry) -> Self {
        let children = LPHashMap::new(node_num as usize);
        Self { entry, children }
    }

    fn add_child(&mut self, input: u32, addr: u32) {
        self.children.add_child(input, addr);
    }
}

pub struct IR {
    cur_addr: u32,
    nodes: Vec<IRNode>,
}

impl IR {
    pub fn new() -> IR {
        IR {
            cur_addr: 0,
            nodes: Vec::new(),
        }
    }

    pub fn len(&self) -> usize {
        self.nodes.len()
    }

    pub fn addr(&self) -> u32 {
        self.cur_addr
    }

    pub fn add_node(&mut self, node_num: u32, entry: Entry) {
        let len = entry.byte_len();
        // eprintln!("{:x}: {}", self.cur_addr, string);
        let new_node = IRNode::new(node_num, entry);
        self.cur_addr += 6 * new_node.children.total_size() as u32 + len as u32 + 5;
        self.nodes.push(new_node);
    }

    pub fn from_dict(dict: Dict) -> Self {
        let mut ir = IR::new();
        let mut node_cache = HashMap::new();
        let mut max_collisions = 0;
        assert_eq!(ir._from_dict(dict, &mut node_cache, &mut max_collisions), 0);
        println!("Max collisions: {}", max_collisions);
        ir
    }

    fn _from_dict(
        &mut self,
        dict: Dict,
        node_cache: &mut HashMap<HashableDict, u32>,
        max_collisions: &mut usize,
    ) -> u32 {
        let addr = self.addr();
        let mut children: Vec<(Stroke, Dict)> = dict.children.into_iter().collect();
        children.sort_by(|a, b| a.0.cmp(&b.0));
        let cur_ind = self.len();
        self.add_node(children.len() as u32, dict.entry.unwrap_or_default());

        for child in children.into_iter() {
            let hash = HashableDict::from_dict(&child.1);
            let child_addr = if !node_cache.contains_key(&hash) {
                let child_addr = self._from_dict(child.1, node_cache, max_collisions);
                if hash.children.iter().all(|(_s, ent)| ent.is_some()) {
                    node_cache.insert(hash.clone(), child_addr);
                }
                child_addr
            } else {
                node_cache.get(&hash).copied().unwrap()
            };
            self.nodes[cur_ind].add_child(child.0.raw(), child_addr);
        }
        let stats = self.nodes[cur_ind].children.stats();
        if stats.0 > *max_collisions {
            *max_collisions = stats.0;
        }
        addr
    }

    pub fn write(&self, w: &mut dyn Write) -> io::Result<()> {
        let mut bytes: Vec<u8> = Vec::new();
        for node in self.nodes.iter() {
            bytes.clear();
            // Note: this is 3 bytes long. MSB is at the end cuz LE.
            bytes.extend_from_slice(&(node.children.total_size() as u32).to_le_bytes()[0..3]);
            bytes.push(node.entry.byte_len() as u8);
            let attr = node.entry.attr;
            let raw_attr: RawAttr = attr.into();
            bytes.push(raw_attr.0);
            let raw_entry: RawEntry = node.entry.clone().into();
            if attr.str_only {
                bytes.extend_from_slice(&raw_entry.as_bytes()[1..]);
            } else {
                bytes.extend_from_slice(raw_entry.as_bytes());
            }
            w.write_all(&bytes)?;
            node.children.write_all_to(w)?;
        }
        Ok(())
    }
}
