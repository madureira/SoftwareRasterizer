# SoftwareRasterizer

A 3D software rasterizer built from scratch in C99, using SDL3 for windowing.

## Prerequisites

- CMake 3.25+
- C99-compatible compiler (GCC, Clang, or MSVC)
- Git
- **Web builds only:** [Emscripten SDK](https://emscripten.org/docs/getting_started/downloads.html)

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
./build.sh debug mac
./build.sh release mac

# Linux
./build.sh debug linux
./build.sh release linux

# WebAssembly (requires Emscripten SDK active in the shell)
./build.sh web
```

Binaries are written to `build/<type>/SoftwareRasterizer`.

## Running the web build locally

After building with `./build.sh web`, serve the output via a local HTTP server (requires Python 3):

```bash
./web_serve.sh        # serves on http://localhost:8080
./web_serve.sh 9000   # custom port
```

## Deploying to GitHub Pages

Copy the web build output to `docs/` for GitHub Pages hosting:

```bash
./web_deploy.sh
```

## VS Code

Install the recommended extensions when prompted, or manually:

- [clangd](https://marketplace.visualstudio.com/items?itemName=llvm-vs-code-extensions.vscode-clangd) — C language support and IntelliSense
- [CodeLLDB](https://marketplace.visualstudio.com/items?itemName=vadimcn.vscode-lldb) — debugger (required for the debug launch configuration)
