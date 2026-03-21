#!/bin/bash
set -euo pipefail

# Incremental build — only recompiles changed test/stub files.
# Requires a prior full build (build.sh) with the build tree persisted in a volume.
#
# Expected mounts:
#   /src/headless_stubs  - test source files (read-only)
#   /build               - output directory
#   /tmp/gta-build       - persisted build tree (read-write volume)

WORK_DIR="/tmp/gta-build"
BUILD_DIR="/build"

if [ ! -f "${WORK_DIR}/build/Release/build.ninja" ]; then
    echo "ERROR: No existing build tree at ${WORK_DIR}."
    echo "Run a full build first: docker-build.sh build"
    exit 1
fi

echo "=== Incremental test build ==="

# ---- Neutralize CMAKE_GENERATOR_PLATFORM for reconfigure ----
# The patched root CMakeLists.txt sets CMAKE_GENERATOR_PLATFORM=Win32 before
# project(). Ninja doesn't support platform specification, which is fine on
# initial configure (platform gets cached and the set() is a no-op), but on
# reconfigure CMake re-evaluates it and fails. Neutralize it — but only if
# not already done (avoid touching mtime which triggers Ninja auto-reconfigure).
if grep -q '^[[:space:]]*set(CMAKE_GENERATOR_PLATFORM Win32)' "${WORK_DIR}/CMakeLists.txt" 2>/dev/null; then
    sed -i 's/^\( *\)set(CMAKE_GENERATOR_PLATFORM Win32)/\1# set(CMAKE_GENERATOR_PLATFORM Win32)  # neutralized for incremental/' \
        "${WORK_DIR}/CMakeLists.txt"
fi

# ---- Copy changed test/stub files ----
CHANGED=0
NEEDS_RECONFIGURE=0
for src in /src/headless_stubs/headless_render_stubs.cpp \
           /src/headless_stubs/soak_test.cpp \
           /src/headless_stubs/differential_test.cpp \
           /src/headless_stubs/game_tests.cpp \
           /src/headless_stubs/TestFramework.h \
           /src/headless_stubs/ScenarioHelpers.h; do
    [ -f "$src" ] || continue
    base=$(basename "$src")
    dst="${WORK_DIR}/source/${base}"
    if [ ! -f "$dst" ] || ! cmp -s "$src" "$dst"; then
        cp "$src" "$dst"
        echo "  Updated: ${base}"
        CHANGED=1
    fi
done

# Copy test files — detect new and changed
if [ -d "/src/headless_stubs/tests" ]; then
    NEW_TESTS=""
    for f in /src/headless_stubs/tests/test_*.cpp; do
        [ -f "$f" ] || continue
        base=$(basename "$f")
        dst="${WORK_DIR}/source/${base}"
        if [ ! -f "$dst" ]; then
            cp "$f" "$dst"
            echo "  NEW test: ${base}"
            NEW_TESTS="${NEW_TESTS}\nlist(APPEND SOURCE_FILES_LIST ${base})"
            CHANGED=1
        elif ! cmp -s "$f" "$dst"; then
            cp "$f" "$dst"
            echo "  Updated: ${base}"
            CHANGED=1
        fi
    done

    # Register new test files in CMakeLists.txt
    if [ -n "$NEW_TESTS" ]; then
        echo "  Registering new tests in CMakeLists.txt..."
        sed -i "s|list(APPEND SOURCE_FILES_LIST game_tests.cpp)|list(APPEND SOURCE_FILES_LIST game_tests.cpp)${NEW_TESTS}|" \
            "${WORK_DIR}/source/CMakeLists.txt"
        NEEDS_RECONFIGURE=1
    fi
fi

if [ "$CHANGED" -eq 0 ]; then
    echo "  No changes detected."
    echo "  (Forcing rebuild anyway in case source was edited in-place)"
fi

# ---- Fix Ninja stale dependency issues ----
# The full build renames Audio subdirs to TitleCase (managers -> Managers, etc.)
# but Ninja's dep database records lowercase paths, causing full rebuilds.
# Create lowercase symlinks so Ninja finds the deps it recorded.
AUDIO_DIR="${WORK_DIR}/source/game_sa/Audio"
if [ -d "$AUDIO_DIR" ]; then
    for dir in Config Entities Hardware Loaders Managers; do
        lower=$(echo "$dir" | tr 'A-Z' 'a-z')
        if [ -d "${AUDIO_DIR}/${dir}" ] && [ ! -e "${AUDIO_DIR}/${lower}" ]; then
            ln -s "$dir" "${AUDIO_DIR}/${lower}"
        fi
    done
fi

# ---- CMake reconfigure (if needed) + Wine path fix + build ----
# We must control the order: configure → fix paths → build.
# Cannot use `cmake --build` because Ninja auto-reconfigures when
# CMakeLists.txt changes, then immediately builds with broken paths.
cd "${WORK_DIR}"

# Only reconfigure when new source files were added. When only existing files
# change, Ninja handles incremental builds without reconfigure. When we do
# reconfigure, we do it explicitly here so we can fix Wine paths before building.
if [ "${NEEDS_RECONFIGURE}" = "1" ]; then
    echo ""
    echo "--- CMake configure (new source files) ---"
    cmake --preset conan-release -DGTASA_UNITY_BUILD=ON
fi

# Fix Wine Z: vs z: drive letter bug in any generated include paths.
# Only rewrite files that actually contain unfixed paths (avoid touching mtime).
echo "--- Fixing Wine paths ---"
WINE_FIX_COUNT=0
for f in $(find "${WORK_DIR}" -name "cmake_pch.hxx" -path "*/CMakeFiles/*" 2>/dev/null) \
         $(find "${WORK_DIR}" -name "unity_*_cxx.cxx" -path "*/Unity/*" 2>/dev/null); do
    if grep -q '#include "/' "$f" 2>/dev/null; then
        sed -i 's|#include "/|#include "z:/|g' "$f"
        WINE_FIX_COUNT=$((WINE_FIX_COUNT + 1))
    fi
done
echo "  Fixed ${WINE_FIX_COUNT} files"

echo ""
echo "--- Building (incremental) ---"
# Use ninja directly instead of cmake --build to prevent Ninja from
# auto-reconfiguring (which would regenerate unity files with broken paths).
ninja -C "${WORK_DIR}/build/Release" -j "$(nproc)"

# ---- Strip TLS + copy output ----
ASI_FILE=$(find "${WORK_DIR}" -name "gta_reversed.asi" | head -1)
if [ -n "$ASI_FILE" ]; then
    python3 -c "
import struct, sys
with open(sys.argv[1], 'r+b') as f:
    f.seek(0x3C)
    pe_off = struct.unpack('<I', f.read(4))[0]
    f.seek(pe_off + 4 + 16)
    opt_size = struct.unpack('<H', f.read(2))[0]
    opt_off = pe_off + 4 + 20
    f.seek(opt_off)
    magic = struct.unpack('<H', f.read(2))[0]
    dd_off = opt_off + (96 if magic == 0x10B else 112)
    tls_dd_off = dd_off + 9 * 8
    f.seek(tls_dd_off)
    old_rva, old_size = struct.unpack('<II', f.read(8))
    if old_rva != 0:
        f.seek(tls_dd_off)
        f.write(struct.pack('<II', 0, 0))
" "$ASI_FILE"
fi

echo ""
echo "=== Incremental build complete ==="
if [ -d "/build" ]; then
    cp -v "${ASI_FILE}" /build/ 2>/dev/null || true
    find "${WORK_DIR}" -name "gta_reversed.map" -exec cp -v {} /build/ \; 2>/dev/null || true
fi
