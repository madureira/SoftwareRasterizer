# SoftwareRasterizer

[![CI](https://github.com/madureira/SoftwareRasterizer/actions/workflows/ci.yml/badge.svg)](https://github.com/madureira/SoftwareRasterizer/actions/workflows/ci.yml)
![Language](https://img.shields.io/badge/language-C99-blue)
![Platform](https://img.shields.io/badge/platform-macOS%20%7C%20Linux%20%7C%20Windows%20%7C%20WebAssembly-blueviolet)
![License](https://img.shields.io/badge/license-MIT-green)

A 3D software rasterizer built from scratch in C99, using SDL3 for windowing.

> **Note:** This project is in early development. Features, performance and behavior may change significantly.

## Prerequisites

- C99-compatible compiler (GCC, Clang, or MSVC)
- CMake 3.25+
- Git
- [Emscripten SDK](https://emscripten.org/docs/getting_started/downloads.html) (Web builds only)

\
[![Live Demo](.github/live_demo_badge.svg)](https://madureira.github.io/SoftwareRasterizer)

## Getting started

### 1. Clone the repository

```bash
git clone git@github.com:madureira/SoftwareRasterizer.git
cd SoftwareRasterizer
```

### 2. Initialize submodules

SDL3 is included as a git submodule under `vendors/SDL3`. Initialize it before building:

```bash
./build.sh init
```

Or manually:

```bash
git submodule update --init --recursive
```

## Building

```bash
# macOS
./build.sh mac debug
./build.sh mac release

# Linux
./build.sh linux debug
./build.sh linux release

# WebAssembly (requires Emscripten SDK active in the shell)
./build.sh web
```

```bat
:: Windows
build.bat win debug
build.bat win release
```

Binaries are written to `build/<type>/SoftwareRasterizer` (or `.exe` on Windows).

## Testing

```bash
# macOS / Linux
./test.sh
```

```bat
:: Windows
test.bat
```

Builds and runs the test suite. Results are printed to stdout; the process exits with code `1` if any test fails.

## Running the web build locally

After building with `./build.sh web`, serve the output via a local HTTP server (requires Python 3):

```bash
./web_serve.sh        # serves on http://localhost:8080
./web_serve.sh 9000   # custom port
```

## VS Code

Two extensions are required. Install them when prompted, or manually via the links below:

- [clangd](https://marketplace.visualstudio.com/items?itemName=llvm-vs-code-extensions.vscode-clangd) — C language support: IntelliSense, go-to-definition, and diagnostics. Without it, Cmd+Click and symbol navigation will not work.
- [CodeLLDB](https://marketplace.visualstudio.com/items?itemName=vadimcn.vscode-lldb) — debugger. Without it, the debug launch configuration will not work.

After installing, run `./build.sh debug mac` (or `linux`) once to generate `build/debug/compile_commands.json`, which clangd uses to index the project.
