use std::collections::{HashMap, hash_map::Entry};

use serde_json::{Map, Value};

use crate::stroke::Stroke;

#[derive(Debug)]
pub enum ParseDictErr {
    InvalidStroke(String),
    InvalidEntry(String),
}

pub struct Dict {
    pub entry: Option<String>,
    pub children: HashMap<Stroke, Dict>,
}

impl Dict {
    pub fn new(entry: Option<String>) -> Dict {
        Dict {
            entry,
            children: HashMap::new(),
        }
    }

    pub fn set_entry(&mut self, entry: String) {
        self.entry = Some(entry);
    }

    pub fn entry(&mut self, stroke: Stroke) -> Entry<Stroke, Dict> {
        self.children.entry(stroke)
    }

    pub fn parse_from_json(m: &Map<String, Value>) -> Result<Dict, ParseDictErr> {
        let mut root = Dict::new(None);
        for (strokes, entry) in m.iter() {
            let entry = entry
                .as_str()
                .ok_or(ParseDictErr::InvalidEntry(strokes.clone()))?;
            let mut cur_dict = &mut root;
            for stroke in strokes
                .split('/')
                .map(|stroke| {
                    stroke
                        .parse()
                        .map_err(|_| ParseDictErr::InvalidStroke(strokes.clone()))
                })
            {
                let stroke = stroke?;
                cur_dict = cur_dict.entry(stroke).or_default();
            }
            cur_dict.set_entry(entry.into());
        }
        Ok(root)
    }
}

impl Default for Dict {
    fn default() -> Dict {
        Dict::new(None)
    }
}
