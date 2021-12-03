use std::env;
use std::path::PathBuf;

fn main() {
    let header = "../qmk/orthography.h";
    let source = "../qmk/orthography.c";
    println!("cargo:rerun-if-changed={}", header);
    println!("cargo:rerun-if-changed={}", source);
    let bindings = bindgen::Builder::default()
        .header(header)
        .parse_callbacks(Box::new(bindgen::CargoCallbacks))
        .generate()
        .expect("Unable to generate bindings");

    // Write the bindings to the $OUT_DIR/bindings.rs file.
    let out_path = PathBuf::from(env::var("OUT_DIR").unwrap());
    bindings
        .write_to_file(out_path.join("bindings.rs"))
        .expect("Couldn't write bindings!");

    cc::Build::new().file(source).compile("libortho.a");
}
