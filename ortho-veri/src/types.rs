use std::collections::BTreeMap;
use serde_derive::Deserialize;
use onig::Regex;

pub type Dict = BTreeMap<String, String>;
pub type SimpleRules = BTreeMap<(String, String), String>;

#[derive(Debug, Deserialize)]
pub struct Rule {
    #[serde(rename(deserialize = "l"))]
    pub word: String,
    #[serde(rename(deserialize = "r"))]
    pub suffix: String,
    #[serde(rename(deserialize = "s"))]
    pub repl: String,
}

#[derive(Debug)]
pub struct RegexRule {
    pub word: String,
    pub suffix: String,
    pub repl: String,
    pub word_re: Regex,
    pub full_re: Regex,
}
