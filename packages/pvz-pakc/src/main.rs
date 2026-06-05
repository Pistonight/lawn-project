// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Pistonight/lawn-project contributors

mod cli;
mod driver;
mod filetime;
mod pak;

use cu::pre::*;

#[cu::cli]
fn main(args: cli::Cli) -> cu::Result<()> {
    let directory = match args.directory.as_deref() {
        Some(x) => x,
        None => cu::check!(
            args.pak_file.strip_suffix(".pak"),
            "cannot infer directory from input, please provide path to directory to pack from or unpack to"
        )?,
    };
    if args.unpack {
        return driver::unpack(&args.pak_file, directory, args.keep);
    }
    driver::pack(directory, &args.pak_file)
}
