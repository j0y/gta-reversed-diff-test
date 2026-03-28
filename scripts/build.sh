#!/bin/bash
set -euo pipefail

# Build gta-reversed using MSVC via Wine inside Docker container
#
# Expected mounts:
#   /src/gta-reversed  - source tree (read-only)
#   /build             - output directory

SRC_DIR="/src/gta-reversed"
BUILD_DIR="/build"
CONAN_PROFILE="/opt/conanprofile-wine.txt"

echo "=== Phase 0: gta-reversed build via msvc-wine ==="
echo "Source: ${SRC_DIR}"
echo "Build:  ${BUILD_DIR}"

# ---- Step 1: Verify MSVC toolchain works ----
echo ""
echo "--- Verifying MSVC toolchain ---"
cat > /tmp/test_cpp23.cpp << 'EOF'
#include <ranges>
#include <span>
int main() {
    int arr[] = {1, 2, 3};
    auto v = std::span(arr);
    return 0;
}
EOF

/opt/msvc/bin/x86/cl /std:c++latest /EHsc /nologo /c /tmp/test_cpp23.cpp /Fo/tmp/test_cpp23.obj
echo "C++23 compilation: OK"

# ---- Step 2: Install Conan dependencies ----
echo ""
echo "--- Installing Conan dependencies ---"

WORK_DIR="/tmp/gta-build"
# Clean contents but keep the directory (may be a volume mountpoint).
if [ -d "${WORK_DIR}" ]; then
    echo "Cleaning existing build tree for full rebuild..."
    rm -rf "${WORK_DIR:?}"/* "${WORK_DIR}"/.[!.]* 2>/dev/null || true
fi
mkdir -p "${WORK_DIR}"
cp -r "${SRC_DIR}/." "${WORK_DIR}/"

cd "${WORK_DIR}"

conan install . \
    --build=missing \
    -s build_type=Release \
    --profile "${CONAN_PROFILE}"

# ---- Step 2b: Apply source patches ----
echo ""
echo "--- Applying source patches ---"

# Static patches: CMake flags, compiler settings, Wine compatibility
git apply /src/patches/06-cmake-root-flags.patch
echo "  Applied 06-cmake-root-flags.patch (root CMakeLists: /ZI, SAFESEH, /MAP, /MD)"
git apply /src/patches/07-cmake-source-flags.patch
echo "  Applied 07-cmake-source-flags.patch (source CMakeLists: includes, warnings, defines, Tracy, TLS)"
git apply /src/patches/08-disable-spdlog.patch
echo "  Applied 08-disable-spdlog.patch (spdlog OFF, gut Logging ctor/Create)"
git apply /src/patches/09-tls-elimination.patch
echo "  Applied 09-tls-elimination.patch (General.h thread_local, MemoryMgr operator new/delete)"
git apply /src/patches/12-fix-int-rng-reciprocal.patch
echo "  Applied 12-fix-int-rng-reciprocal.patch (GetRandomNumberInRange<int>: use RAND_MAX_INT_RECIPROCAL)"

# Fix case-sensitivity mismatches: CMakeLists.txt uses TitleCase but actual dirs are lowercase
echo "--- Fixing case-sensitive directory names ---"
cd "${WORK_DIR}/source/game_sa/Audio"
for dir in Config Entities Hardware Loaders Managers; do
    lower=$(echo "$dir" | tr 'A-Z' 'a-z')
    if [ -d "$lower" ] && [ ! -e "$dir" ]; then
        mv "$lower" "$dir"
        # Keep lowercase symlink — MSVC /showIncludes may report lowercase,
        # and Ninja records that casing. Without the symlink, incremental
        # builds on case-sensitive Linux see the dep as missing → full rebuild.
        ln -s "$dir" "$lower"
        echo "  Renamed: Audio/$lower -> Audio/$dir (+ symlink)"
    fi
done
cd "${WORK_DIR}"

# Fix individual file/dir case mismatches that MSVC /showIncludes reports
# differently from the actual Linux filenames. Symlinks let both casings work,
# which is critical for Ninja incremental builds on case-sensitive filesystems.
echo "--- Fixing file case mismatches for incremental builds ---"
_symlink_case() {
    local dir="$1" actual="$2" alias="$3"
    if [ -e "${dir}/${actual}" ] && [ ! -e "${dir}/${alias}" ]; then
        ln -s "$actual" "${dir}/${alias}"
        echo "  Symlink: ${alias} -> ${actual}"
    fi
}
_symlink_case "${WORK_DIR}/source/game_sa/Attractors" "PedAtmAttractor.h" "PedATMAttractor.h"
_symlink_case "${WORK_DIR}/source/game_sa/Tasks/TaskTypes" "TaskSimpleGoToPointFine.h" "TaskSimpleGotoPointFine.h"
_symlink_case "${WORK_DIR}/source" "reversiblehooks" "ReversibleHooks"

# ---- Step 2c: Inject headless stubs and test framework ----
if [ -f "/src/headless_stubs/headless_render_stubs.cpp" ]; then
    echo ""
    echo "--- Injecting headless stubs ---"
    cp /src/headless_stubs/headless_render_stubs.cpp "${WORK_DIR}/source/headless_render_stubs.cpp"

    # Add source file to CMake (dynamic — file list varies)
    sed -i 's|list(APPEND SOURCE_FILES_LIST StdInc.cpp)|list(APPEND SOURCE_FILES_LIST StdInc.cpp)\nlist(APPEND SOURCE_FILES_LIST headless_render_stubs.cpp)|' \
        "${WORK_DIR}/source/CMakeLists.txt"

    # Wire up dllmain.cpp + WinMain.cpp probes (static patches)
    git apply /src/patches/10-dllmain-headless-wiring.patch
    echo "  Applied 10-dllmain-headless-wiring.patch"

    # Runtime behavior patches (SDL, DirectSound, DecisionMaker, etc.)
    git apply /src/patches/01-headless-wine-runtime.patch
    echo "  Applied 01-headless-wine-runtime.patch"

    # WinMain progress probes
    git apply /src/patches/11-winmain-probes.patch
    echo "  Applied 11-winmain-probes.patch"

    # Diagnostic probes (FileLoader, ConvertBufferToObject, scripts)
    for p in 02 03 04 05; do
        patch_file=$(ls /src/patches/${p}-*.patch 2>/dev/null | head -1)
        if [ -n "$patch_file" ] && [ -s "$patch_file" ]; then
            git apply "$patch_file"
            echo "  Applied $(basename "$patch_file")"
        fi
    done

    # Inject soak test (Phase 4)
    if [ -f "/src/headless_stubs/soak_test.cpp" ]; then
        echo "--- Injecting soak test ---"
        cp /src/headless_stubs/soak_test.cpp "${WORK_DIR}/source/soak_test.cpp"
        sed -i 's|list(APPEND SOURCE_FILES_LIST headless_render_stubs.cpp)|list(APPEND SOURCE_FILES_LIST headless_render_stubs.cpp)\nlist(APPEND SOURCE_FILES_LIST soak_test.cpp)|' \
            "${WORK_DIR}/source/CMakeLists.txt"
    fi

    # Inject differential test harness (Phase 4)
    if [ -f "/src/headless_stubs/differential_test.cpp" ]; then
        echo "--- Injecting differential test harness ---"
        cp /src/headless_stubs/differential_test.cpp "${WORK_DIR}/source/differential_test.cpp"
        sed -i 's|list(APPEND SOURCE_FILES_LIST soak_test.cpp)|list(APPEND SOURCE_FILES_LIST soak_test.cpp)\nlist(APPEND SOURCE_FILES_LIST differential_test.cpp)|' \
            "${WORK_DIR}/source/CMakeLists.txt"
    fi

    # Phase 4b: Scenario test framework
    if [ -f "/src/headless_stubs/game_tests.cpp" ]; then
        echo "--- Injecting game test framework ---"
        cp /src/headless_stubs/game_tests.cpp "${WORK_DIR}/source/game_tests.cpp"
        cp /src/headless_stubs/TestFramework.h "${WORK_DIR}/source/TestFramework.h"
        [ -f /src/headless_stubs/ScenarioHelpers.h ] && cp /src/headless_stubs/ScenarioHelpers.h "${WORK_DIR}/source/ScenarioHelpers.h"
        sed -i 's|list(APPEND SOURCE_FILES_LIST differential_test.cpp)|list(APPEND SOURCE_FILES_LIST differential_test.cpp)\nlist(APPEND SOURCE_FILES_LIST game_tests.cpp)|' \
            "${WORK_DIR}/source/CMakeLists.txt"
        # Add all test files from tests/ directory
        if [ -d "/src/headless_stubs/tests" ]; then
            TEST_FILES=""
            for f in /src/headless_stubs/tests/test_*.cpp; do
                [ -f "$f" ] || continue
                base=$(basename "$f")
                cp "$f" "${WORK_DIR}/source/${base}"
                TEST_FILES="${TEST_FILES}\nlist(APPEND SOURCE_FILES_LIST ${base})"
                echo "  Added test: $base"
            done
            if [ -n "$TEST_FILES" ]; then
                sed -i "s|list(APPEND SOURCE_FILES_LIST game_tests.cpp)|list(APPEND SOURCE_FILES_LIST game_tests.cpp)${TEST_FILES}|" \
                    "${WORK_DIR}/source/CMakeLists.txt"
            fi
        fi
    fi
fi

# ---- Step 2d: Dynamic path injection ----
# Tracy include path depends on Conan cache location — must stay as runtime injection
TRACY_INC=$(find /root/.conan2 -path "*/tracy*/include" -type d 2>/dev/null | head -1)
if [ -n "$TRACY_INC" ]; then
    sed -i "s|target_include_directories(\${RE_PROJECT_LIB_NAME} PRIVATE|target_include_directories(\${RE_PROJECT_LIB_NAME} PRIVATE\n    ${TRACY_INC}|" "${WORK_DIR}/source/CMakeLists.txt"
    echo "  Tracy include: $TRACY_INC"
fi

# ---- Step 3: CMake configure ----
echo ""
echo "--- CMake configure ---"
cmake --preset conan-release \
    -DGTASA_UNITY_BUILD=ON

# ---- Step 3b: Fix generated file paths for Wine ----
# CMake generates #include "/unix/path" in cmake_pch.hxx and unity_*.cxx files.
# cl.exe under Wine resolves "/" as uppercase Z:\, but wine-msvc.sh converts -I
# paths to lowercase z:\. Fix: prepend z: to all Unix #include paths.
echo ""
echo "--- Fixing paths for Wine (Z: vs z: drive letter bug) ---"
PCH_HXX=$(find "${WORK_DIR}" -name "cmake_pch.hxx" -path "*/CMakeFiles/*" 2>/dev/null | head -1)
if [ -n "$PCH_HXX" ]; then
    echo "  Patching PCH header: ${PCH_HXX}"
    sed -i 's|#include "/|#include "z:/|g' "$PCH_HXX"
fi
UNITY_COUNT=0
find "${WORK_DIR}" -name "unity_*_cxx.cxx" -path "*/Unity/*" | while read f; do
    sed -i 's|#include "/|#include "z:/|g' "$f"
done
UNITY_COUNT=$(find "${WORK_DIR}" -name "unity_*_cxx.cxx" -path "*/Unity/*" | wc -l)
echo "  Patched ${UNITY_COUNT} unity files"

# ---- Step 4: Build ----
echo ""
echo "--- Building gta_reversed.asi ---"
cmake --build --preset conan-release --parallel "$(nproc)"

# ---- Step 5: Strip TLS directory from PE ----
echo ""
echo "--- Stripping TLS directory from PE ---"
ASI_FILE=$(find "${WORK_DIR}" -name "gta_reversed.asi" | head -1)
if [ -n "$ASI_FILE" ]; then
    python3 -c "
import struct, sys
with open(sys.argv[1], 'r+b') as f:
    f.seek(0x3C)
    pe_off = struct.unpack('<I', f.read(4))[0]
    f.seek(pe_off)
    sig = f.read(4)
    assert sig == b'PE\x00\x00', f'Not a PE file: {sig}'
    coff_off = pe_off + 4
    f.seek(coff_off + 16)
    opt_size = struct.unpack('<H', f.read(2))[0]
    opt_off = coff_off + 20
    f.seek(opt_off)
    magic = struct.unpack('<H', f.read(2))[0]
    if magic == 0x10B:
        dd_off = opt_off + 96
    elif magic == 0x20B:
        dd_off = opt_off + 112
    else:
        raise ValueError(f'Unknown PE magic: {hex(magic)}')
    tls_dd_off = dd_off + 9 * 8
    f.seek(tls_dd_off)
    old_rva, old_size = struct.unpack('<II', f.read(8))
    print(f'TLS directory: RVA=0x{old_rva:X}, Size=0x{old_size:X}')
    if old_rva != 0:
        f.seek(tls_dd_off)
        f.write(struct.pack('<II', 0, 0))
        print('TLS directory zeroed.')
    else:
        print('No TLS directory to strip.')
" "$ASI_FILE"
fi

echo ""
echo "=== Build complete ==="
find "${WORK_DIR}" -name "gta_reversed.asi" -o -name "gta_reversed.dll" 2>/dev/null
if [ -d "/build" ]; then
    cp -v "${WORK_DIR}"/build/Debug/source/bin/Debug/gta_reversed.asi /build/ 2>/dev/null || \
    find "${WORK_DIR}" -name "gta_reversed.asi" -exec cp -v {} /build/ \; 2>/dev/null || \
    echo "(no .asi found yet)"
    find "${WORK_DIR}" -name "gta_reversed.map" -exec cp -v {} /build/ \; 2>/dev/null || \
    echo "(no .map file found)"
fi
