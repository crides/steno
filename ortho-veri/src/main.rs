mod types;
mod bindings;
mod hw_mock;

use std::ffi::CString;
use std::fs::File;
use std::collections::BTreeSet;

use clap::{App, Arg, ArgGroup};
use onig::{Captures, Regex};

use types::*;

/// Replace using back references in `repl`
fn replace(re: &Regex, text: &str, repl: &str) -> String {
    let back_ref = Regex::new(r"\$(\d)").unwrap();
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
        while let (Some(group_start), Some(group_end), Some(text_start), Some(text_end)) =
            (pos.next(), pos.next(), pos.next(), pos.next())
        {
            let group = repl[(group_start + 1)..group_end].parse().unwrap();
            output.push_str(caps.at(group).unwrap());
            output.push_str(&repl[text_start..text_end]);
        }
        output
    })
}

fn apply_rules(simple_rules: &SimpleRules, regex_rules: &[RegexRule], word: &str, suffix: &str) -> RuleResult {
    if let Some(repl) = simple_rules.get(&(word.into(), suffix.into())) {       // FIXME
        return Some((repl.to_owned(), 0));
    }
    for (i, rule) in regex_rules.iter().enumerate() {
        if rule.word_re.find(word).is_some() {
            let simple_add = format!("{}\u{fffd}{}", word, suffix);
            if rule.full_re.find(&simple_add).is_some() {
                return Some((replace(&rule.full_re, &simple_add, &rule.repl), i as u8));
            }
        }
    }
    None
}

fn apply_c_rules(word: &str, suffix: &str, chop: bool) -> RuleResult {
    let word_len = word.len();
    let word_start = if word_len > 7 && chop { word_len - 7 } else {0};
    let c_word = CString::new(&word[word_start..word_len]).unwrap();
    let c_suffix = CString::new(suffix).unwrap();
    let mut c_out = vec![0i8; 16];
    let c_out_ptr = c_out.as_mut_ptr();
    let mut clause = vec![0u8];
    let c_clause = clause.as_mut_ptr();
    std::mem::forget(c_out);
    let ret: i8 = unsafe { bindings::process_ortho(c_word.as_ptr(), c_suffix.as_ptr(), c_out_ptr, c_clause) };
    if ret < 0 {
        None
    } else {
        let c_out = unsafe { CString::from_raw(c_out_ptr) };
        Some((format!(
            "{}{}",
            &word[..(word.len() - ret as usize)],
            c_out.to_string_lossy()
        ), clause[0]))
    }
}

fn progress_bar(len: usize, msg: &'static str) -> indicatif::ProgressBar {
    let pbar = indicatif::ProgressBar::new(len as u64).with_style(
        indicatif::ProgressStyle::default_bar()
            .progress_chars("=> ")
            .template("{msg} [{wide_bar}] ETA: {eta} ({pos}/{len})"),
    );
    pbar.set_message(msg);
    pbar
}

type RuleResult = Option<(String, u8)>;

fn diff_result(expected: RuleResult, got: RuleResult, word: &str, suffix: &str, extra: bool) -> Option<String> {
    match (expected, got) {
        (Some((exp, e)), Some((got, g))) if exp != got => {
            Some(format!(
                "Mismatch: suffix: {:?}, word: {:?}, expected: {:?} ({}), got: {:?} ({})",
                suffix,
                word,
                exp,
                e,
                got,
                g,
            ))
        }
        (Some((exp, e)), None) => Some(format!(
            "No result: suffix: {:?}, word: {:?}, expected: {:?} ({})",
            suffix, word, exp, e,
        )),
        (None, Some((got, g))) if extra => Some(format!(
            "Extra result: suffix: {:?}, word: {:?}, got: {:?} ({})",
            suffix, word, got, g
        )),
        _ => None,
    }
}

macro_rules! root_file {
    ($f:literal) => { concat!(concat!(env!("CARGO_MANIFEST_DIR"), "/"), $f) }
}

fn main() {
    let matches = App::new("ortho-veri")
        .arg(Arg::with_name("simple-rule-file"))
        .arg(Arg::with_name("regex-rule-file"))
        .arg(Arg::with_name("flash-file"))
        .arg(Arg::with_name("dict-file"))
        .arg(Arg::with_name("chop").long("chop"))
        .arg(Arg::with_name("extra").long("extra"))
        .arg(Arg::with_name("verify").long("verify"))
        .arg(Arg::with_name("test").long("test").number_of_values(2))
        .group(
            ArgGroup::with_name("cmd")
                .args(&["test", "verify"])
                .required(true),
        )
        .get_matches();
    let (simple_rule_file, regex_rule_file, dict_file, flash_file) = (
        matches.value_of("simple-rule-file").unwrap_or(root_file!("english.simple.json")),
        matches.value_of("regex-rule-file").unwrap_or(root_file!("english.regex.json")),
        matches.value_of("dict-file").unwrap_or(root_file!("../dict.json")),
        matches.value_of("flash-file").unwrap_or(root_file!("../steno.bin")),
    );
    let raw_simple_rules: Vec<Rule> = serde_json::from_reader(File::open(simple_rule_file).unwrap()).unwrap();
    let simple_rules: SimpleRules = raw_simple_rules.into_iter().map(|r| ((r.word, r.suffix), r.repl)).collect();
    let raw_regex_rules: Vec<Rule> = serde_json::from_reader(File::open(regex_rule_file).unwrap()).unwrap();
    let regex_rules = raw_regex_rules
        .into_iter()
        .map(|rule| RegexRule {
            word_re: Regex::new(&rule.word).unwrap(),
            full_re: Regex::new(&format!("{}\u{fffd}{}", rule.word, rule.suffix)).unwrap(),
            word: rule.word,
            suffix: rule.suffix,
            repl: rule.repl,
        })
        .collect::<Vec<_>>();
    let flash = std::fs::read(flash_file).unwrap();
    *hw_mock::STORAGE.write().unwrap() = Some(flash);

    if matches.is_present("verify") {
        let dict: Dict = serde_json::from_reader(File::open(dict_file).unwrap()).unwrap();
        let words = dict.values().collect::<BTreeSet<&String>>();
        let entry_suffix = Regex::new(r"\{\^([[:alpha:]]+)\}").unwrap();

        let suffixes = dict
            .values()
            .filter_map(|entry| {
                if entry_suffix.is_match(entry) {
                    Some(
                        entry_suffix
                            .replace(entry, |caps: &Captures| caps.at(1).unwrap().to_string()),
                    )
                } else {
                    None
                }
            })
            .collect::<BTreeSet<_>>();
        let suf_bar = progress_bar(suffixes.len(), "suffixes");
        for suffix in suffixes {
            for word in &words {
                let res = apply_rules(&simple_rules, &regex_rules, word, &suffix);
                let c_res = apply_c_rules(word, &suffix, matches.is_present("chop"));
                if let Some(msg) = diff_result(res, c_res, word, &suffix, matches.is_present("extra")) {
                    suf_bar.println(msg);
                }
            }
            suf_bar.inc(1);
        }
    } else {
        let mut values = matches.values_of("test").unwrap();
        let (word, suffix) = (values.next().unwrap(), values.next().unwrap());
        let res = apply_rules(&simple_rules, &regex_rules, word, &suffix);
        let c_res = apply_c_rules(&word, &suffix, matches.is_present("chop"));
        match diff_result(res, c_res, &word, &suffix, matches.is_present("extra")) {
            Some(msg) => {
                eprintln!("{}", msg);
            }
            None => {
                eprintln!("Same");
            }
        }
    }
}
