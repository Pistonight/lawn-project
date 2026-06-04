// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Pistonight/lawn-project contributors

use std::io::Write;

use cu::pre::*;

mod cli;
pub use cli::{Cli, Inputs};
mod data;
mod xml;

pub fn run(cli: Cli) -> cu::Result<()> {
    let inputs = cli.read_inputs()?;
    if cli.dump {
        return dump(inputs, cli.ptr);
    }
    compile(inputs)
}

pub fn dump(inputs: Inputs, show_ptr: bool) -> cu::Result<()> {
    match inputs {
        Inputs::Stdout(bytes) => {
            let stream = data::ReanimCompiledStream::read_compiled(&bytes)?;
            let data = stream.read()?;
            let json = data.to_json(show_ptr)?;
            println!("{json}");
            cu::lv::disable_print_time();
        }
        Inputs::Files(files) => {
            let len = files.len();
            for (output_path, input) in files {
                match input.path {
                    Some(p) => cu::debug!("dumping '{}'", p.display()),
                    None => cu::debug!("dumping <stdin>"),
                }
                let stream = data::ReanimCompiledStream::read_compiled(&input.bytes)?;
                let data = stream.read()?;
                let json = data.to_json(show_ptr)?;
                cu::fs::write(output_path, json)?;
            }
            if len > 1 {
                cu::info!("dumped {len} files");
            }
        }
    }
    Ok(())
}

pub fn compile(inputs: Inputs) -> cu::Result<()> {
    match inputs {
        Inputs::Stdout(bytes) => {
            let xml_src = xml::format_document(&bytes)?;
            let document = xml::ReanimDocument::parse_xml(&xml_src)?;
            let stream = document.parse()?.compile()?;
            let mut stdout = std::io::stdout();
            cu::check!(stream.write(&mut stdout), "failed to write to stdout")?;
            cu::check!(stdout.flush(), "failed to flush to stdout")?;
            cu::lv::disable_print_time();
        }
        Inputs::Files(files) => {
            let len = files.len();
            for (output_path, input) in files {
                let input_path = input
                    .path
                    .map(|x| format!("'{}'", x.display()))
                    .unwrap_or("<stdin>".into());
                cu::debug!("compiling {input_path}");
                let stream = cu::check!(
                    (|| {
                        let xml_src = xml::format_document(&input.bytes)?;
                        let document = xml::ReanimDocument::parse_xml(&xml_src)?;
                        let stream = document.parse()?.compile()?;
                        cu::Ok(stream)
                    })(),
                    "failed to parse input file: '{input_path}'",
                )?;
                let mut writer = cu::fs::writer(output_path)?;
                stream.write(&mut writer)?;
                writer.flush()?;
            }
            if len > 1 {
                cu::info!("compiled {len} files");
            }
        }
    }
    Ok(())
}
