// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Pistonight/lawn-project contributors

#[cu::cli]
fn main(cli: pvz_reanimc::Cli) -> cu::Result<()> {
    pvz_reanimc::run(cli)
}
