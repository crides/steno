use std::fmt::{self, Debug, Display, Formatter};
use std::str::FromStr;

pub static KEYS: &str = "#STKPWHRAO*EUFRPBLGTSDZ";

#[derive(PartialEq, Eq, Hash, PartialOrd, Ord, Clone, Copy)]
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

impl Display for Stroke {
    fn fmt(&self, f: &mut Formatter) -> fmt::Result {
        const DIGIT_BITS: u32 = 0x7562A8;
        static KEYS: &str = "#STKPWHRAO*EUFRPBLGTSDZ";
        let mut buf = String::new();
        if self.is_set(22) && (self.0 & !DIGIT_BITS) != 0 {
            for &(bit, digit) in &[
                (21, '1'),
                (20, '2'),
                (18, '3'),
                (16, '4'),
                (14, '5'),
                (13, '0'),
                (9, '6'),
                (7, '7'),
                (5, '8'),
                (3, '9'),
            ] {
                if self.is_set(bit) {
                    buf.push(digit);
                }
            }
        } else {
            let mut has_mid = false;
            for i in (0..23).rev() {
                if (self.0 & (1 << i)) > 0 {
                    if i < 10 {
                        if !has_mid {
                            buf.push('-');
                            has_mid = true;
                        }
                    } else if i <= 14 {
                        has_mid = true;
                    }
                    buf.push(KEYS.chars().nth(22 - i).unwrap());
                }
            }
        }
        write!(f, "{}", buf)
    }
}

impl Debug for Stroke {
    fn fmt(&self, f: &mut Formatter) -> fmt::Result {
        f.debug_tuple("Stroke")
            .field(&format_args!("{:x}", &self.0))
            .field(&format_args!("{}", &self))
            .finish()
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
                    ind += KEYS[ind..]
                        .find(c)
                        .ok_or_else(|| StrokeParseError(s.clone(), c))?;
                    res.set(KEYS.len() - ind - 1);
                }
            }
        }
        Ok(res)
    }
}
