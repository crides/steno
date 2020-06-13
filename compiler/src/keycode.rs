use std::iter::once;

use lalrpop_util::lalrpop_mod;

lalrpop_mod!(pub keycode);
pub use keycode::TermListParser;

#[derive(Debug)]
pub enum Term {
    Mod(u8, Vec<Term>),
    Key(u8),
}

impl Term {
    pub fn into_keycodes(self) -> Vec<u8> {
        match self {
            Term::Key(k) => vec![k],
            Term::Mod(m, terms) => once(m)
                .chain(terms.into_iter().flat_map(|t| t.into_keycodes()))
                .chain(once(m))
                .collect(),
        }
    }

    pub fn new_key(s: &str) -> Option<Term> {
        let keycode = match s {
            "a" => 0x04,
            "b" => 0x05,
            "c" => 0x06,
            "d" => 0x07,
            "e" => 0x08,
            "f" => 0x09,
            "g" => 0x0a,
            "h" => 0x0b,
            "i" => 0x0c,
            "j" => 0x0d,
            "k" => 0x0e,
            "l" => 0x0f,
            "m" => 0x10,
            "n" => 0x11,
            "o" => 0x12,
            "p" => 0x13,
            "q" => 0x14,
            "r" => 0x15,
            "s" => 0x16,
            "t" => 0x17,
            "u" => 0x18,
            "v" => 0x19,
            "w" => 0x1a,
            "x" => 0x1b,
            "y" => 0x1c,
            "z" => 0x1d,

            "1" => 0x1e,
            "2" => 0x1f,
            "3" => 0x20,
            "4" => 0x21,
            "5" => 0x22,
            "6" => 0x23,
            "7" => 0x24,
            "8" => 0x25,
            "9" => 0x26,
            "0" => 0x27,

            "Return" => 0x28,
            "Escape" => 0x29,
            "BackSpace" => 0x2a,
            "Tab" => 0x2b,
            "Space" => 0x2c,
            "-" => 0x2d,
            "=" => 0x2e,
            "\\" => 0x31,
            ";" => 0x33,
            "'" => 0x34,
            "`" => 0x35,
            "," => 0x36,
            "." => 0x37,
            "/" => 0x38,
            "Caps_Lock" => 0x39,

            "F1" => 0x3a,
            "F2" => 0x3b,
            "F3" => 0x3c,
            "F4" => 0x3d,
            "F5" => 0x3e,
            "F6" => 0x3f,
            "F7" => 0x40,
            "F8" => 0x41,
            "F9" => 0x42,
            "F10" => 0x43,
            "F11" => 0x44,
            "F12" => 0x45,

            "Insert" => 0x49,
            "Home" => 0x4a,
            "Pageup" => 0x4b,
            "Delete" => 0x4c,
            "End" => 0x4d,
            "Pagedown" => 0x4e,
            "Right" => 0x4f,
            "Left" => 0x50,
            "Down" => 0x51,
            "Up" => 0x52,

            "XF86Cut" => 0x7b,
            "XF86Copy" => 0x7c,
            "XF86Paste" => 0x7d,
            "XF86AudioRaiseVolume" => 0x80,
            "XF86AudioLowerVolume" => 0x81,

            "XF86AudioPause" => 0xe8,
            "XF86AudioPrev" => 0xea,
            "XF86AudioNext" => 0xeb,
            "XF86Eject" => 0xec,
            "XF86AudioMute" => 0xef,
            "XF86WWW" => 0xf0,
            "XF86Back" => 0xf1,
            "XF86Forward" => 0xf2,
            "XF86Stop" => 0xf3,
            "XF86ScrollUp" => 0xf5,
            "XF86ScrollDown" => 0xf6,
            "XF86Sleep" => 0xf8,
            "XF86Refresh" => 0xfa,
            "XF86Calculator" => 0xfb,
            _ => 0,
        };
        if keycode == 0 {
            None
        } else {
            Some(Term::Key(keycode))
        }
    }
}
