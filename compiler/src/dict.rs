//! Provides value level types and constructs for parsing and representing the JSON dictionary. May contain
//! values which are already byte level (e.g. keycodes) for simplicity
use std::collections::{hash_map::Entry as MapEntry, HashMap};
use std::fmt::Display;

use lalrpop_util::ParseError;
use regex::Regex;

use crate::bar::progress_bar;
use crate::keycode::TermListParser;
use crate::stroke::Stroke;

lazy_static! {
    /// Stolen from somewhere in Plover. Matches against atoms inside a meta string (either `{}` enclosed
    /// atoms or not enclosed plain strings)
    static ref META_RE: Regex = Regex::new(r"[^{}]+|\{[^{}]*\}").unwrap();
}

pub type JsonDict = HashMap<String, String>;

#[derive(Clone, Debug, PartialEq, Eq, Hash)]
pub enum Input {
    /// Raw HID keycodes, with `0xE0` to `0xE3` toggling Ctrl, Shift, Alt, Super (GUI/Windows key) respectively
    Keycodes(Vec<u8>),
    /// Strings, which can contain Unicode characters
    String(String),
}

#[derive(Clone, Copy, Debug, PartialEq, Eq, Hash)]
pub enum Caps {
    Lower,
    /// Keep capitalization state for the next atom
    Keep,
    /// Caps
    Caps,
    /// UPPER
    Upper,
}

/// Simplified attributes for formatting atoms or entries for easy MCU consumption. Also includes other
/// properties about the entry itself 
#[derive(Clone, Copy, Debug, PartialEq, Eq, Hash)]
pub struct Attr {
    pub caps: Caps,
    pub space_prev: bool,
    pub space_after: bool,
    pub glue: bool,
    /// Whether this entry has an output or not. Note that 0-length entries can be outputs e.g. ones that
    /// contain only attribute changes but no printable content.
    pub present: bool,
    /// Whether the entry contains only strings i.e. no keycodes, which means no header byte required, leading
    /// to some space savings
    pub str_only: bool,
}

impl Attr {
    fn none() -> Self {
        Attr {
            caps: Caps::Lower,
            space_prev: false,
            space_after: false,
            glue: false,
            present: false,
            str_only: false,
        }
    }

    fn valid_default() -> Self {
        Attr {
            caps: Caps::Lower,
            space_prev: true,
            space_after: true,
            glue: false,
            present: true,
            str_only: false,
        }
    }
}

/// Entry value associated to a key in the dictionary
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
    fn unicode_string_len(s: &str) -> usize {
        s.chars().map(|c| if c.is_ascii() { 1 } else { 4 }).sum()
    }

    /// Total length in bytes for the entry. Used to cheaply determine the size of a certain entry
    pub fn byte_len(&self) -> usize {
        if self.input.len() == 1 {
            if let Input::String(s) = &self.input[0] {
                return Entry::unicode_string_len(s);
            }
        }

        self.input
            .iter()
            .map(|i| match i {
                Input::String(s) => {
                    if s.is_empty() {
                        0
                    } else {
                        Entry::unicode_string_len(s) + 1
                    }
                }
                Input::Keycodes(k) => k.len() + 1,
            })
            .sum()
    }

    /// Length for this entry if it's a string
    pub fn strlen(&self) -> Option<usize> {
        if self.input.len() == 1 {
            if let Some(Input::String(s)) = self.input.first() {
                Some(s.len())
            } else {
                None
            }
        } else {
            None
        }
    }

    /// Parse a single atom in an `Entry`. Can be either `{}` enclosed or not
    fn parse_atom(mut s: &str) -> Result<(Attr, Input), ParseDictError> {
        // Attributes for the current entry; i.e. will not appear in returning `Attr`
        let mut attr = Attr::valid_default();
        if s.starts_with('{') && s.ends_with('}') {
            s = &s[1..(s.len() - 1)];
            if s.starts_with('#') {
                let keycodes = TermListParser::new()
                    .parse(&s[1..])?
                    .into_iter()
                    .flat_map(|t| t.into_keycodes())
                    .collect();
                Ok((attr, Input::Keycodes(keycodes)))
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
                Ok((attr, Input::String(s.into())))
            }
        } else {
            Ok((attr, Input::String(s.into())))
        }
    }

    /// Parses the whole entry. Calls `parse_atom` and joins the inputs and attributes together, changing the
    /// contents when necessary
    pub fn parse_entry(s: &str) -> Result<Entry, ParseDictError> {
        let mut entry = Entry {
            attr: Attr::valid_default(),
            input: Vec::new(),
        };
        let mut last_cap = Caps::Lower;

        let mut atoms = META_RE
            .find_iter(s)
            .map(|m| Entry::parse_atom(m.as_str()))
            .collect::<Result<Vec<(Attr, Input)>, ParseDictError>>()?;
        let len = atoms.len();
        for i in 0..len {
            let prev_attr = if i == 0 {
                Attr::valid_default()
            } else {
                atoms[i - 1].0
            };
            let (attr, input) = &mut atoms[i];
            match input {
                Input::Keycodes(keycodes) => {
                    if let Some(Input::Keycodes(prev_keycodes)) = entry.input.last_mut() {
                        prev_keycodes.extend(keycodes.iter());
                    } else {
                        entry.input.push(input.clone());
                    }
                }
                Input::String(s) => {
                    let mut buf = String::new();
                    if prev_attr.glue && attr.glue || !prev_attr.space_after || !attr.space_prev {
                        if i == 0 {
                            entry.attr.space_prev = false;
                        }
                    } else if entry.strlen().unwrap_or(0) > 0 && !s.is_empty() {
                        buf.push(' ');
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
                    if entry.strlen().unwrap_or(0) == 0 && s.is_empty() {
                        attr.space_after = attr.space_after && prev_attr.space_after;
                        attr.space_prev = attr.space_prev && prev_attr.space_prev;
                    }
                    if let Some(Input::String(prev_str)) = entry.input.last_mut() {
                        prev_str.push_str(&buf);
                    } else if !buf.is_empty() {
                        entry.input.push(Input::String(buf));
                    }
                }
            }
        }
        let last_attr = atoms
            .last()
            .map(|(a, _s)| a)
            .copied()
            .unwrap_or_else(Attr::valid_default);
        entry.attr.caps = last_attr.caps;
        entry.attr.space_after = last_attr.space_after;
        if entry.input.len() == 1 {
            if let Some(Input::String(s)) = entry.input.first() {
                entry.attr.str_only = true;
                if !s.is_empty() && s.chars().all(|c| c.is_ascii_digit()) {
                    entry.attr.glue = true;
                }
            }
        }
        if atoms.iter().any(|(a, _s)| a.glue) {
            entry.attr.glue = true;
        }
        Ok(entry)
    }
}

#[derive(Debug)]
pub enum ParseDictError {
    InvalidStroke(String),
    InvalidKeycode(String),
    ParseError(String),
}

impl<L: Display, T: Display> From<ParseError<L, T, String>> for ParseDictError {
    fn from(e: ParseError<L, T, String>) -> ParseDictError {
        if let ParseError::User { error } = e {
            ParseDictError::InvalidKeycode(error)
        } else {
            ParseDictError::ParseError(e.to_string())
        }
    }
}

/// Parsed value representation for a JSON dictionary. Is of tree like structure to facilitate further
/// compilation
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

    pub fn parse_from_json(m: &JsonDict) -> Result<Dict, ParseDictError> {
        let mut root = Dict::new(None);
        let pbar = progress_bar(m.len(), "Parsing dictionary");
        pbar.set_draw_delta(m.len() as u64 / 100);
        for (strokes, entry) in m.iter() {
            let mut cur_dict = &mut root;
            for stroke in strokes.split('/').map(|stroke| {
                stroke
                    .parse()
                    .map_err(|_| ParseDictError::InvalidStroke(strokes.clone()))
            }) {
                let stroke = stroke?;
                cur_dict = cur_dict.entry(stroke).or_default();
            }
            cur_dict.set_entry(Entry::parse_entry(entry)?);
            pbar.inc(1);
        }
        pbar.finish_with_message("Dictionary parsed");
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
    assert_eq!(
        Entry::parse_entry("a").unwrap(),
        Entry {
            attr: Attr {
                str_only: true,
                ..Attr::valid_default()
            },
            input: vec![Input::String("a".into())]
        }
    );
}

#[test]
fn test_finger_spell() {
    assert_eq!(
        Entry::parse_entry("{>}{&c}").unwrap(),
        Entry {
            attr: Attr {
                glue: true,
                str_only: true,
                ..Attr::valid_default()
            },
            input: vec![Input::String("c".into())],
        }
    );
}

#[test]
fn test_cap() {
    assert_eq!(
        Entry::parse_entry("{-|}").unwrap(),
        Entry {
            attr: Attr {
                caps: Caps::Caps,
                ..Attr::valid_default()
            },
            input: vec![],
        }
    );
}

#[test]
fn test_cap_star() {
    assert_eq!(
        Entry::parse_entry("{^}{-|}").unwrap(),
        Entry {
            attr: Attr {
                caps: Caps::Caps,
                space_prev: false,
                space_after: false,
                ..Attr::valid_default()
            },
            input: vec![],
        }
    );
}

#[test]
fn test_entry_len() {
    assert_eq!(
        Entry::parse_entry("{^}{#Return}{^}{-|}")
            .unwrap()
            .byte_len(),
        2
    );
}

#[test]
fn test_command() {
    assert_eq!(
        Entry::parse_entry("oh yeah{,}babe").unwrap(),
        Entry {
            attr: Attr {
                str_only: true,
                ..Attr::valid_default()
            },
            input: vec![Input::String("oh yeah, babe".into())],
        }
    );
}
