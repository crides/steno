use std::collections::{hash_map::Entry, HashMap};
use std::hash::{Hash, Hasher};

use crate::stroke::Stroke;
use regex::Regex;

lazy_static! {
    static ref META_RE: Regex = Regex::new(r"[^{}]+|\{[^{}]*\}").unwrap();
}

pub type JsonDict = HashMap<String, String>;

bitfield! {
    #[derive(PartialEq, Eq, Hash, Clone, Copy)]
    pub struct Attr(u8);
    impl Debug;
    caps, set_caps: 1, 0;
    space_prev, set_space_prev: 2, 2;
    space_after, set_space_after: 3, 3;
    glue, set_glue: 4, 4;
    all_caps, set_all_caps: 5, 5;
    all_lower, set_all_lower: 6, 6;
}

impl Default for Attr {
    fn default() -> Self {
        let mut attr = Attr(0);
        attr.set_space_prev(1);
        attr.set_space_after(1);
        attr
    }
}

#[derive(Debug)]
pub enum ParseDictErr {
    InvalidStroke(String),
    InvalidEntry(String),
}

#[derive(Debug, Clone)]
pub struct Dict {
    pub entry: Option<String>,
    pub attr: Attr,
    pub children: HashMap<Stroke, Dict>,
}

/// A hashable counter part for `Dict`, contains only the information needed
#[derive(Debug, Eq, Clone)]
pub struct HashableDict {
    pub entry: Option<String>,
    pub attr: Attr,
    pub keys: Vec<(Stroke, Option<String>)>,
}

impl PartialEq for HashableDict {
    fn eq(&self, other: &Self) -> bool {
        if self.entry != other.entry {
            return false;
        }
        if self.attr != other.attr {
            return false;
        }
        for (key, other_key) in self.keys.iter().zip(other.keys.iter()) {
            if key.0 != other_key.0 {
                return false;
            }
            if key.1.is_none() || other_key.1.is_none() || key.1 != other_key.1 {
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
        let mut keys: Vec<(Stroke, Option<String>)> = children
            .iter()
            .map(|(k, v)| (*k, v.entry.clone()))
            .collect();
        keys.sort();
        Self {
            entry: entry.clone(),
            attr: *attr,
            keys,
        }
    }
}

impl Hash for HashableDict {
    fn hash<H: Hasher>(&self, state: &mut H) {
        self.entry.hash(state);
        self.attr.hash(state);
        self.keys.hash(state);
    }
}

impl Dict {
    pub fn new(entry: Option<String>) -> Dict {
        Dict {
            entry,
            attr: Attr::default(),
            children: HashMap::new(),
        }
    }

    pub fn set_entry(&mut self, entry: String) {
        self.entry = Some(entry);
    }

    pub fn entry(&mut self, stroke: Stroke) -> Entry<Stroke, Dict> {
        self.children.entry(stroke)
    }

    pub fn parse_entry(s: &str) -> (Attr, String) {
        let mut buf = String::new();
        let mut attr = Attr::default();

        let (mut space, mut caps, mut glue) = (true, false, false);
        let atoms = META_RE.find_iter(s).collect::<Vec<_>>();
        // println!("{:#?}", atoms);
        for i in 0..atoms.len() {
            let mat = atoms[i];
            let mut mat_s = mat.as_str();
            if mat_s.starts_with('{') && mat_s.ends_with('}') {
                mat_s = &mat_s[1..(mat_s.len() - 1)];
                match mat_s {
                    "?" | "!" | "." => {
                        buf.push_str(mat_s);
                        space = true;
                        caps = true;
                        if i == 0 {
                            attr.set_space_prev(0);
                        }
                        if i == atoms.len() - 1 {
                            attr.set_caps(3);
                            attr.set_space_after(1);
                        }
                    }
                    "," | ";" | ":" => {
                        buf.push_str(mat_s);
                        space = true;
                        caps = false;
                        if i == 0 {
                            attr.set_space_prev(0);
                        }
                        if i == atoms.len() - 1 {
                            attr.set_space_after(1);
                        }
                    }
                    _ => {
                        if mat_s.starts_with('^') {
                            mat_s = &mat_s[1..];
                            if i == 0 {
                                attr.set_space_prev(0);
                            }
                        }
                        if mat_s.ends_with('^') {
                            mat_s = &mat_s[..(mat_s.len() - 1)];
                            if i == atoms.len() - 1 {
                                attr.set_space_after(0);
                            }
                            space = false;
                        }
                        if attr.space_prev() == 1 && i > 0 {
                            buf.push(' ');
                        }
                        buf.push_str(mat_s);
                    }
                }
            } else {
                if space && i > 0 {
                    buf.push(' ');
                }
                if caps {
                    caps = false;
                    buf.push(mat_s.chars().next().unwrap().to_ascii_uppercase());
                    buf.push_str(&mat_s[1..]);
                } else {
                    buf.push_str(mat_s);
                }
            }
        }
        if buf.chars().all(|c| c.is_ascii_digit()) {
            attr.set_glue(1);
        }
        (attr, buf)
    }

    pub fn parse_from_json(m: &JsonDict) -> Result<Dict, ParseDictErr> {
        let mut root = Dict::new(None);
        for (strokes, entry) in m.iter() {
            let mut cur_dict = &mut root;
            for stroke in strokes.split('/').map(|stroke| {
                stroke
                    .parse()
                    .map_err(|_| ParseDictErr::InvalidStroke(strokes.clone()))
            }) {
                let stroke = stroke?;
                cur_dict = cur_dict.entry(stroke).or_default();
            }
            let (attr, entry) = Dict::parse_entry(entry);
            cur_dict.set_entry(entry);
            cur_dict.attr = attr;
        }
        Ok(root)
    }
}

impl Default for Dict {
    fn default() -> Dict {
        Dict::new(None)
    }
}
