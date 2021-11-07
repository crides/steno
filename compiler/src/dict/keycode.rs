use std::iter::once;

#[derive(Debug, Clone, PartialEq, Eq, Hash)]
pub enum KeyExpr {
    Mod(u8, KeyExprs),
    Key(u8),
}

#[derive(Debug, Clone, PartialEq, Eq, Hash)]
pub struct KeyExprs(pub Vec<KeyExpr>);

impl KeyExprs {
    pub fn byte_len(&self) -> usize {
        self.0.iter().map(|k| k.byte_len()).sum()
    }

    pub fn to_keycodes(&self) -> Vec<u8> {
        self.0.iter().flat_map(|t| t.to_keycodes()).collect()
    }
}

impl KeyExpr {
    fn byte_len(&self) -> usize {
        match self {
            KeyExpr::Mod(_, ks) => ks.byte_len(),
            KeyExpr::Key(_) => 1,
        }
    }

    fn to_keycodes(&self) -> Vec<u8> {
        match self {
            KeyExpr::Key(k) => vec![*k],
            KeyExpr::Mod(m, keys) => once(*m).chain(keys.to_keycodes()).chain(once(*m)).collect(),
        }
    }

    pub fn modifier(s: &str) -> Option<u8> {
        let b = match s {
            "control" | "Control_L" => 0xe0,
            "Shift_L" | "shift" => 0xe1,
            "Alt_L" | "alt" | "option" => 0xe2,
            "Super_L" | "super" | "windows" | "command" => 0xe3,
            "Control_R" => 0xe4,
            "Shift_R" => 0xe5,
            "Alt_R" => 0xe6,
            "Super_R" => 0xe7,
            _ => 0,
        };
        if b == 0 {
            None
        } else {
            Some(b)
        }
    }

    pub fn key(s: &str) -> Option<KeyExpr> {
        let keycode = match s {
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
            _ => match s.to_ascii_lowercase().as_str() {
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

                "return" | "enter" => 0x28,
                "escape" => 0x29,
                "backspace" => 0x2a,
                "tab" => 0x2b,
                "space" => 0x2c,
                "-" => 0x2d,
                "=" => 0x2e,
                "\\" => 0x31,
                ";" => 0x33,
                "'" => 0x34,
                "`" => 0x35,
                "," => 0x36,
                "." => 0x37,
                "/" => 0x38,
                "caps_lock" | "capslock" => 0x39,

                "f1" => 0x3a,
                "f2" => 0x3b,
                "f3" => 0x3c,
                "f4" => 0x3d,
                "f5" => 0x3e,
                "f6" => 0x3f,
                "f7" => 0x40,
                "f8" => 0x41,
                "f9" => 0x42,
                "f10" => 0x43,
                "f11" => 0x44,
                "f12" => 0x45,

                "insert" => 0x49,
                "home" => 0x4a,
                "pageup" => 0x4b,
                "delete" => 0x4c,
                "end" => 0x4d,
                "pagedown" => 0x4e,
                "right" => 0x4f,
                "left" => 0x50,
                "down" => 0x51,
                "up" => 0x52,
                _ => 0,
            },
        };
        if keycode == 0 {
            None
        } else {
            Some(KeyExpr::Key(keycode))
        }
    }
}
