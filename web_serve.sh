#!/usr/bin/env bash

set -euo pipefail

# ============================================================
# WEB SERVE SCRIPT
# Serves the web build locally via Python HTTP server.
# ============================================================

BUILD_DIR="build/web"
PORT="${1:-8080}"

if ! command -v python3 &>/dev/null; then
    echo "Error: python3 is not installed. Install it from https://www.python.org/downloads/"
    exit 1
fi

if [ ! -f "$BUILD_DIR/index.html" ]; then
    echo "Error: Web build not found. Run ./build.sh web first."
    exit 1
fi

echo "Serving at http://localhost:${PORT}"
echo "Press Ctrl+C to stop."
echo ""

python3 -m http.server "$PORT" --directory "$BUILD_DIR"
