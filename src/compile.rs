use std::io::{self, prelude::*};

use crate::dict::Dict;
use crate::stroke::Stroke;

struct IRNode {
    node_num: u16,
    level: u8,
    string: String,
    children: Vec<(u32, u32)>,
}

impl IRNode {
    fn new(node_num: u16, level: u8, string: String) -> Self {
        Self {
            node_num,
            level,
            string,
            children: Vec::with_capacity(node_num as usize),
        }
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

    pub fn add_node(&mut self, node_num: u16, level: u8, string: String) {
        self.cur_addr += 8 * node_num as u32 + string.len() as u32 + 4;
        self.nodes.push(IRNode::new(node_num, level, string));
    }

    pub fn from_dict(dict: Dict) -> Self {
        let mut ir = IR::new();
        ir._from_dict(dict, 0);
        ir
    }

    fn _from_dict(&mut self, dict: Dict, level: u8) -> u32 {
        let addr = self.addr();
        let mut children: Vec<(Stroke, Dict)> = dict.children.into_iter().collect();
        children.sort_by(|a, b| a.0.cmp(&b.0));
        let cur_ind = self.len();
        self.add_node(children.len() as u16, level, dict.entry.unwrap_or("".into()));

        for child in children.into_iter() {
            let child_addr = self._from_dict(child.1, level + 1);
            self.nodes[cur_ind].children.push((child.0.raw(), child_addr));
        }
        addr
    }

    pub fn write(&self, w: &mut dyn Write) -> io::Result<()> {
        for node in self.nodes.iter() {
            let node_num = node.node_num;
            let string_len = node.string.len();
            let mut bytes: Vec<u8> = Vec::new();
            bytes.extend_from_slice(&node_num.to_le_bytes());
            bytes.push(node.level);
            bytes.push(string_len as u8);
            bytes.extend_from_slice(&node.string.clone().into_bytes());
            for child in node.children.iter() {
                bytes.extend_from_slice(&child.0.to_le_bytes());
                bytes.extend_from_slice(&child.1.to_le_bytes());
            }
            w.write_all(&bytes).expect("write bytes");
        }
        Ok(())
    }
}
