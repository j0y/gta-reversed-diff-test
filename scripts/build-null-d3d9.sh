#!/bin/bash
set -euo pipefail

# Build the null D3D9 stub DLL using MSVC via Wine
# Runs inside the same Docker container as the main build

echo "=== Building null d3d9.dll ==="

cmake -S /src/null_d3d9 -B /tmp/null_d3d9_build \
    -DCMAKE_TOOLCHAIN_FILE=/opt/toolchain-msvc-wine.cmake \
    -DCMAKE_BUILD_TYPE=Release \
    -G Ninja

cmake --build /tmp/null_d3d9_build

echo ""
echo "=== Build complete ==="
ls -la /tmp/null_d3d9_build/d3d9.dll

if [ -d "/build" ]; then
    cp -v /tmp/null_d3d9_build/d3d9.dll /build/d3d9.dll
fi
