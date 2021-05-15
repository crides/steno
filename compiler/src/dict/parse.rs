use nom::{
    branch::alt,
    bytes::complete::tag,
    character::complete::{alpha1, alphanumeric1, char, multispace0, none_of, one_of},
    combinator::{cut, eof, map, map_res, opt, peek, recognize},
    error::VerboseError,
    multi::{many0, many1},
    sequence::{delimited, pair, preceded, terminated, tuple},
    Err::Error,
    Finish, IResult,
};

use crate::dict::keycode::KeyExpr;

// pub type ParseError<'i> = nom::error::Error<&'i str>;
pub type ParseResult<'i, T, E = ParseEntryError<'i>> = IResult<&'i str, T, E>;

#[derive(Debug, PartialEq)]
pub enum ParseEntryError<'i> {
    InvalidKeycode(&'i str),
    InvalidModifier(&'i str),
    Plover(&'i str),
    PloverMode(&'i str),
    UnknownEntry(&'i str),
    Nom(nom::error::VerboseError<&'i str>),
}

impl<'i> nom::error::ParseError<&'i str> for ParseEntryError<'i> {
    fn from_error_kind(input: &'i str, kind: nom::error::ErrorKind) -> Self {
        ParseEntryError::Nom(VerboseError::from_error_kind(input, kind))
    }

    fn append(input: &'i str, kind: nom::error::ErrorKind, other: Self) -> Self {
        match other {
            ParseEntryError::Nom(err) => {
                ParseEntryError::Nom(VerboseError::append(input, kind, err))
            }
            _ => other,
        }
    }
}

impl<'i> nom::error::FromExternalError<&'i str, ParseEntryError<'i>> for ParseEntryError<'i> {
    fn from_external_error(
        _input: &'i str,
        _kind: nom::error::ErrorKind,
        e: ParseEntryError<'i>,
    ) -> Self {
        e
    }
}

impl<'i> ParseEntryError<'i> {
    pub fn format_error(self, input: &'i str) -> String {
        match self {
            ParseEntryError::Nom(n) => {
                format!("Nom error:\n{}", nom::error::convert_error(input, n))
            }
            ParseEntryError::InvalidKeycode(k) => format!("Invalid keycode '{}'", k),
            ParseEntryError::InvalidModifier(k) => format!("Invalid modifier '{}'", k),
            ParseEntryError::Plover(p) => format!("Plover command '{}' not supported yet", p),
            ParseEntryError::PloverMode(p) => {
                format!("Plover MODE command '{}' not supported yet", p)
            }
            ParseEntryError::UnknownEntry(e) => {
                format!("Unknown entry '{}'", e)
            }
        }
    }
}

macro_rules! parsers {
    ($($name:ident: $ret:ty = $body:expr)*) => {
        $(fn $name(s: &str) -> ParseResult<$ret> { $body(s) })*
    };
}

const ATTACH: char = '^';

#[derive(Debug, Serialize)]
pub enum Parsed<'i> {
    Keycodes(Vec<KeyExpr>),
    Text(&'i str),
    HalfStop(&'i str),
    FullStop(&'i str),
    Glue(&'i str),
    AttachFront(&'i str),
    AttachBack(&'i str),
    AttachBoth(&'i str),
    Carry(bool, &'i str, bool),
    Capital,
    CapitalLast,
    Lower,
    LowerLast,
    Upper,
    UpperLast,
    Attach,
    ResetFormat,
    ToggleStar,
    Repeat,
    RetroSpace,
    RetroNoSpace,
    DictEdit,
}

fn inspect<'i, T: std::fmt::Debug>(
    _t: &'static str,
    mut f: impl nom::Parser<&'i str, T, ParseEntryError<'i>>,
) -> impl FnMut(&'i str) -> ParseResult<T> {
    move |s: &'i str| {
        let r = f.parse(s);
        // println!("[{}]: {:?}", _t, r);
        r
    }
}

parsers! {
    ident: &str = recognize(pair(alt((alpha1, tag("_"))), many0(alt((alphanumeric1, tag("_"))))))

    keycode: KeyExpr =
    inspect("keycode", map_res(ident, |k| KeyExpr::key(k).ok_or_else(|| ParseEntryError::InvalidKeycode(k))))
}

fn keyexpr(s: &str) -> ParseResult<KeyExpr> {
    let (after_key, key) = ident(s)?;
    if let Ok((after_left, _)) = char::<_, ParseEntryError>('(')(after_key) {
        let m =
            KeyExpr::modifier(key).ok_or_else(|| Error(ParseEntryError::InvalidModifier(key)))?;
        let ke = map(terminated(cut(keylist), char(')')), |k| KeyExpr::Mod(m, k))(after_left);
        ke
    } else {
        keycode(s)
    }
}

parsers! {
    keylist: Vec<KeyExpr> = many1(map(pair(keyexpr, multispace0), |(k, _)| k))

    text: &str = recognize(many1(alt((recognize(none_of(r"\{}")), recognize(pair(char('\\'), one_of(r"\{}")))))))
    attachable_text: &str = recognize(many1(alt((recognize(none_of(r"\{}^")), recognize(pair(char('\\'), one_of(r"\{}^")))))))

    meta_inner: Parsed =
    alt((
        map(preceded(char('#'), inspect("keylist", cut(keylist))), Parsed::Keycodes),
        map(tag("PLOVER:ADD_TRANSLATION"), |_| Parsed::DictEdit),
        preceded(tag("PLOVER:"), cut(map_res(text, |p| Err(ParseEntryError::Plover(p))))),
        preceded(tag("MODE:"), cut(map_res(text, |p| Err(ParseEntryError::PloverMode(p))))),
        map(preceded(char('&'), cut(text)), Parsed::Glue),
        map(
            tuple((opt(char(ATTACH)), preceded(tag("~|"), attachable_text), opt(char(ATTACH)))),
            |(f, t, b)| Parsed::Carry(f.is_some(), t, b.is_some())
        ),
        map(delimited(char(ATTACH), attachable_text, char(ATTACH)), Parsed::AttachBoth),
        map(preceded(char(ATTACH), text), Parsed::AttachFront),
        map(char(ATTACH), |_| Parsed::Attach),
        map(terminated(attachable_text, char(ATTACH)), Parsed::AttachBack),
        alt((
            map(tag("-|"), |_| Parsed::Capital),
            map(tag("*-|"), |_| Parsed::CapitalLast),
            map(char('>'), |_| Parsed::Lower),
            map(tag("*>"), |_| Parsed::LowerLast),
            map(char('<'), |_| Parsed::Upper),
            map(tag("*<"), |_| Parsed::UpperLast),
            map(tag("*?"), |_| Parsed::RetroSpace),
            map(tag("*!"), |_| Parsed::RetroNoSpace),
            map(tag("*+"), |_| Parsed::Repeat),
            map(char('*'), |_| Parsed::ToggleStar),
            map(alt((tag(","), tag(";"), tag(":"), tag("--"))), Parsed::HalfStop),
            map(recognize(alt((char('?'), char('!'), char('.')))), Parsed::FullStop),
            // HACK? peek back up to ensure this is _indeed_ an empty entry without passing it to the
            // all-handling error muncher
            map(terminated(tag(""), peek(char('}'))), |_| Parsed::ResetFormat),
            inspect("last", cut(map_res(recognize(many1(none_of("}"))), |i| Err(ParseEntryError::UnknownEntry(i))))),
        )),
    ))

    atom: Parsed = alt((preceded(char('{'), cut(terminated(inspect("minner", meta_inner), char('}')))), map(text, Parsed::Text)))

    entry: Vec<Parsed> = terminated(many1(atom), eof)
}

pub fn parse_entry(s: &str) -> Result<Vec<Parsed>, ParseEntryError> {
    entry(s).finish().map(|(_, p)| p)
}
