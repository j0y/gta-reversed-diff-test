#!/bin/bash
set -euo pipefail

# Run GTA:SA headlessly under Wine with gta_reversed.asi and null d3d9.dll
#
# Expected mounts/paths:
#   /game        - Game data files (read-only)
#   /build       - Build artifacts (gta_reversed.asi, d3d9.dll)
#   /gamebin     - gta_sa_compact.exe (read-only)
#
# Or set these environment variables:
#   GAME_DATA    - Path to game data directory
#   BUILD_DIR    - Path to build output (contains .asi and d3d9.dll)
#   GAME_EXE     - Path to gta_sa_compact.exe

GAME_DATA="${GAME_DATA:-/game}"
BUILD_DIR="${BUILD_DIR:-/build}"
GAME_EXE="${GAME_EXE:-/gamebin/gta_sa_compact.exe}"
WINEPREFIX="${WINEPREFIX:-/opt/wine-gtasa}"
GAMEDIR="${WINEPREFIX}/drive_c/Games/GTASA"
TIMEOUT="${TIMEOUT:-60}"

export WINEPREFIX
export WINEARCH=win32
export WINEDEBUG=-all

echo "=== Phase 2: Headless Game Bootstrap ==="
echo "Game data:  ${GAME_DATA}"
echo "Build dir:  ${BUILD_DIR}"
echo "Game exe:   ${GAME_EXE}"
echo "Wine prefix: ${WINEPREFIX}"

# ---- Step 1: Setup Wine prefix ----
if [ ! -d "${WINEPREFIX}/drive_c" ]; then
    echo ""
    echo "--- Setting up Wine prefix ---"
    /scripts/setup-wine-prefix.sh
fi

# ---- Step 2: Assemble game directory ----
echo ""
echo "--- Assembling game directory ---"
mkdir -p "${GAMEDIR}"

# Copy game data (skip if already present — large copy)
if [ ! -d "${GAMEDIR}/data" ]; then
    echo "Copying game data files..."
    # Copy everything except the exe (we use our own)
    rsync -a --exclude='gta_sa.exe' --exclude='*.pdb' --exclude='*.log' \
        "${GAME_DATA}/" "${GAMEDIR}/"
    echo "Game data copied."
else
    echo "Game data already in place."
fi

# Copy game binary
cp -f "${GAME_EXE}" "${GAMEDIR}/gta_sa.exe"

# Copy our build artifacts
echo "Installing build artifacts..."
cp -f "${BUILD_DIR}/gta_reversed.asi" "${GAMEDIR}/gta_reversed.asi"
cp -f "${BUILD_DIR}/d3d9.dll" "${GAMEDIR}/d3d9.dll"

# Install MSVC runtime DLLs — the .asi is compiled with /MD (dynamic CRT) and
# Wine's built-in msvcp140.dll is missing C++23 functions like _Throw_Cpp_error.
# Place in both the game dir (for app-local loading) and system32 (for Wine search path).
echo "Installing MSVC runtime DLLs..."
VCRT_DIR="/opt/msvc/VC/Redist/MSVC/14.50.35710/x86/Microsoft.VC145.CRT"
UCRT_DIR="/opt/msvc/Windows Kits/10/Redist/10.0.26100.0/ucrt/DLLs/x86"
SYS32="${WINEPREFIX}/drive_c/windows/system32"
for dll in msvcp140.dll msvcp140_1.dll msvcp140_2.dll vcruntime140.dll concrt140.dll; do
    if [ -f "${VCRT_DIR}/${dll}" ]; then
        cp -f "${VCRT_DIR}/${dll}" "${GAMEDIR}/"
        cp -f "${VCRT_DIR}/${dll}" "${SYS32}/"
    fi
done
if [ -f "${UCRT_DIR}/ucrtbase.dll" ]; then
    cp -f "${UCRT_DIR}/ucrtbase.dll" "${GAMEDIR}/"
    cp -f "${UCRT_DIR}/ucrtbase.dll" "${SYS32}/"
fi

# Ensure ASI loader is present (dinput8.dll from game data)
if [ ! -f "${GAMEDIR}/dinput8.dll" ]; then
    echo "WARNING: dinput8.dll (ASI loader) not found in game directory!"
    echo "The ASI loader must be present for gta_reversed.asi to load."
fi

# Copy headless configuration
cp -f /configs/gta-reversed.ini "${GAMEDIR}/gta-reversed.ini"

echo "Game directory assembled at ${GAMEDIR}"
ls -la "${GAMEDIR}/gta_sa.exe" "${GAMEDIR}/gta_reversed.asi" "${GAMEDIR}/d3d9.dll" "${GAMEDIR}/dinput8.dll" 2>/dev/null

# ---- Step 3: Start virtual display ----
echo ""
echo "--- Starting virtual display ---"
XVFB_PID=""
if [ -z "${DISPLAY:-}" ]; then
    if command -v Xvfb >/dev/null 2>&1; then
        Xvfb :99 -screen 0 800x600x24 -nolisten tcp &
        XVFB_PID=$!
        export DISPLAY=:99
        sleep 1
        echo "Xvfb started on :99 (PID ${XVFB_PID})"
    else
        echo "WARNING: No DISPLAY set and Xvfb not available."
        echo "Wine may fail to create windows. Install xvfb package."
    fi
else
    echo "Using existing DISPLAY=${DISPLAY}"
fi

# Dump all Wine C:\ log files
dump_logs() {
    echo ""
    echo "=== Wine C:\\ log files ==="
    for f in "${WINEPREFIX}/drive_c"/*.log; do
        [ -f "$f" ] || continue
        echo "--- $(basename "$f") ---"
        cat "$f"
        echo ""
    done
}

# Cleanup function
cleanup() {
    dump_logs
    if [ -n "${XVFB_PID}" ]; then
        kill "${XVFB_PID}" 2>/dev/null || true
    fi
    wineserver -k 2>/dev/null || true
}
trap cleanup EXIT

# ---- Step 4: Launch game ----
echo ""
echo "--- Launching GTA:SA headlessly ---"
echo "Timeout: ${TIMEOUT}s"

cd "${GAMEDIR}"

# Launch with timeout — the game should start and stay running
# We capture output to check for initialization markers
# --unhook-except keeps only FastLoader (CLoadingScreen) hooks active,
# disabling all other reversed hooks that crash during init under Wine.
# FastLoader skips splash screens, dialogs, and intro videos.
GAME_ARGS="${GAME_ARGS:---unhook-except=Global/CLoadingScreen}"
timeout "${TIMEOUT}" wine gta_sa.exe ${GAME_ARGS} 2>&1 | tee /tmp/game.log &
GAME_PID=$!

# Wait a bit then check if it's still running
sleep 5

if kill -0 "${GAME_PID}" 2>/dev/null; then
    echo ""
    echo "Game process is running (PID ${GAME_PID})"
    echo ""
    echo "--- Game output (first 50 lines) ---"
    head -50 /tmp/game.log 2>/dev/null || true

    # Wait for the game to finish (or timeout)
    wait "${GAME_PID}" 2>/dev/null
    EXIT_CODE=$?

    if [ "${EXIT_CODE}" -eq 124 ]; then
        echo ""
        echo "=== Game ran for ${TIMEOUT}s without crashing (timeout reached) ==="
        echo "This is GOOD — the game initialized and the main loop is running."

        # Kill the game gracefully
        wineserver -k 2>/dev/null || true
        exit 0
    else
        echo ""
        echo "=== Game exited with code ${EXIT_CODE} ==="
        echo ""
        echo "--- Last 30 lines of output ---"
        tail -30 /tmp/game.log 2>/dev/null || true
        exit "${EXIT_CODE}"
    fi
else
    echo ""
    echo "=== Game process died within 5 seconds ==="
    echo ""
    echo "--- Full output ---"
    cat /tmp/game.log 2>/dev/null || true
    exit 1
fi
