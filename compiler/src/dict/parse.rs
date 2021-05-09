use nom::{
    branch::alt,
    bytes::complete::tag,
    character::complete::{alpha1, alphanumeric1, char, multispace0, none_of, one_of},
    combinator::{eof, map, map_opt, opt, recognize},
    multi::{many0, many1},
    sequence::{delimited, pair, preceded, terminated, tuple},
    Finish, IResult,
};

use crate::dict::keycode::KeyExpr;

// pub type ParseError<'i> = nom::error::Error<&'i str>;
pub type ParseError<I> = nom::error::VerboseError<I>;
pub type ParseResult<'i, T, E = ParseError<&'i str>> = IResult<&'i str, T, E>;

macro_rules! parsers {
    ($($vis:vis $name:ident: $ret:ty = $body:expr)*) => {
        $($vis fn $name(s: &str) -> ParseResult<$ret> { $body(s) })*
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
    Plover(&'i str),
    PloverMode(&'i str),
}

fn inspect<'i, T: std::fmt::Debug>(
    t: &'static str,
    mut f: impl nom::Parser<&'i str, T, ParseError<&'i str>>,
) -> impl FnMut(&'i str) -> ParseResult<T> {
    move |s: &'i str| {
        let r = f.parse(s);
        println!("[{}]: {:#?}", t, r);
        r
    }
}

parsers! {
    modifier: u8 =
    alt((
        map(alt((tag("control"), tag("Control_L"))), |_| 0x80),
        map(alt((tag("Shift_L"), tag("shift"))), |_| 0xe1),
        map(alt((tag("Alt_L"), tag("alt"), tag("option"))), |_| 0xe2),
        map(
            alt((tag("Super_L"), tag("super"), tag("windows"), tag("command"))),
            |_| 0xe3,
        ),
        map(tag("Control_R"), |_| 0xe4),
        map(tag("Shift_R"), |_| 0xe5),
        map(tag("Alt_R"), |_| 0xe6),
        map(tag("Super_R"), |_| 0xe7),
    ))

    keycode: KeyExpr =
    map_opt(
        recognize(pair(alt((alpha1, tag("_"))), many0(alt((alphanumeric1, tag("_")))))),
        KeyExpr::new_key,
    )

    keyexpr: KeyExpr =
    alt((
        keycode,
        map(tuple((modifier, char('('), keylist, char(')'))), |(m, _, k, _)| KeyExpr::Mod(m, k))
    ))

    keylist: Vec<KeyExpr> = many1(map(pair(keyexpr, multispace0), |(k, _)| k))

    text: &str = recognize(many1(alt((recognize(none_of(r"\{}")), recognize(pair(char('\\'), one_of(r"\{}")))))))
    attachable_text: &str = recognize(many1(alt((recognize(none_of(r"\{}^")), recognize(pair(char('\\'), one_of(r"\{}^")))))))

    meta_inner: Parsed =
    alt((
        map(preceded(char('#'), keylist), Parsed::Keycodes),
        map(tag("PLOVER:ADD_TRANSLATION"), |_| Parsed::DictEdit),
        map(preceded(tag("PLOVER:"), text), Parsed::Plover),
        map(preceded(tag("MODE:"), text), Parsed::PloverMode),
        map(preceded(char('&'), text), Parsed::Glue),
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
            map(tag(""), |_| Parsed::ResetFormat),
        )),
    ))

    atom: Parsed = alt((map(text, Parsed::Text), map(tuple((char('{'), meta_inner, char('}'))), |(_, m, _)| m)))

    entry: Vec<Parsed> = terminated(many1(atom), eof)
}

pub fn parse_entry(s: &str) -> Result<Vec<Parsed>, ParseError<&str>> {
    entry(s).finish().map(|(_, p)| p)
}
