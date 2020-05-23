use std::collections::{hash_map::Entry, HashMap};

use crate::stroke::Stroke;
use regex::Regex;

lazy_static! {
    static ref META_RE: Regex = Regex::new(r"[^{}]+|\{[^{}]*\}").unwrap();
}

pub type JsonDict = HashMap<String, String>;

#[derive(Clone, Copy, Debug, PartialEq, Eq, Hash)]
pub enum Caps {
    Lower,
    Keep,
    Caps,
    Upper,
}

#[derive(Clone, Copy, Debug, PartialEq, Eq, Hash)]
pub struct Attr {
    pub caps: Caps,
    pub space_prev: bool,
    pub space_after: bool,
    pub glue: bool,
    pub present: bool,
}

impl Attr {
    fn none() -> Self {
        Attr {
            caps: Caps::Lower,
            space_prev: false,
            space_after: false,
            glue: false,
            present: false,
        }
    }
}

impl Default for Attr {
    fn default() -> Self {
        Attr {
            caps: Caps::Lower,
            space_prev: true,
            space_after: true,
            glue: false,
            present: true,
        }
    }
}

#[derive(Debug)]
pub struct InvalidStroke(String);

#[derive(Debug, Clone)]
pub struct Dict {
    pub entry: Option<String>,
    pub attr: Attr,
    pub children: HashMap<Stroke, Dict>,
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

    pub fn parse_atom(mut s: &str) -> (Attr, String) {
        let mut attr = Attr::default();
        // Attributes for the current entry; i.e. will not appear in returning `Attr`
        if s.starts_with('{') && s.ends_with('}') {
            s = &s[1..(s.len() - 1)];
            match s {
                "?" | "!" | "." => {
                    attr.space_prev = false;
                    attr.caps = Caps::Caps;
                }
                "," | ";" | ":" => {
                    attr.space_prev = false;
                }
                "-|" => {
                    attr.caps = Caps::Caps;
                    s = "";
                }
                ">" => {
                    attr.caps = Caps::Lower;
                    attr.space_after = false;
                    s = "";
                }
                "<" => {
                    attr.caps = Caps::Upper;
                    attr.space_after = false;
                    s = "";
                }
                _ => {
                    if s.starts_with('&') {
                        s = &s[1..];
                        attr.glue = true;
                    }
                    if s.starts_with('^') {
                        s = &s[1..];
                        attr.space_prev = false;
                    }
                    if s.ends_with('^') {
                        s = &s[..(s.len() - 1)];
                        attr.space_after = false;
                    }
                    if s.starts_with("~|") {
                        s = &s[2..];
                        attr.caps = Caps::Keep;
                    }
                }
            }
        }
        (attr, s.into())
    }

    pub fn parse_entry(s: &str) -> (Attr, String) {
        let mut buf = String::new();
        let mut entry_attr = Attr {
            present: true,
            ..Attr::default()
        };
        let mut last_cap = Caps::Lower;

        let mut atoms = META_RE
            .find_iter(s)
            .map(|m| Dict::parse_atom(m.as_str()))
            .collect::<Vec<_>>();
        let len = atoms.len();
        // println!("{:#?}", atoms);
        for i in 0..len {
            let prev_attr = if i == 0 {
                Attr::default()
            } else {
                atoms[i - 1].0
            };
            let (attr, string) = &mut atoms[i];
            if prev_attr.glue && attr.glue || !prev_attr.space_after || !attr.space_prev {
                if i == 0 {
                    entry_attr.space_prev = false;
                }
            } else if buf.len() > 0 && !string.is_empty() {
                buf.push(' ');
            }
            if prev_attr.caps == Caps::Caps
                || last_cap == Caps::Caps && prev_attr.caps == Caps::Keep
            {
                let mut chars = string.chars();
                if let Some(c) = chars.next() {
                    buf.push(c.to_ascii_uppercase());
                }
                buf.extend(chars);
                last_cap = Caps::Caps;
            } else if prev_attr.caps == Caps::Upper
                || last_cap == Caps::Upper && prev_attr.caps == Caps::Keep
            {
                buf.push_str(&string.to_ascii_uppercase());
                last_cap = Caps::Upper;
            } else {
                buf.push_str(&string);
                last_cap = Caps::Lower;
            }
        }
        let last_attr = atoms.last().map(|(a, _s)| a).copied().unwrap_or_default();
        entry_attr.caps = last_attr.caps;
        entry_attr.space_after = last_attr.space_after;
        if buf.chars().all(|c| c.is_ascii_digit()) || atoms.iter().any(|(a, _s)| a.glue) {
            entry_attr.glue = true;
        }
        (entry_attr, buf)
    }

    pub fn parse_from_json(m: &JsonDict) -> Result<Dict, InvalidStroke> {
        let mut root = Dict::new(None);
        for (strokes, entry) in m.iter() {
            let mut cur_dict = &mut root;
            for stroke in strokes
                .split('/')
                .map(|stroke| stroke.parse().map_err(|_| InvalidStroke(strokes.clone())))
            {
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
        Dict {
            attr: Attr::none(),
            ..Dict::new(None)
        }
    }
}

#[test]
fn test_plain() {
    assert_eq!(Dict::parse_entry("a"), (Attr::default(), "a".into()));
}

#[test]
fn test_glue() {
    assert_eq!(
        Dict::parse_entry("{&a}{&b}"),
        (
            Attr {
                glue: true,
                ..Attr::default()
            },
            "ab".into()
        )
    );
}

#[test]
fn test_finger_spell() {
    assert_eq!(
        Dict::parse_entry("{>}{&c}"),
        (
            Attr {
                glue: true,
                ..Attr::default()
            },
            "c".into()
        )
    );
}
