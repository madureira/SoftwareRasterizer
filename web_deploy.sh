#!/usr/bin/env bash

set -euo pipefail

# ============================================================
# WEB DEPLOY SCRIPT
# Copies the web build output to docs/ for GitHub Pages hosting.
# GitHub Pages is configured to serve from the /docs folder.
# ============================================================

BUILD_DIR="build/web"
DEPLOY_DIR="docs"

# Verify the web build exists
if [ ! -f "$BUILD_DIR/index.html" ]; then
    echo "Error: Web build not found. Run ./build.sh web first."
    exit 1
fi

# Create deploy directory
mkdir -p "$DEPLOY_DIR"

# Copy output files
cp "$BUILD_DIR/index.html" "$DEPLOY_DIR/"
cp "$BUILD_DIR/index.js"   "$DEPLOY_DIR/"
cp "$BUILD_DIR/index.wasm" "$DEPLOY_DIR/"
[[ -f "$BUILD_DIR/index.data" ]] && cp "$BUILD_DIR/index.data" "$DEPLOY_DIR/"

# Prevent GitHub Pages from running Jekyll (required for .wasm MIME type)
touch "$DEPLOY_DIR/.nojekyll"

echo "Deployed to $DEPLOY_DIR/."
echo ""
echo "To publish on GitHub Pages:"
echo "  1. In your repo Settings -> Pages, set Source to Branch: master, Folder: /docs"
echo "  2. Then: git add docs/ && git commit -m 'Update web build' && git push"
