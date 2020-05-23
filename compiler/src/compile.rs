use std::collections::HashMap;
use std::hash::{Hash, Hasher};
use std::io::{self, prelude::*};

use crate::dict::{Attr, Caps, Dict};
use crate::hashmap::LPHashMap;
use crate::stroke::Stroke;

bitfield! {
    #[derive(PartialEq, Eq, Hash, Clone, Copy)]
    pub struct RawAttr(u8);
    impl Debug;
    caps, set_caps: 1, 0;
    space_prev, set_space_prev: 2, 2;
    space_after, set_space_after: 3, 3;
    glue, set_glue: 4, 4;
    present, set_present: 5, 5;
}

impl Default for RawAttr {
    fn default() -> Self {
        Attr::default().into()
    }
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
        ra
    }
}

/// A hashable counter part for `Dict`, contains only the information needed
#[derive(Debug, Eq, Clone)]
pub struct HashableDict {
    pub entry: Option<String>,
    pub attr: Attr,
    pub children: Vec<(Stroke, Option<String>)>,
}

impl PartialEq for HashableDict {
    fn eq(&self, other: &Self) -> bool {
        if self.entry != other.entry {
            return false;
        }
        if self.attr != other.attr {
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
        let Dict {
            entry,
            attr,
            children,
        } = d;
        let mut children: Vec<(Stroke, Option<String>)> = children
            .iter()
            .map(|(k, v)| (*k, v.entry.clone()))
            .collect();
        children.sort();
        Self {
            entry: entry.clone(),
            attr: *attr,
            children,
        }
    }
}

impl Hash for HashableDict {
    fn hash<H: Hasher>(&self, state: &mut H) {
        self.entry.hash(state);
        self.attr.hash(state);
        self.children.hash(state);
    }
}

struct IRNode {
    string: String,
    attr: Attr,
    children: LPHashMap,
}

impl IRNode {
    fn new(node_num: u32, string: String, attr: Attr) -> Self {
        let children = LPHashMap::new(node_num as usize);
        Self {
            string,
            attr,
            children,
        }
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

    pub fn add_node(&mut self, node_num: u32, string: String, attr: Attr) {
        let len = string.len();
        // eprintln!("{:x}: {}", self.cur_addr, string);
        let new_node = IRNode::new(node_num, string, attr);
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
        self.add_node(
            children.len() as u32,
            dict.entry.unwrap_or("".into()),
            dict.attr,
        );

        for child in children.into_iter() {
            let hash = HashableDict::from_dict(&child.1);
            let child_addr = if !node_cache.contains_key(&hash) {
                let child_addr = self._from_dict(child.1, node_cache, max_collisions);
                if hash.children.iter().all(|(_s, ent)| ent.is_some()) {
                    node_cache.insert(hash.clone(), child_addr);
                }
                child_addr
            } else {
                // if hash.children.len() > 0 {
                //     println!("{}, {:#?}", child.0, hash);
                // }
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
            let string_len = node.string.len();
            // Note: this is 3 bytes long. MSB is at the end cuz LE.
            bytes.extend_from_slice(&(node.children.total_size() as u32).to_le_bytes()[0..3]);
            bytes.push(string_len as u8);
            let raw_attr: RawAttr = node.attr.into();
            bytes.push(raw_attr.0);
            bytes.extend_from_slice(&node.string.clone().into_bytes());
            w.write_all(&bytes)?;
            node.children.write_all_to(w)?;
        }
        Ok(())
    }
}
