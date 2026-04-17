# SD Card I/O Example

An example project that demonstrates how to set up saving and reading a file from an SD card with libdragon.

## Requirements

- A real N64, don't run this on emulators like ares
- Libdragon Preview branch
- MIPS64 C compiler
- Make

## How to Build
This tutorial assumes you have your N64 Toolchain set up including GCC for MIPS.
Make sure you are on the preview branch of libdragon.

Clone this repository with `--recurse-submodules` or if you haven't run:

```bash

git submodule update --init
```
---
Initialize libdragon:
```bash
libdragon init
```
Then run make to build this project:

```bash
libdragon make
```

---