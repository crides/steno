use std::collections::{hash_map::Entry as MapEntry, HashMap};

use crate::stroke::Stroke;
use regex::Regex;

lazy_static! {
    static ref META_RE: Regex = Regex::new(r"[^{}]+|\{[^{}]*\}").unwrap();
}

pub type JsonDict = HashMap<String, String>;

#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
pub struct Keycode {
    pub ctrl: bool,
    pub shift: bool,
    pub alt: bool,
    pub gui: bool,
    pub key: u8,
}

impl Keycode {
    pub fn new(key: u8) -> Keycode {
        Keycode {
            ctrl: false,
            shift: false,
            alt: false,
            gui: false,
            key,
        }
    }

    pub fn ctrl(mut self) -> Self {
        self.ctrl = true;
        self
    }

    pub fn shift(mut self) -> Self {
        self.shift = true;
        self
    }

    pub fn alt(mut self) -> Self {
        self.alt = true;
        self
    }

    pub fn gui(mut self) -> Self {
        self.gui = true;
        self
    }
}

#[derive(Clone, Debug, PartialEq, Eq, Hash)]
pub enum Input {
    Keycode(Keycode),
    String(String),
}

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

    fn valid_default() -> Self {
        Attr {
            caps: Caps::Lower,
            space_prev: true,
            space_after: true,
            glue: false,
            present: true,
        }
    }
}

#[derive(Clone, Debug, PartialEq, Eq, Hash)]
pub struct Entry {
    pub attr: Attr,
    pub input: Vec<Input>,
}

impl Default for Entry {
    fn default() -> Entry {
        Entry {
            attr: Attr::none(),
            input: Vec::new(),
        }
    }
}

impl Entry {
    pub fn len(&self) -> usize {
        self.input.iter().map(|i| {
            match i {
                Input::String(s) => if s.is_empty() { 0 } else { s.len() + 1 },
                Input::Keycode(_) => 2,
            }
        }).sum()
    }
    
    fn parse_atom(mut s: &str) -> (Attr, Input) {
        // Attributes for the current entry; i.e. will not appear in returning `Attr`
        let mut attr = Attr::valid_default();
        if s.starts_with('{') && s.ends_with('}') {
            s = &s[1..(s.len() - 1)];
            if s.starts_with('#') {
                let key = match &s[1..] {
                    "BackSpace" => 0x2a,
                    "Delete" => 0x4c,
                    "End" => 0x4d,
                    "Escape" => 0x29,
                    "Home" => 0x4a,
                    "Insert" => 0x49,
                    "Page_Down" => 0x4e,
                    "Page_Up" => 0x4b,
                    "Return" => 0x28,
                    "Right" => 0x4f,
                    "Left" => 0x50,
                    "Down" => 0x51,
                    "Up" => 0x52,
                    "Tab" => 0x2b,
                    _ => 0,
                };
                (attr, Input::Keycode(Keycode::new(key)))
            } else {
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
                        s = "";
                    }
                    "<" => {
                        attr.caps = Caps::Upper;
                        s = "";
                    }
                    "^" => {
                        attr.space_prev = false;
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
                (attr, Input::String(s.into()))
            }
        } else {
            (attr, Input::String(s.into()))
        }
    }

    pub fn parse_entry(s: &str) -> Entry {
        let mut buf = String::new();
        let mut entry = Entry {
            attr: Attr::valid_default(),
            input: Vec::new(),
        };
        let mut last_cap = Caps::Lower;

        let mut atoms = META_RE
            .find_iter(s)
            .map(|m| Entry::parse_atom(m.as_str()))
            .collect::<Vec<_>>();
        let len = atoms.len();
        for i in 0..len {
            let prev_attr = if i == 0 {
                Attr::valid_default()
            } else {
                atoms[i - 1].0
            };
            let (attr, input) = &mut atoms[i];
            match input {
                Input::Keycode(_) => {
                    entry.input.push(input.clone());
                }
                Input::String(s) => {
                    if prev_attr.glue && attr.glue || !prev_attr.space_after || !attr.space_prev {
                        if i == 0 {
                            entry.attr.space_prev = false;
                        }
                    } else {
                        if buf.len() > 0 && !s.is_empty() {
                            buf.push(' ');
                        }
                    }
                    if prev_attr.caps == Caps::Caps
                        || last_cap == Caps::Caps && prev_attr.caps == Caps::Keep
                    {
                        let mut chars = s.chars();
                        if let Some(c) = chars.next() {
                            buf.push(c.to_ascii_uppercase());
                        }
                        buf.extend(chars);
                        last_cap = Caps::Caps;
                    } else if prev_attr.caps == Caps::Upper
                        || last_cap == Caps::Upper && prev_attr.caps == Caps::Keep
                    {
                        buf.push_str(&s.to_ascii_uppercase());
                        last_cap = Caps::Upper;
                    } else {
                        buf.push_str(&s);
                        last_cap = Caps::Lower;
                    }
                    if buf.is_empty() && s.is_empty() {
                        attr.space_after = attr.space_after && prev_attr.space_after;
                        attr.space_prev = attr.space_prev && prev_attr.space_prev;
                    }
                    if let Some(Input::String(prev_str)) = entry.input.last_mut() {
                        prev_str.push_str(&s);
                    } else {
                        entry.input.push(input.clone());
                    }
                }
            }
        }
        let last_attr = atoms.last().map(|(a, _s)| a).copied().unwrap_or(Attr::valid_default());
        entry.attr.caps = last_attr.caps;
        entry.attr.space_after = last_attr.space_after;
        if !buf.is_empty() && buf.chars().all(|c| c.is_ascii_digit()) || atoms.iter().any(|(a, _s)| a.glue) {
            entry.attr.glue = true;
        }
        entry
    }
}

#[derive(Debug)]
pub struct InvalidStroke(String);

#[derive(Debug, Clone)]
pub struct Dict {
    pub entry: Option<Entry>,
    pub children: HashMap<Stroke, Dict>,
}

impl Dict {
    pub fn new(entry: Option<Entry>) -> Dict {
        Dict {
            entry,
            children: HashMap::new(),
        }
    }

    pub fn set_entry(&mut self, entry: Entry) {
        self.entry = Some(entry);
    }

    pub fn entry(&mut self, stroke: Stroke) -> MapEntry<Stroke, Dict> {
        self.children.entry(stroke)
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
            let entry = Entry::parse_entry(entry);
            cur_dict.set_entry(entry);
        }
        Ok(root)
    }
}

impl Default for Dict {
    fn default() -> Dict {
        Dict::new(None)
    }
}

#[test]
fn test_plain() {
    assert_eq!(Entry::parse_entry("a"), Entry { attr: Attr::valid_default(), input: vec![Input::String("a".into())] });
}

#[test]
fn test_finger_spell() {
    assert_eq!(
        Entry::parse_entry("{>}{&c}"),
        (
            Entry {
                attr: Attr {
                    glue: true,
                    ..Attr::valid_default()
                },
                input: vec![Input::String("c".into())],
            }
        )
    );
}

#[test]
fn test_cap() {
    assert_eq!(
        Entry::parse_entry("{-|}"),
        (
            Entry {
                attr: Attr {
                    caps: Caps::Caps,
                    ..Attr::valid_default()
                },
                input: vec![Input::String("".into())],
            }
        )
    );
}

#[test]
fn test_cap_star() {
    assert_eq!(
        Entry::parse_entry("{^}{-|}"),
        (
            Entry {
                attr: Attr {
                    caps: Caps::Caps,
                    space_prev: false,
                    space_after: false,
                    ..Attr::valid_default()
                },
                input: vec![Input::String("".into())],
            }
        )
    );
}

#[test]
fn test_entry_len() {
    assert_eq!(Entry::parse_entry("{^}{#Return}{^}{-|}").len(), 2);
}
