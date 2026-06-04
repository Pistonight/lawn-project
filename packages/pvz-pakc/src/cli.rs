// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Pistonight/lawn-project contributors

use cu::pre::*;

/// Tool for packing and unpacking .pak file
#[derive(Debug, clap::Parser, AsRef)]
#[clap(group(
    clap::ArgGroup::new("mode")
        .args(["pack", "unpack"])
        .required(true)
        .multiple(false)
))]
pub struct Cli {
    /// Pack mode (files -> .pak)
    #[clap(short, long)]
    pub pack: bool,
    /// Unpack mode (.pak -> files)
    #[clap(short, long)]
    pub unpack: bool,

    /// Input file (if unpack mode) or directory (if pack mode)
    pub input: String,
    /// Output directory (if unpack mode) or file (if pack mode)
    pub output: String,

    /// When unpacking, keep existing files in the output directory
    /// even if it's not part of the pak file
    #[clap(short, long, requires = "unpack")]
    pub keep: bool,

    #[as_ref]
    #[clap(flatten)]
    flags: cu::cli::Flags,
}
