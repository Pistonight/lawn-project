use clap::Parser;
use cu::pre::*;
use pvz_reanimc::{self, Cli};

#[test]
fn test_mainpak() -> cu::Result<()> {
    cu::cli::level("d");
    let reanimes_original = "../pvz-assets/main/reanim/*.reanim";
    let reanime_compileds = "../pvz-assets/main/compiled/reanim/*.reanim.compiled";
    let test_output = "tests/output";

    cu::fs::rec_remove(test_output)?;
    // compile original reanim using our compiler
    pvz_reanimc::run(Cli::parse_from([
        "_",
        reanimes_original,
        "-o",
        "tests/output/compiled",
    ]))?;
    // dump original
    pvz_reanimc::run(Cli::parse_from([
        "_",
        reanime_compileds,
        "--dump",
        "-o",
        "tests/output/original_dump",
    ]))?;
    // dump our compiled
    pvz_reanimc::run(Cli::parse_from([
        "_",
        "tests/output/compiled/*",
        "--dump",
        "-o",
        "tests/output/our_dump",
    ]))?;

    // diff orignal and our compiled dump
    let dir = cu::fs::read_dir("tests/output/original_dump")?;
    let mut has_diff = false;
    for entry in dir {
        let entry = entry?;
        let name = entry.file_name().into_utf8()?;
        let our = cu::path!("tests/output/our_dump" / name);
        let original_bytes = cu::fs::read(entry.path())?;
        let our_bytes = cu::fs::read(our)?;
        if original_bytes != our_bytes {
            has_diff = true;
            cu::error!("diff found for '{name}'");
        }
    }

    cu::ensure!(!has_diff)?;

    Ok(())
}
