use std::io::{self, prelude::*};

use bitfield::BitRange;

use crate::dict::{Dict, Attr};
use crate::stroke::Stroke;
use crate::hashmap::LPHashMap;

struct IRNode {
    node_num: u32,
    string: String,
    attr: Attr,
    children: LPHashMap,
}

impl IRNode {
    fn new(node_num: u32, string: String, attr: Attr) -> Self {
        let children = LPHashMap::new(node_num as usize);
        Self {
            node_num: children.total_size() as u32,
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
        let new_node = IRNode::new(node_num, string, attr);
        self.cur_addr += 6 * new_node.children.total_size() as u32 + len as u32 + 5;
        self.nodes.push(new_node);
    }

    pub fn from_dict(dict: Dict) -> Self {
        let mut ir = IR::new();
        assert_eq!(ir._from_dict(dict), 0);
        ir
    }

    fn _from_dict(&mut self, dict: Dict) -> u32 {
        let addr = self.addr();
        let mut children: Vec<(Stroke, Dict)> = dict.children.into_iter().collect();
        children.sort_by(|a, b| a.0.cmp(&b.0));
        let cur_ind = self.len();
        self.add_node(children.len() as u32, dict.entry.unwrap_or("".into()), dict.attr);

        for child in children.into_iter() {
            let entry: String = child.1.entry.clone().unwrap_or("None".into());
            let child_addr = self._from_dict(child.1);
            self.nodes[cur_ind].add_child(child.0.raw(), child_addr);
        }
        let stats = self.nodes[cur_ind].children.stats();
        println!("{} {} {}", stats.0, stats.1, stats.2);
        addr
    }

    pub fn write(&self, w: &mut dyn Write) -> io::Result<()> {
        let mut bytes: Vec<u8> = Vec::new();
        for node in self.nodes.iter() {
            bytes.clear();
            let string_len = node.string.len();
            bytes.extend_from_slice(&(node.children.total_size() as u32).to_le_bytes()[0..3]);
            bytes.push(string_len as u8);
            bytes.push(node.attr.0);
            bytes.extend_from_slice(&node.string.clone().into_bytes());
            w.write_all(&bytes)?;
            node.children.write_all_to(w)?;
        }
        Ok(())
    }
}
