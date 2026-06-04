// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Pistonight/lawn-project contributors
// * * * * *
// This file is adopted from:
// https://github.com/lmintlcx/pvztoolkit/blob/99fcdecf53f80bd02d784eee243fa5b12a9e59c1/src/pak.cpp
//
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) Imintlcx

use std::fs::File;
use std::io::{BufReader, Read as _, Write};
use std::os::windows::fs::MetadataExt;
use std::path::{Component, Path};

use cu::pre::*;
use pvz_utils::BinReader;

use crate::filetime::{self, WinFileTime};

const HEADER_MAGIC: u32 = 0xbac04ac0;
const HEADER_VERSION: u32 = 0;
const INDEX_BLOCK_OK: u8 = 0;
const INDEX_BLOCK_EOF: u8 = 0x80;

/// Read *decrypted* bytes as a PakFile
pub fn read(bytes: &[u8]) -> cu::Result<PakFile<'_>> {
    let mut r = BinReader::new(bytes);
    let header = cu::check!(PakHeader::read(&mut r), "failed to read header")?;

    let mut index = Vec::new();
    loop {
        match r.read_u8()? {
            INDEX_BLOCK_EOF => {
                break;
            }
            INDEX_BLOCK_OK => {}
            other => {
                cu::bail!("unexpected index bit: 0x{other:02x}");
            }
        }
        let entry = cu::check!(
            PakIndex::read(&mut r),
            "failed to read index[{}]",
            index.len()
        )?;
        index.push(entry);
    }

    // check the sizes declared in the index matches the input bytes len
    let expected_size = r.pos() as u32 + index.iter().map(|x| x.file_size).sum::<u32>();
    if expected_size != bytes.len() as u32 {
        cu::bail!(
            "size mismatch: expected {expected_size} bytes, actual input is {} bytes",
            bytes.len()
        );
    }

    let mut files = Vec::with_capacity(index.len());
    for block in &index {
        let size = block.file_size;
        let bytes = r.read_slice(size as usize)?;
        files.push(bytes);
    }

    Ok(PakFile {
        header,
        index,
        files,
    })
}

pub fn write(
    base: &Path,
    files: impl Iterator<Item = impl AsRef<Path>>,
    out: &Path,
) -> cu::Result<()> {
    let base = base.normalize()?;
    let mut index_buf = Vec::new();
    let mut file_buf = Vec::new();

    PakHeader::new().write(&mut index_buf);
    for file in files {
        let path = file.as_ref().normalize()?;
        let rel_path = path.try_to_rel_from(&base);
        let entry_path = cu::check!(
            make_path(rel_path.as_os_str().as_encoded_bytes()),
            "failed to make path for file: '{}'",
            path.display()
        )?;
        let mut file = cu::check!(
            File::open(&path),
            "failed to open file: '{}'",
            path.display()
        )?;
        let metadata = cu::check!(
            file.metadata(),
            "failed to get file metadata: '{}'",
            path.display()
        )?;
        let file_size = metadata.file_size();
        if file_size >= u32::MAX as u64 {
            cu::bail!(
                "file is too big ({} bytes): '{}'",
                file_size,
                path.display()
            );
        }
        let file_time = filetime::get(&file)?;
        cu::debug!(
            "{file_time} 0x{file_size:08x} {}",
            String::from_utf8_lossy(&entry_path)
        );
        index_buf.push(INDEX_BLOCK_OK);
        let index = cu::check!(
            PakIndex::new(&entry_path, file_size as u32, file_time),
            "failed to create index for file: '{}'",
            path.display()
        )?;
        index.write(&mut index_buf);

        let mut reader = BufReader::new(&mut file);
        cu::check!(
            reader.read_to_end(&mut file_buf),
            "failed to read file content: '{}'",
            path.display()
        )?;
    }

    index_buf.push(INDEX_BLOCK_EOF);
    crypt(&mut index_buf);
    crypt(&mut file_buf);
    let mut writer = cu::fs::buf_writer(out)?;
    cu::check!(writer.write_all(&index_buf), "failed to write index buffer")?;
    cu::check!(writer.write_all(&file_buf), "failed to write file buffer")?;
    cu::check!(writer.flush(), "failed to flush output file")?;
    Ok(())
}

/// Convert relative path to validated path in the pak
pub fn make_path(rel_path: &[u8]) -> cu::Result<Vec<u8>> {
    // first validate rel_path is ascii
    if !rel_path.is_ascii() {
        cu::bail!(
            "only ascii paths are supported right now: '{}'",
            String::from_utf8_lossy(rel_path)
        );
    }
    // safety: we just checked above
    let rel_str = unsafe { str::from_utf8_unchecked(rel_path) };
    let rel_path = Path::new(rel_str);
    if rel_path.is_absolute() || rel_path.starts_with("..") {
        cu::bail!("illegal path: '{}'", rel_path.display());
    }
    let rel_path_len = rel_path.as_os_str().len();
    // we are only taking out components so the length must be shorter
    let mut file_name = Vec::with_capacity(rel_path_len);
    for part in rel_path.components() {
        match part {
            Component::Prefix(_) => {
                cu::bail!("unexpected prefix in entry path: '{}'", rel_path.display());
            }
            Component::RootDir => {
                cu::bail!(
                    "unexpected root dir in entry path: '{}'",
                    rel_path.display()
                );
            }
            Component::CurDir => {
                // ignore cur dir
            }
            Component::ParentDir => {
                cu::bail!(
                    "unexpected parent dir in entry path: '{}'",
                    rel_path.display()
                );
            }
            Component::Normal(os_str) => {
                if !file_name.is_empty() {
                    file_name.push(b'\\');
                }
                file_name.extend_from_slice(os_str.as_encoded_bytes());
            }
        }
    }
    Ok(file_name)
}

pub struct PakFile<'a> {
    #[allow(unused)]
    pub header: PakHeader,
    index: Vec<PakIndex<'a>>,
    files: Vec<&'a [u8]>,
}

impl PakFile<'_> {
    pub fn iter(&self) -> impl Iterator<Item = (&PakIndex<'_>, &[u8])> {
        std::iter::zip(self.index.iter(), self.files.iter().copied())
    }
    pub fn len(&self) -> usize {
        self.index.len()
    }
}
#[derive(Clone, Copy)]
pub struct PakHeader {
    pub magic: u32,
    pub version: u32,
}

impl PakHeader {
    fn new() -> Self {
        Self {
            magic: HEADER_MAGIC,
            version: HEADER_VERSION,
        }
    }
    fn read(r: &mut BinReader<'_>) -> cu::Result<Self> {
        let magic = r.read_u32()?;
        if magic != HEADER_MAGIC {
            cu::bail!("invalid magic: expected 0x{HEADER_MAGIC:08x}, actual 0x{magic:08x}");
        }
        let version = r.read_u32()?;
        if version != HEADER_VERSION {
            cu::bail!("invalid version: expected 0x{HEADER_VERSION:08x}, actual 0x{version:08x}");
        }
        Ok(Self { magic, version })
    }
    fn write(self, w: &mut Vec<u8>) {
        w.extend_from_slice(&self.magic.to_le_bytes());
        w.extend_from_slice(&self.version.to_le_bytes());
    }
}

pub struct PakIndex<'a> {
    path_len: u8,
    pub path: &'a [u8],
    pub file_size: u32,
    pub file_time: WinFileTime,
}

impl<'a> PakIndex<'a> {
    fn new(path: &'a [u8], file_size: u32, file_time: WinFileTime) -> cu::Result<Self> {
        let len = path.len();
        if len >= u8::MAX as usize {
            cu::bail!("path is too long: {}", String::from_utf8_lossy(path));
        }
        Ok(Self {
            path_len: len as u8,
            path,
            file_size,
            file_time,
        })
    }
    fn read(r: &mut BinReader<'a>) -> cu::Result<Self> {
        let name_len = cu::check!(r.read_u8(), "failed to read path_len")?;
        let name = cu::check!(r.read_slice(name_len as usize), "failed to read path")?;
        let file_size = cu::check!(r.read_u32(), "failed to read file_size")?;
        let dw_lo = cu::check!(r.read_u32(), "failed to read file_time lo")?;
        let dw_hi = cu::check!(r.read_u32(), "failed to read file_time hi")?;
        let file_time = WinFileTime { dw_lo, dw_hi };
        Ok(PakIndex {
            path_len: name_len,
            path: name,
            file_size,
            file_time,
        })
    }
    fn write(&self, w: &mut Vec<u8>) {
        w.reserve(1 + self.path_len as usize + 4 + 8);
        w.push(self.path_len);
        w.extend_from_slice(self.path);
        w.extend_from_slice(&self.file_size.to_le_bytes());
        w.extend_from_slice(&self.file_time.dw_lo.to_le_bytes());
        w.extend_from_slice(&self.file_time.dw_hi.to_le_bytes());
    }
}

pub fn crypt(buf: &mut Vec<u8>) {
    for b in buf {
        *b ^= 0xf7;
    }
}
