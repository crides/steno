mod entry;
mod keycode;
pub mod parse;

use std::collections::BTreeMap;
use std::fmt::Display;

use crate::bar::progress_bar;
use crate::stroke::Strokes;

pub use entry::{Attr, Entry, Input};

pub type JsonDict = BTreeMap<String, String>;

#[derive(Debug)]
pub enum ParseDictError {
    InvalidStroke(String, String),
}

impl Display for ParseDictError {
    fn fmt(&self, f: &mut std::fmt::Formatter) -> std::fmt::Result {
        use ParseDictError::*;
        match self {
            InvalidStroke(s, ss) => write!(f, "Invalid stroke '{}' in '{}'", s, ss),
        }
    }
}

/// Parsed value representation for a JSON dictionary. Differs from the JSON dictionary only in that the
/// values here are parsed.
#[derive(Debug, Clone)]
pub struct Dict(pub BTreeMap<Strokes, Entry>);

enum Multiple<'a> {
    Single((&'a str, String)),
    Multiple(Vec<(&'a str, String)>),
}

impl<'a> Multiple<'a> {
    fn push(&mut self, other: (&'a str, String)) {
        match self {
            Multiple::Single(val) => {
                *self = Multiple::Multiple(vec![val.clone(), other]);
            }
            Multiple::Multiple(ref mut v) => v.push(other),
        }
    }
}

impl Dict {
    pub fn merge_dicts(mut dicts: Vec<(&str, JsonDict)>) -> BTreeMap<String, String> {
        if dicts.len() == 1 {
            dicts.remove(0).1
        } else {
            let (first_name, first) = dicts.remove(0);
            let first = first
                .into_iter()
                .map(|(k, v)| (k, Multiple::Single((first_name, v))))
                .collect::<BTreeMap<_, _>>();
            let merged = dicts
                .into_iter()
                .fold(first, |mut merged, (new_name, new)| {
                    for (key, val) in new.into_iter() {
                        if let Some(ref mut v) = merged.get_mut(&key) {
                            v.push((new_name, val));
                        } else {
                            merged.insert(key, Multiple::Single((new_name, val)));
                        }
                    }
                    merged
                });
            let mut has_conflict = false;
            let merged = merged
                .into_iter()
                .map(|(k, m)| match m {
                    Multiple::Single((_name, val)) => (k, val),
                    Multiple::Multiple(mut v) => {
                        has_conflict = true;
                        println!("Conflict on key '{}'", k);
                        for (file, val) in &v[..(v.len() - 1)] {
                            println!("    in file {}, mapped to '{}'", file, val);
                        }
                        let last = v.pop().unwrap();
                        println!("  using '{}' from {}", last.1, last.0);
                        (k, last.1)
                    }
                })
                .collect();
            if !has_conflict {
                println!("Merged without conflicts");
            }
            merged
        }
    }

    pub fn parse(merged: BTreeMap<String, String>) -> Result<Dict, ParseDictError> {
        let total_len = merged.len();
        let pbar = progress_bar(total_len, "Parsing dictionary");
        pbar.set_draw_delta(total_len as u64 / 100);

        let parsed = merged
            .into_iter()
            .filter_map(|(strokes, entry)| {
                let res = match strokes
                    .split('/')
                    .map(|stroke| {
                        stroke.parse().map_err(|_| {
                            ParseDictError::InvalidStroke(stroke.to_string(), strokes.clone())
                        })
                    })
                    .collect::<Result<Vec<_>, _>>()
                {
                    Ok(parsed_strokes) => match Entry::parse_entry(&entry) {
                        Ok(e) => Some(Ok((Strokes(parsed_strokes), e))),
                        Err(e) => {
                            pbar.println(format!(
                                "Ignoring malformed entry '{}' mapped from '{}': {}",
                                entry,
                                strokes,
                                e.format_error(&entry)
                            ));
                            None
                        }
                    },
                    Err(e) => Some(Err(e)),
                };
                pbar.inc(1);
                res
            })
            .collect::<Result<BTreeMap<_, _>, ParseDictError>>()?;
        pbar.finish_with_message("Dictionary parsed");
        Ok(Dict(parsed))
    }
}
