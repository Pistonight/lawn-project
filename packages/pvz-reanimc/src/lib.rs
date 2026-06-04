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
            // compiling is a bit slow (on my machine takes 3 seconds to compile all from the
            // original main.pak), so parallelize to get free perf
            cu::co::run(async move {
                let len = files.len();
                let mut handles = Vec::with_capacity(len);
                let pool = cu::co::pool(-1);
                for (output_path, input) in files {
                    let input_path = input
                        .path
                        .map(|x| format!("'{}'", x.display()))
                        .unwrap_or("<stdin>".into());
                    cu::debug!("compiling {input_path}");
                    let handle = pool.spawn_blocking(move || {
                        let stream = (|| {
                            let xml_src = xml::format_document(&input.bytes)?;
                            let document = xml::ReanimDocument::parse_xml(&xml_src)?;
                            let stream = document.parse()?.compile()?;
                            cu::Ok(stream)
                        })();
                        let stream =
                            cu::check!(stream, "failed to parse input file: '{input_path}'")?;
                        cu::Ok((stream, output_path))
                    });
                    handles.push(handle);
                }
                let mut set = cu::co::set(handles);
                while let Some(result) = set.next().await {
                    let (stream, output_path) = result???;
                    let mut writer = cu::fs::writer(output_path)?;
                    stream.write(&mut writer)?;
                    writer.flush()?;
                }
                if len > 1 {
                    cu::info!("compiled {len} files");
                }
                cu::Ok(())
            })?;
        }
    }
    Ok(())
}
