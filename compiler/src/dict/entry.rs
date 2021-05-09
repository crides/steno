//! Provides value level types and constructs for parsing and representing the JSON dictionary. May contain
//! values which are already byte level (e.g. keycodes) for simplicity
use onig::{Captures, Regex};

use super::parse::parse_entry;
pub use super::parse::{Parsed, ParseError};

lazy_static! {
    /// Stolen from somewhere in Plover. Matches against atoms inside a meta string (either `{}` enclosed
    /// atoms or not enclosed plain strings)
    static ref ESCAPED: Regex = Regex::new(r"\\([\\{}^])").unwrap();
}

#[derive(Debug)]
pub enum ParseEntryError<'i> {
    InvalidKeycode(&'i str),
}

impl<'i> std::fmt::Display for ParseEntryError<'i> {
    fn fmt(&self, f: &mut std::fmt::Formatter) -> std::fmt::Result {
        match self {
            ParseEntryError::InvalidKeycode(k) => write!(f, "Invalid keycode '{}'", k),
        }
    }
}

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
    Capital,
    /// Carry/Keep the case until `length` characters later; if the current character should be upper cased
    /// for example, then don't make the current word upper case, but instead apply upper case to `length`
    /// characters (not bytes; unicode code points) later
    Carry(String),
    /// Reset formatting (case conversion)
    ResetFormat,
    /// Make last stroke (not word) lower case
    LowerLast,
    /// Make last stroke (not word) upper case
    UpperLast,
    /// Make last stroke (not word) Capitalized/Title case
    CapitalLast,
    /// Repeat last stroke
    Repeat,
    /// Toggle asterisk on the last stroke
    ToggleStar,
    /// Add space between last and last last stroke
    RetroSpace,
    /// Remove space between last and last last stroke
    RetroNoSpace,
    /// Dictionary editing
    DictEdit,
}

impl Input {
    /// String length of this input segment
    fn strlen(&self) -> usize {
        match self {
            Input::String(s) => s.len(),
            Input::Carry(s) => s.len(),
            _ => 0,
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
                Input::Carry(s) => s.len() + 2,
                Input::Keycodes(k) => k.len() + 2,
                _ => 1,
            })
            .sum()
    }

    fn replace_escapes(s: &str) -> String {
        ESCAPED.replace_all(s, |c: &Captures| c.at(1).unwrap().to_string())
    }

    /// Parse a single atom in an `Entry`. Can be either `{}` enclosed or not
    fn compile_atom<'i>(a: Parsed<'i>) -> Entry {
        use Parsed::*;
        match a {
            Text(s) => Entry {
                attr: Attr {
                    glue: s.chars().all(|c| c.is_numeric()),
                    ..Attr::valid_default()
                },
                inputs: vec![Input::String(Entry::replace_escapes(s))],
            },
            Keycodes(k) => Entry {
                attr: Attr::valid_default(),
                inputs: vec![Input::Keycodes(
                    k.into_iter().flat_map(|k| k.into_keycodes()).collect(),
                )],
            },
            HalfStop(s) => Entry {
                attr: Attr {
                    space_prev: false,
                    ..Attr::valid_default()
                },
                inputs: vec![Input::String(s.into())],
            },
            FullStop(s) => Entry {
                attr: Attr {
                    space_prev: false,
                    ..Attr::valid_default()
                },
                inputs: vec![Input::String(s.into()), Input::Capital],
            },
            Glue(s) => Entry {
                attr: Attr {
                    glue: true,
                    ..Attr::valid_default()
                },
                inputs: vec![Input::String(Entry::replace_escapes(s))],
            },
            AttachFront(s) => Entry {
                attr: Attr {
                    space_prev: false,
                    ..Attr::valid_default()
                },
                inputs: vec![Input::String(Entry::replace_escapes(s))],
            },
            AttachBack(s) => Entry {
                attr: Attr {
                    space_after: false,
                    ..Attr::valid_default()
                },
                inputs: vec![Input::String(Entry::replace_escapes(s))],
            },
            AttachBoth(s) => Entry {
                attr: Attr {
                    space_prev: false,
                    space_after: false,
                    ..Attr::valid_default()
                },
                inputs: vec![Input::String(Entry::replace_escapes(s))],
            },
            Carry(front, s, back) => Entry {
                attr: Attr {
                    space_prev: !front,
                    space_after: !back,
                    ..Attr::valid_default()
                },
                inputs: vec![Input::Carry(Entry::replace_escapes(s))],
            },
            Capital => Entry {
                attr: Attr::valid_default(),
                inputs: vec![Input::Capital],
            },
            CapitalLast => Entry {
                attr: Attr::valid_default(),
                inputs: vec![Input::CapitalLast],
            },
            Lower => Entry {
                attr: Attr::valid_default(),
                inputs: vec![Input::Lower],
            },
            LowerLast => Entry {
                attr: Attr::valid_default(),
                inputs: vec![Input::LowerLast],
            },
            Upper => Entry {
                attr: Attr::valid_default(),
                inputs: vec![Input::Upper],
            },
            UpperLast => Entry {
                attr: Attr::valid_default(),
                inputs: vec![Input::UpperLast],
            },
            Attach => Entry {
                attr: Attr {
                    space_prev: false,
                    space_after: false,
                    ..Attr::valid_default()
                },
                inputs: vec![],
            },
            ResetFormat => Entry {
                attr: Attr::valid_default(),
                inputs: vec![Input::ResetFormat],
            },
            ToggleStar => Entry {
                attr: Attr::valid_default(),
                inputs: vec![Input::ToggleStar],
            },
            Repeat => Entry {
                attr: Attr::valid_default(),
                inputs: vec![Input::Repeat],
            },
            RetroSpace => Entry {
                attr: Attr::valid_default(),
                inputs: vec![Input::RetroSpace],
            },
            RetroNoSpace => Entry {
                attr: Attr::valid_default(),
                inputs: vec![Input::RetroNoSpace],
            },
            DictEdit => Entry {
                attr: Attr::valid_default(),
                inputs: vec![Input::DictEdit],
            },
            Plover(s) => {
                eprintln!("{{PLOVER:{}}} not supported yet", s);
                Entry {
                    attr: Attr::valid_default(),
                    inputs: vec![Input::String(format!("{{PLOVER:{}}}", s))],
                }
            }
            PloverMode(s) => {
                eprintln!("{{MODE:{}}} not supported yet", s);
                Entry {
                    attr: Attr::valid_default(),
                    inputs: vec![Input::String(format!("{{MODE:{}}}", s))],
                }
            }
        }

        // Attributes for the current entry; i.e. will not appear in returning `Attr`
    }

    /// Parses the whole entry. Calls `parse_atom` and joins the inputs and attributes together, changing the
    /// contents when necessary
    pub fn parse_entry(s: &str) -> Result<Entry, ParseError<&str>> {
        let mut entry = Entry {
            attr: Attr::valid_default(),
            inputs: Vec::new(),
        };

        let atoms = parse_entry(s)?
            .into_iter()
            .map(|a| Entry::compile_atom(a))
            .collect::<Vec<Entry>>();
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
                Input::Capital,
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
            inputs: vec![Input::Capital],
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
            inputs: vec![Input::Capital],
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

#[test]
fn test_braces() {
    assert_eq!(
        Entry::parse_entry(r"{^}\}{^}").unwrap(),
        Entry {
            attr: Attr {
                space_after: false,
                space_prev: false,
                ..Attr::valid_default()
            },
            inputs: vec![Input::String("}".into())],
        }
    );
    assert_eq!(
        Entry::parse_entry(r"\\\{\}").unwrap(),
        Entry {
            attr: Attr::valid_default(),
            inputs: vec![Input::String("\\{}".into())],
        }
    );
}
