// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Pistonight/lawn-project contributors

mod cli;
mod driver;
mod filetime;
mod pak;

#[cu::cli]
fn main(args: cli::Cli) -> cu::Result<()> {
    if args.unpack {
        return driver::unpack(&args.input, &args.output, args.keep);
    }
    driver::pack(&args.input, &args.output)
}
