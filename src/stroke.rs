use std::fmt::{self, Display, Formatter, Write};
use std::str::FromStr;

pub static KEYS: &'static str = "#STKPWHRAO*EUFRPBLGTSDZ";

#[derive(Debug, PartialEq, Eq, Hash, PartialOrd, Ord)]
pub struct Stroke(u32);

impl Stroke {
    pub fn new() -> Self {
        Self(0)
    }

    pub fn set(&mut self, bit: usize) {
        self.0 |= 1 << bit;
    }

    pub fn is_set(&self, bit: usize) -> bool {
        self.0 & (1 << bit) != 0
    }

    pub fn raw(self) -> u32 {
        self.0
    }
}

#[derive(Debug)]
pub struct StrokeParseError(String, char);

impl Display for StrokeParseError {
    fn fmt(&self, f: &mut Formatter) -> fmt::Result {
        write!(f, "Unknown char '{}' in string '{}'", self.1, self.0)
    }
}

impl FromStr for Stroke {
    type Err = StrokeParseError;
    fn from_str(s: &str) -> Result<Self, Self::Err> {
        let s = s
            .replace("1", "#S")
            .replace("2", "#T")
            .replace("3", "#P")
            .replace("4", "#H")
            .replace("5", "#A")
            .replace("6", "#F")
            .replace("7", "#P")
            .replace("8", "#L")
            .replace("9", "#T")
            .replace("0", "#O");
        let mut res = Stroke::new();
        let mut ind = 0;
        for c in s.chars() {
            match c {
                '-' => ind = KEYS.find('*').unwrap(),
                '#' => res.set(KEYS.len() - 1),
                _ => {
                    ind += KEYS[ind..].find(c).ok_or(StrokeParseError(s.clone(), c))?;
                    res.set(KEYS.len() - ind - 1);
                }
            }
        }
        Ok(res)
    }
}

impl Display for Stroke {
    fn fmt(&self, f: &mut Formatter) -> fmt::Result {
        for (i, c) in KEYS.chars().enumerate() {
            f.write_char(if self.is_set(KEYS.len() - i - 1) {
                c
            } else {
                '-'
            })?;
        }
        Ok(())
    }
}
