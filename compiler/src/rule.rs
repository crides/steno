//! For applying defined orthographic rules to the dictionary by appending suffixes from the dictionary to the
//! other words/phrases, and keeping those which appears in a word/phrase list. This is done so that the
//! keyboard doesn't need to handle the complicated orthographic rules at runtime.
use std::collections::HashMap;

use onig::{Regex, Captures};

use crate::bar::progress_bar;

pub type Dict = HashMap<String, String>;

lazy_static! {
    static ref ENTRY_SUFFIX: Regex = Regex::new(r"\{\^([[:alpha:]]*)\}").unwrap();
}

#[derive(Debug, Deserialize)]
struct Rule {
    word: String,
    suffix: String,
    repl: String,
}

#[derive(Deserialize)]
pub struct Rules {
    regex: Vec<Rule>,
    simple: Vec<Rule>,
    word_list: HashMap<String, u8>,
}

/// Replace using back references in `repl`
fn replace(re: &Regex, text: &str, repl: &str) -> String {
    let back_ref = Regex::new(r"\\(\d)").unwrap();
    re.replace(text, |caps: &Captures| {
        let mut poss = vec![0];
        for (start, end) in back_ref.find_iter(repl) {
            poss.extend(&[start, start, end, end]);
        }
        poss.push(repl.len());
        let mut pos = poss.into_iter();
        let mut output = String::new();
        let (first_start, first_end) = (pos.next().unwrap(), pos.next().unwrap());
        output.push_str(&repl[first_start..first_end]);
        while let (Some(group_start), Some(group_end), Some(text_start), Some(text_end)) = (pos.next(), pos.next(), pos.next(), pos.next()) {
            let group = repl[(group_start + 1)..group_end].parse().unwrap();
            output.push_str(caps.at(group).unwrap());
            output.push_str(&repl[text_start..text_end]);
        }
        output
    })
}

pub fn apply_rules(rules: &Rules, source: &Dict) -> Dict {
    let pbar = progress_bar(source.len(), "Extracting suffixes");
    pbar.set_draw_delta(source.len() as u64 / 100);
    let suffixes: Dict = source
        .iter()
        .filter_map(|(stroke, entry)| {
            pbar.inc(1);
            if ENTRY_SUFFIX.is_match(entry) {
                Some((
                    stroke.clone(),
                    ENTRY_SUFFIX.replace(entry, |caps: &Captures| caps.at(1).unwrap().to_string()),
                ))
            } else {
                None
            }
        })
        .collect();
    pbar.finish_with_message("Done extracting suffixes");

    let mut cached_res = HashMap::new();
    let mut output: Dict = HashMap::new();
    let pbar = progress_bar(source.len(), "Applying rules");
    pbar.set_draw_delta(source.len() as u64 / 200);
    for (stroke, entry) in source.iter() {
        output.insert(stroke.clone(), entry.clone());
        for rule in rules.simple.iter() {
            if &rule.word == entry {
                let new_stroke = suffixes.iter().find_map(|(k, v)| {
                    if v == &rule.suffix {
                        Some(k.clone())
                    } else {
                        None
                    }
                });
                if let Some(new_stroke) = new_stroke {
                    output.insert(format!("{}/{}", stroke, new_stroke), rule.repl.clone());
                }
            }
        }
        for rule in rules.regex.iter() {
            if !cached_res.contains_key(&rule.word) {
                cached_res.insert(rule.word.clone(), Regex::new(&rule.word).unwrap());
            }
            let word_re = cached_res.get(&rule.word).unwrap();
            if word_re.find(&entry).is_some() {
                let pat = format!(
                    "{}\u{ffff}{}",
                    &rule.word[..(rule.word.len() - 1)],
                    rule.suffix
                );
                if !cached_res.contains_key(&pat) {
                    cached_res.insert(pat.clone(), Regex::new(&pat).unwrap());
                }

                for (new_stroke, suffix) in suffixes.iter() {
                    let simple_add = format!("{}\u{ffff}{}", entry, suffix);
                    let pat_re = cached_res.get(&pat).unwrap();
                    if pat_re.find(&simple_add).is_some() {
                        let word = replace(pat_re, &simple_add[..], &rule.repl[..]);
                        if rules.word_list.contains_key(&word) && &word != entry {
                            output.insert(format!("{}/{}", stroke, new_stroke), word.into());
                        }
                    }
                }
            }
        }
        pbar.inc(1);
    }
    pbar.finish_with_message("Done applying rules");
    output
}
