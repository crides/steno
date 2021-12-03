mod types;

use std::io::Write;
use types::*;

const RULES: &'static str = include_str!("../english.regex.fix.json");

macro_rules! wln {
    ($($tt:tt)*) => { writeln!($($tt)*).unwrap() }
}

fn gen_from_hir(w: &mut dyn Write, hir: &regex_syntax::hir::Hir) {
    use regex_syntax::hir::*;
    match hir.kind() {
        HirKind::Empty => {
        }
        HirKind::Literal(l) => {
            match l {
                Literal::Unicode(c) => {
                    wln!(w, "if (s[i] == {}) {{ i ++; }} else {{ error = true; break; }}", *c as usize);
                }
                Literal::Byte(b) => {
                    wln!(w, "if (s[i] == {}) {{ i ++; }} else {{ error = true; break; }}", b);
                }
            }
        }
        HirKind::Class(c) => {
            match c {
                Class::Unicode(_) => (),
                Class::Bytes(bs) => {
                    wln!(w, "do {{");
                    for cls in bs.iter() {
                        if cls.start() == cls.end() {
                            wln!(w, "if (s[i] == {}) {{ i ++; continue; }}", cls.start());
                        } else {
                            wln!(w, "if ({} <= s[i] && s[i] <= {}) {{ i ++; continue; }}", cls.start(), cls.end());
                        }
                    }
                    wln!(w, "error = true; }} while (0);");
                }
            }
        }
        HirKind::Anchor(a) => {
            match a {
                Anchor::StartLine | Anchor::StartText => {
                    wln!(w, "if (i != 0) {{ error = true; break; }}");
                }
                Anchor::EndLine | Anchor::EndText => {
                    wln!(w, "if (i != s_len) {{ error = true; break; }}");
                }
            }
        }
        HirKind::WordBoundary(_) => unimplemented!(),
        HirKind::Repetition(r) => {
            match r.kind {
                RepetitionKind::ZeroOrOne => {
                    wln!(w, "{{");
                    gen_from_hir(w, &r.hir);
                    wln!(w, "error = false; }}");
                }
                RepetitionKind::ZeroOrMore => {
                    wln!(w, "while (true) {{");
                    gen_from_hir(w, &r.hir);
                    wln!(w, "error = false; }}");
                }
                RepetitionKind::OneOrMore => {
                    gen_from_hir(w, &r.hir);
                    wln!(w, "while (true) {{");
                    gen_from_hir(w, &r.hir);
                    wln!(w, "error = false; }}");
                }
                RepetitionKind::Range(_) => unimplemented!(),
            }
        }
        HirKind::Group(g) => {
            match g.kind {
                GroupKind::NonCapturing => {
                    gen_from_hir(w, &g.hir);
                }
                GroupKind::CaptureName { .. } => unimplemented!(),
                GroupKind::CaptureIndex(ind) => {
                    wln!(w, "groups[{}].start = i;", ind);
                    gen_from_hir(w, &g.hir);
                    wln!(w, "groups[{}].end = i;", ind);
                }
            }
        }
        HirKind::Concat(hirs) => {
            for hir in hirs {
                gen_from_hir(w, hir);
                // wln!(w, "if (error) {{ break; }}");
            }
        }
        HirKind::Alternation(hirs) => {
            for hir in hirs {
                wln!(w, "error = false; i_save = i; do {{");
                gen_from_hir(w, hir);
                wln!(w, "}} while (0); if (!error) {{ break; }} i = i_save;");
            }
        }
    }
}

fn rule_gen(w: &mut dyn Write, rs: &[(&str, &str)]) {
    wln!(w, r"#include <string.h>
#include <stdint.h>
#include <stdbool.h>
typedef struct {{
    uint8_t start, end;
}} group_t;
uint8_t ortho_rules(char *inp, char *output) {{
const uint8_t inp_len = strlen(inp); bool error = false; group_t groups[10] = {{0}};");
    for (re, repl) in rs {
        let mut builder = regex_syntax::ParserBuilder::new();
        let mut parser = builder.unicode(false).allow_invalid_utf8(true).build();
        let hir = parser.parse(re).unwrap();
        wln!(w, "
        // {}
        error = false;
        for (uint8_t j = 0; j < inp_len; j ++) {{
            char *s = &inp[j]; uint8_t i = 0, i_save = 0, s_len = inp_len - j;", re);
        wln!(w, "while (true) {{");
        gen_from_hir(w, &hir);
        wln!(w, "}}
        if (!error) {{");
        repl_gen(w, repl);
        wln!(w, "return j; }} }}");
    }
    wln!(w, "return -1; }}");
}

use onig::Regex;
lazy_static::lazy_static! {
    pub static ref REPL_ATOM: Regex = Regex::new(r"\$(\d)|\w+").unwrap();
}

fn repl_gen(w: &mut dyn Write, rule: &str) {
    for capture in REPL_ATOM.captures_iter(rule) {
        if let Some(g) = capture.at(1) {
            wln!(w, "strncat(output, s, groups[{0}].end - groups[{0}].start);", g.parse::<u8>().unwrap());
        } else {
            wln!(w, "strcat(output, \"{}\");", capture.at(0).unwrap());
        }
    }
}

fn main() {
    let raw_regex_rules: Vec<Rule> = serde_json::from_str(RULES).unwrap();
    let mut out_file = std::fs::File::create("rules.c").unwrap();
    let full_regexes: Vec<_> = raw_regex_rules.iter().map(|r| (format!("{} {}", r.word, r.suffix), r.repl.clone())).collect();
    rule_gen(&mut out_file, &full_regexes.iter().map(|(f, s)| (f.as_str(), s.as_str())).collect::<Vec<_>>());
}
