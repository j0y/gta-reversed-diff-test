#!/bin/bash
set -euo pipefail

# Build a minimal test ASI to verify DLL loading works under Wine.
#
# Usage (inside Docker):
#   /scripts/build-test-asi.sh
#
# Usage (from host):
#   docker run --rm \
#       -v ./test_asi:/src/test_asi:ro \
#       -v ./scripts/build-test-asi.sh:/scripts/build-test-asi.sh:ro \
#       -v ./build-output:/build \
#       gta-reversed-build /scripts/build-test-asi.sh

SRC="/src/test_asi"
OUT="/build"

echo "=== Building minimal test ASI ==="

# Compile
echo "--- Compiling ---"
/opt/msvc/bin/x86/cl \
    /nologo /EHsc /O2 /MD \
    /c "${SRC}/test_asi.cpp" \
    /Fo/tmp/test_asi.obj

# Link as DLL with .asi extension
echo "--- Linking ---"
/opt/msvc/bin/x86/link \
    /nologo /DLL \
    /DEF:"${SRC}/test_asi.def" \
    /OUT:/tmp/test_asi.asi \
    /MAP:/tmp/test_asi.map \
    /SUBSYSTEM:WINDOWS \
    /tmp/test_asi.obj \
    kernel32.lib user32.lib

echo "--- Output ---"
ls -la /tmp/test_asi.asi /tmp/test_asi.map

# Copy to output
cp /tmp/test_asi.asi "${OUT}/"
cp /tmp/test_asi.map "${OUT}/"

echo ""
echo "=== test_asi.asi built ($(wc -c < /tmp/test_asi.asi) bytes) ==="
