// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Pistonight/lawn-project contributors

use std::collections::BTreeSet;
use std::path::Path;

use cu::pre::*;

use crate::{filetime, pak};

pub fn unpack(input: &str, output: &str, keep: bool) -> cu::Result<()> {
    let output = Path::new(output);

    // collect current file list in output if not --keep
    let mut file_list = BTreeSet::new();
    if !keep && output.exists() {
        let mut walk = cu::check!(cu::fs::walk(output), "failed to walk output directory")?;
        while let Some(entry) = walk.next() {
            let entry = match entry {
                Err(e) => {
                    cu::warn!("ignored error reading file in output directory: {e:?}");
                    continue;
                }
                Ok(x) => x,
            };
            if !entry.is_file() {
                continue;
            }
            let path = cu::check!(
                pak::make_path(entry.rel_path().as_os_str().as_encoded_bytes()),
                "failed to create path for existing file in output directory"
            )?;
            file_list.insert(path);
        }
    }

    let mut bytes = cu::check!(cu::fs::read(input), "failed to read input .pak file")?;
    pak::crypt(&mut bytes);

    let pack = cu::check!(pak::read(&bytes), "failed to parse input .pak file")?;
    for (meta, bytes) in pack.iter() {
        let path = meta.path;
        let size = meta.file_size;
        let time = meta.file_time;
        let rel_path = cu::check!(
            pak::make_path(path),
            "invalid path from pak: '{}'",
            String::from_utf8_lossy(path)
        )?;
        let rel_path_str = cu::check!(
            str::from_utf8(&rel_path),
            "unexpected: path is not utf-8: '{}'",
            String::from_utf8_lossy(path)
        )?;
        cu::debug!("{time} 0x{size:08x} {rel_path_str}");
        let output_path = output.join(rel_path_str);
        cu::fs::write(&output_path, bytes)?;
        filetime::set(&output_path, time)?;

        file_list.remove(path);
    }

    if !keep {
        for path in file_list {
            let rel_path_str = cu::check!(
                str::from_utf8(&path),
                "unexpected: path is not utf-8: '{}'",
                String::from_utf8_lossy(&path)
            )?;
            let target_path = output.join(rel_path_str);
            cu::fs::remove(&target_path)?;
            cu::print!("removed {rel_path_str}");
        }
    }

    cu::info!("unpacked {} files", pack.len());
    Ok(())
}

pub fn pack(input: &str, output: &str) -> cu::Result<()> {
    let mut file_list = BTreeSet::new();
    let input = Path::new(input);
    let mut walk = cu::check!(cu::fs::walk(input), "failed to walk input directory")?;
    let mut count = 0;
    while let Some(entry) = walk.next() {
        let entry = entry?;
        if !entry.is_file() {
            continue;
        }
        file_list.insert(entry.path());
        count += 1;
    }

    cu::check!(
        pak::write(input, file_list.iter(), Path::new(output)),
        "failed to create .pak file"
    )?;
    cu::info!("packed {count} files");
    Ok(())
}
