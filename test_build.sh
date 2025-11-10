#!/bin/bash
# Simple build test script

cd "$(dirname "$0")/build" || exit 1
echo "Building..."
ninja 2>&1 | tail -20
echo ""
echo "Build exit code: $?"

