//! Provides value level types and constructs for parsing and representing the JSON dictionary. May contain
//! values which are already byte level (e.g. keycodes) for simplicity
use std::collections::BTreeMap;
use std::fmt::Display;

use lalrpop_util::ParseError;
use onig::Regex;

use crate::bar::progress_bar;
use crate::keycode::TermListParser;
use crate::stroke::Stroke;

lazy_static! {
    /// Stolen from somewhere in Plover. Matches against atoms inside a meta string (either `{}` enclosed
    /// atoms or not enclosed plain strings)
    static ref META_RE: Regex = Regex::new(r"[^{}]+|\{[^{}]*\}").unwrap();
}

pub type JsonDict = BTreeMap<String, String>;

#[derive(Clone, Debug, PartialEq, Eq, Hash)]
pub enum Input {
    /// Raw HID keycodes, with `0xE0` to `0xE3` toggling Ctrl, Shift, Alt, Super (GUI/Windows key) respectively
    Keycodes(Vec<u8>),
    /// Strings, which can contain Unicode characters
    String(String),
    /// Make the next word lower case
    Lower,
    /// Make the next word UPPER CASE
    Upper,
    /// Make the next word Capitalized/Title case
    Capitalized,
    /// Carry/Keep the case until `length` characters later; if the current character should be upper cased
    /// for example, then don't make the current word upper case, but instead apply upper case to `length`
    /// characters (not bytes; unicode code points) later
    Keep(String),
    /// Reset formatting (case conversion)
    ResetFormat,
    /// Make last stroke (not word) lower case
    LowerLast,
    /// Make last stroke (not word) upper case
    UpperLast,
    /// Make last stroke (not word) Capitalized/Title case
    CapitalizedLast,
    /// Repeat last stroke
    Repeat,
    /// Toggle asterisk on the last stroke
    ToggleStar,
    /// Add space between last and last last stroke
    AddSpace,
    /// Remove space between last and last last stroke
    RemoveSpace,
    /// Add a new translation
    AddTranslation,
    /// Edit a new translation
    EditTranslation,
    /// Remove a new translation
    RemoveTranslation,
}

impl Input {
    /// String length of this input segment
    fn strlen(&self) -> usize {
        match self {
            Input::String(s) => s.len(),
            Input::Keep(s) => s.len(),
            _ => 0,
        }
    }

    /// Total length in bytes for the entry. Used to cheaply determine the size of a certain entry
    pub fn byte_len(&self) -> usize {
        match self {
            Input::String(s) => s.len(),
            Input::Keep(s) => s.len() + 2,
            Input::Keycodes(k) => k.len() + 2,
            _ => 1,
        }
    }
}

/// Simplified attributes for formatting atoms or entries for easy MCU consumption. Also includes other
/// properties about the entry itself
#[derive(Clone, Copy, Debug, PartialEq, Eq, Hash)]
pub struct Attr {
    pub space_prev: bool,
    pub space_after: bool,
    pub glue: bool,
    /// Whether this entry has an output or not. Note that 0-length entries can be outputs e.g. ones that
    /// contain only attribute changes but no printable content.
    pub present: bool,
}

impl Attr {
    fn none() -> Self {
        Attr {
            space_prev: false,
            space_after: false,
            glue: false,
            present: false,
        }
    }

    fn valid_default() -> Self {
        Attr {
            space_prev: true,
            space_after: true,
            glue: false,
            present: true,
        }
    }
}

/// Entry value associated to a key in the dictionary
#[derive(Clone, Debug, PartialEq, Eq, Hash)]
pub struct Entry {
    pub attr: Attr,
    pub inputs: Vec<Input>,
}

impl Default for Entry {
    fn default() -> Entry {
        Entry {
            attr: Attr::none(),
            inputs: Vec::new(),
        }
    }
}

impl Entry {
    /// Total length in bytes for the entry. Used to cheaply determine the size of a certain entry
    pub fn byte_len(&self) -> usize {
        self.inputs
            .iter()
            .map(|i| match i {
                Input::String(s) => s.len(),
                Input::Keep(s) => s.len() + 2,
                Input::Keycodes(k) => k.len() + 2,
                _ => 1,
            })
            .sum()
    }

    /// Parse a single atom in an `Entry`. Can be either `{}` enclosed or not
    fn parse_atom(mut s: &str) -> Result<Entry, ParseDictError> {
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
                Ok(Entry {
                    attr,
                    inputs: vec![Input::Keycodes(keycodes)],
                })
            } else {
                let inputs = match s {
                    "?" | "!" | "." => {
                        attr.space_prev = false;
                        vec![Input::String(s.into()), Input::Capitalized]
                    }
                    "," | ";" | ":" => {
                        attr.space_prev = false;
                        vec![Input::String(s.into())]
                    }
                    "BAT_STENO:ADD_TRANSLATION" => vec![Input::AddTranslation],
                    "BAT_STENO:EDIT_TRANSLATION" => vec![Input::EditTranslation],
                    "BAT_STENO:REMOVE_TRANSLATION" => vec![Input::RemoveTranslation],
                    "-|" => vec![Input::Capitalized],
                    ">" => vec![Input::Lower],
                    "<" => vec![Input::Upper],
                    "^" => {
                        attr.space_prev = false;
                        attr.space_after = false;
                        vec![]
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
                            vec![Input::Keep(s[2..].into())]
                        } else if !s.is_empty() {
                            vec![Input::String(s.into())]
                        } else {
                            vec![]
                        }
                    }
                };
                Ok(Entry { attr, inputs })
            }
        } else {
            if s.chars().all(|c| c.is_numeric()) {
                attr.glue = true;
            }
            Ok(Entry {
                attr,
                inputs: vec![Input::String(s.into())],
            })
        }
    }

    /// Parses the whole entry. Calls `parse_atom` and joins the inputs and attributes together, changing the
    /// contents when necessary
    pub fn parse_entry(s: &str) -> Result<Entry, ParseDictError> {
        let mut entry = Entry {
            attr: Attr::valid_default(),
            inputs: Vec::new(),
        };

        let atoms = META_RE
            .find_iter(s)
            .map(|(b, e)| Entry::parse_atom(&s[b..e]))
            .collect::<Result<Vec<Entry>, ParseDictError>>()?;
        if atoms.is_empty() {
            Ok(entry)
        } else if atoms.len() == 1 {
            Ok(atoms[0].clone())
        } else {
            entry.attr.glue = atoms.iter().any(|e| e.attr.glue);
            entry.attr.space_prev = atoms[0].attr.space_prev;
            // Find the last non-zero length entry or attribute only entry and get the attribute; there can be
            // zero-width commands after wards, which always don't affect spacing. We want to use attributes
            // from either a string entry (which has non-zero length) or a entry with no inputs (attribute only)
            entry.attr.space_after = atoms
                .iter()
                .rev()
                .find(|a| {
                    a.inputs.is_empty() || a.inputs.iter().map(|i| i.strlen()).sum::<usize>() > 0
                })
                .map(|a| a.attr.space_after)
                .unwrap_or(true);
            let spaces: Vec<bool> = atoms
                .windows(2)
                .map(|pair| {
                    let (fst, snd) = (pair[0].attr, pair[1].attr);
                    fst.space_after && snd.space_prev && !(fst.glue && snd.glue)
                })
                .collect();
            assert_eq!(spaces.len() + 1, atoms.len());
            let mut inputs = atoms[0].inputs.clone();
            for i in 0..spaces.len() {
                if spaces[i]
                    && atoms[i + 1]
                        .inputs
                        .iter()
                        .map(|i| i.strlen())
                        .sum::<usize>()
                        > 0
                    && atoms[i].inputs.iter().map(|i| i.strlen()).sum::<usize>() > 0
                {
                    inputs.push(Input::String(" ".into()));
                }
                inputs.extend(atoms[i + 1].inputs.clone());
            }
            for mut input in inputs.into_iter() {
                match (entry.inputs.last_mut(), &mut input) {
                    (Some(Input::String(a)), Input::String(ref mut b)) => {
                        a.push_str(b);
                    }
                    (Some(Input::Keycodes(a)), Input::Keycodes(ref mut b)) => {
                        a.append(b);
                    }
                    _ => {
                        entry.inputs.push(input);
                    }
                }
            }
            Ok(entry)
        }
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

/// Parsed value representation for a JSON dictionary. Differs from the JSON dictionary only in that the
/// values here are parsed.
#[derive(Debug, Clone)]
pub struct Dict(pub BTreeMap<Vec<Stroke>, Entry>);

impl Dict {
    pub fn parse_from_json(m: &JsonDict) -> Result<Dict, ParseDictError> {
        let pbar = progress_bar(m.len(), "Parsing dictionary");
        pbar.set_draw_delta(m.len() as u64 / 100);
        let dict = m
            .iter()
            .map(|(strokes, entry)| {
                let strokes = strokes
                    .split('/')
                    .map(|stroke| {
                        stroke
                            .parse()
                            .map_err(|_| ParseDictError::InvalidStroke(strokes.clone()))
                    })
                    .collect::<Result<Vec<_>, _>>()?;
                let entry = Entry::parse_entry(entry)?;
                pbar.inc(1);
                Ok((strokes, entry))
            })
            .collect::<Result<BTreeMap<_, _>, ParseDictError>>()?;
        pbar.finish_with_message("Dictionary parsed");
        Ok(Dict(dict))
    }
}

#[test]
fn test_plain() {
    assert_eq!(
        Entry::parse_entry("a").unwrap(),
        Entry {
            attr: Attr::valid_default(),
            inputs: vec![Input::String("a".into())]
        }
    );
}

#[test]
fn test_punctuation() {
    let entry = Entry::parse_entry("{.}is").unwrap();
    assert_eq!(
        entry,
        Entry {
            attr: Attr {
                space_prev: false,
                ..Attr::valid_default()
            },
            inputs: vec![
                Input::String(".".into()),
                Input::Capitalized,
                Input::String(" is".into())
            ],
        }
    );
}

#[test]
fn test_finger_spell() {
    let entry = Entry::parse_entry("{&P}").unwrap();
    assert_eq!(
        entry,
        Entry {
            attr: Attr {
                glue: true,
                ..Attr::valid_default()
            },
            inputs: vec![Input::String("P".into())],
        }
    );
}

#[test]
fn test_finger_spell_lower() {
    let entry = Entry::parse_entry("{>}{&c}").unwrap();
    assert_eq!(
        entry,
        Entry {
            attr: Attr {
                glue: true,
                ..Attr::valid_default()
            },
            inputs: vec![Input::Lower, Input::String("c".into())],
        }
    );
}

#[test]
fn test_cap() {
    assert_eq!(
        Entry::parse_entry("{-|}").unwrap(),
        Entry {
            attr: Attr::valid_default(),
            inputs: vec![Input::Capitalized],
        }
    );
}

#[test]
fn test_cap_star() {
    assert_eq!(
        Entry::parse_entry("{^}{-|}").unwrap(),
        Entry {
            attr: Attr {
                space_prev: false,
                space_after: false,
                ..Attr::valid_default()
            },
            inputs: vec![Input::Capitalized],
        }
    );
}

#[test]
fn test_entry_len() {
    assert_eq!(
        Entry::parse_entry("{^}{#Return}{^}{-|}")
            .unwrap()
            .byte_len(),
        4
    );
}

#[test]
fn test_number_only() {
    assert_eq!(
        Entry::parse_entry("05").unwrap(),
        Entry {
            attr: Attr {
                glue: true,
                ..Attr::valid_default()
            },
            inputs: vec![Input::String("05".into())],
        }
    );
}

#[test]
fn test_command() {
    assert_eq!(
        Entry::parse_entry("oh yeah{,}babe").unwrap(),
        Entry {
            attr: Attr::valid_default(),
            inputs: vec![Input::String("oh yeah, babe".into())],
        }
    );
}
