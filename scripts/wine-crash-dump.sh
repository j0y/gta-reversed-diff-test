#!/bin/bash
set -euo pipefail

# wine-crash-dump.sh — Run game under Wine, capture crash, resolve to function name.
#
# Usage (inside Docker):
#   /scripts/wine-crash-dump.sh
#
# Usage (from host):
#   docker run --rm \
#       -v ./GTASA:/game:ro \
#       -v ./build-output:/build:ro \
#       -v ./gta_sa_compact.exe:/gamebin/gta_sa_compact.exe:ro \
#       -v ./scripts:/scripts:ro \
#       -v ./configs:/configs:ro \
#       gta-reversed-build /scripts/wine-crash-dump.sh
#
# Env vars:
#   GAME_ARGS    — extra args for gta_sa.exe (default: --unhook-except=Global/CLoadingScreen)
#   TIMEOUT      — seconds to wait (default: 15)
#   MAP_FILE     — path to .map file (default: /build/gta_reversed.map)

WINEPREFIX="${WINEPREFIX:-/opt/wine-gtasa}"
GAMEDIR="${WINEPREFIX}/drive_c/Games/GTASA"
SYS32="${WINEPREFIX}/drive_c/windows/system32"
TIMEOUT="${TIMEOUT:-15}"
MAP_FILE="${MAP_FILE:-/build/gta_reversed.map}"
GAME_ARGS="${GAME_ARGS:---unhook-except=Global/CLoadingScreen}"
VCRT_DIR="/opt/msvc/VC/Redist/MSVC/14.50.35710/x86/Microsoft.VC145.CRT"
UCRT_DIR="/opt/msvc/Windows Kits/10/Redist/10.0.26100.0/ucrt/DLLs/x86"

export WINEPREFIX WINEARCH=win32

echo "=== Wine Crash Dump Tool ==="
echo "Timeout: ${TIMEOUT}s"
echo "Game args: ${GAME_ARGS}"
echo "Map file: ${MAP_FILE}"

# ---- Setup Wine prefix ----
if [ ! -d "${WINEPREFIX}/drive_c" ]; then
    echo "--- Setting up Wine prefix ---"
    WINEDEBUG=-all wineboot --init 2>/dev/null
    wineserver --wait
    WINEDEBUG=-all wine reg add "HKCU\Software\Wine\Drivers" /v Audio /d "" /f 2>/dev/null
    WINEDEBUG=-all wine reg add "HKCU\Software\Wine\DllOverrides" /v d3d9 /d native /f 2>/dev/null
    WINEDEBUG=-all wine reg add "HKCU\Software\Wine\DllOverrides" /v msvcp140 /d "native,builtin" /f 2>/dev/null
    WINEDEBUG=-all wine reg add "HKCU\Software\Wine\DllOverrides" /v msvcp140_1 /d "native,builtin" /f 2>/dev/null
    WINEDEBUG=-all wine reg add "HKCU\Software\Wine\DllOverrides" /v msvcp140_2 /d "native,builtin" /f 2>/dev/null
    WINEDEBUG=-all wine reg add "HKCU\Software\Wine\DllOverrides" /v vcruntime140 /d "native,builtin" /f 2>/dev/null
    WINEDEBUG=-all wine reg add "HKCU\Software\Wine\DllOverrides" /v concrt140 /d "native,builtin" /f 2>/dev/null
    WINEDEBUG=-all wine reg add "HKCU\Software\Wine\DllOverrides" /v ucrtbase /d "native,builtin" /f 2>/dev/null
    wineserver --wait
fi

# ---- Assemble game directory ----
echo "--- Assembling game directory ---"
mkdir -p "${GAMEDIR}"
if [ ! -d "${GAMEDIR}/data" ]; then
    rsync -a /game/ "${GAMEDIR}/" 2>/dev/null || true
fi
cp -f /gamebin/gta_sa_compact.exe "${GAMEDIR}/gta_sa.exe" 2>/dev/null || true
cp -f /build/gta_reversed.asi "${GAMEDIR}/" 2>/dev/null || true
cp -f /build/d3d9.dll "${GAMEDIR}/" 2>/dev/null || true
cp -f /configs/gta-reversed.ini "${GAMEDIR}/" 2>/dev/null || true

# Install MSVC runtime DLLs
for dll in msvcp140.dll msvcp140_1.dll msvcp140_2.dll vcruntime140.dll concrt140.dll; do
    [ -f "${VCRT_DIR}/${dll}" ] && cp -f "${VCRT_DIR}/${dll}" "${GAMEDIR}/" && cp -f "${VCRT_DIR}/${dll}" "${SYS32}/"
done
[ -f "${UCRT_DIR}/ucrtbase.dll" ] && cp -f "${UCRT_DIR}/ucrtbase.dll" "${GAMEDIR}/" && cp -f "${UCRT_DIR}/ucrtbase.dll" "${SYS32}/"

# ---- Start virtual display ----
if ! pgrep -x Xvfb > /dev/null 2>&1; then
    Xvfb :99 -screen 0 800x600x24 &>/dev/null &
    sleep 1
fi
export DISPLAY=:99

# ---- Run game with crash-relevant debug channels ----
echo ""
echo "--- Running game (capturing crash info) ---"
cd "${GAMEDIR}"

export WINEDEBUG="trace+seh,trace+module"
CRASH_LOG="/tmp/wine_crash.log"

timeout "${TIMEOUT}" wine gta_sa.exe ${GAME_ARGS} 2>"${CRASH_LOG}" &
GAME_PID=$!
wait "${GAME_PID}" 2>/dev/null || true

wineserver -k 2>/dev/null || true

# ---- Analyze crash ----
echo ""
echo "=== Crash Analysis ==="

# Check if DllMain was reached
if [ -f "${GAMEDIR}/dllmain_reached.log" ]; then
    echo "DllMain: REACHED"
    cat "${GAMEDIR}/dllmain_reached.log"
else
    echo "DllMain: NOT REACHED (crash during CRT init / static constructors)"
fi

# Check hooks.csv
if [ -f "${GAMEDIR}/hooks.csv" ]; then
    HOOK_COUNT=$(wc -l < "${GAMEDIR}/hooks.csv")
    echo "hooks.csv: ${HOOK_COUNT} entries (InjectHooksMain completed)"
else
    echo "hooks.csv: NOT CREATED"
fi

echo ""

# Extract crash info
CRASH_ADDR=$(grep "0024.*dispatch_exception.*code=c0000005" "${CRASH_LOG}" 2>/dev/null | head -1)
if [ -z "${CRASH_ADDR}" ]; then
    CRASH_ADDR=$(grep "page fault" "${CRASH_LOG}" 2>/dev/null | head -1)
fi

if [ -n "${CRASH_ADDR}" ]; then
    echo "--- Exception ---"
    # Show the exception and register dump
    grep "0024.*dispatch_exception\|0024.*except_handler" "${CRASH_LOG}" | head -10
    echo ""

    # Extract EIP from seh trace
    EIP=$(grep "0024.*dispatch_exception.*code=c0000005.*addr=" "${CRASH_LOG}" | head -1 | sed 's/.*addr=\([0-9a-fA-F]*\).*/\1/')
    if [ -z "$EIP" ]; then
        EIP=$(grep "0024.*dispatch_exception.*ip=" "${CRASH_LOG}" | head -1 | sed 's/.*ip=\([0-9a-fA-F]*\).*/\1/')
    fi

    # Extract ASI base address
    ASI_BASE=$(grep "0024.*Loaded module.*gta_reversed\.asi.*at " "${CRASH_LOG}" | head -1 | sed 's/.*at \([0-9a-fA-F]*\).*/\1/')

    # Extract fault address (what was accessed)
    FAULT_ADDR=$(grep "0024.*dispatch_exception.*info\[1\]=" "${CRASH_LOG}" | head -1 | sed 's/.*info\[1\]=\([0-9a-fA-F]*\).*/\1/')
    FAULT_TYPE=$(grep "0024.*dispatch_exception.*info\[0\]=" "${CRASH_LOG}" | head -1 | sed 's/.*info\[0\]=\([0-9a-fA-F]*\).*/\1/')

    echo "Crash EIP:     0x${EIP:-unknown}"
    echo "ASI base:      0x${ASI_BASE:-unknown}"
    echo "Fault address: 0x${FAULT_ADDR:-unknown} (${FAULT_TYPE:-?}: 0=read, 1=write)"

    if [ -n "$EIP" ] && [ -n "$ASI_BASE" ]; then
        OFFSET=$((0x${EIP} - 0x${ASI_BASE}))
        printf "ASI offset:    0x%X\n" "${OFFSET}"

        # Check if crash is inside the ASI
        if [ "${OFFSET}" -gt 0 ] && [ "${OFFSET}" -lt 33554432 ]; then  # < 32MB
            echo ""
            echo "--- Symbol Resolution ---"
            if [ -f "${MAP_FILE}" ]; then
                python3 /scripts/resolve-crash.py \
                    --map "${MAP_FILE}" \
                    --offset "$(printf '0x%X' ${OFFSET})" 2>&1
            else
                echo "No .map file at ${MAP_FILE}"
                echo "Rebuild with /MAP flag to generate it."
                printf "Manual lookup: offset 0x%X in gta_reversed.asi\n" "${OFFSET}"
            fi
        else
            echo ""
            echo "Crash is OUTSIDE the ASI address range — likely in a Wine/system DLL"
            # Try to find which module
            echo ""
            echo "--- Module Map (thread 0024) ---"
            grep "0024.*Loaded module" "${CRASH_LOG}" | while read line; do
                MOD_ADDR=$(echo "$line" | sed 's/.* at \([0-9a-fA-F]*\).*/\1/')
                MOD_NAME=$(echo "$line" | sed 's/.*Loaded module L"\(.*\)".*/\1/')
                printf "  0x%s  %s\n" "${MOD_ADDR}" "${MOD_NAME}"
            done
        fi
    fi

    echo ""
    echo "--- Register State ---"
    grep "0024.*eax=\|0024.*ebp=" "${CRASH_LOG}" | head -2

    echo ""
    echo "--- Exception Handlers ---"
    grep "0024.*except_handler" "${CRASH_LOG}" | head -5
else
    echo "No crash detected in ${TIMEOUT}s — game may have run successfully"
    echo ""
    echo "--- Last module activity ---"
    grep "0024.*process_attach" "${CRASH_LOG}" | tail -5
fi

echo ""
echo "--- DLL Load Order (thread 0024) ---"
grep "0024.*process_attach.*START\|0024.*process_attach.*END\|0024.*process_attach.*FAIL" "${CRASH_LOG}" | tail -20

echo ""
echo "Full log: ${CRASH_LOG} ($(wc -l < "${CRASH_LOG}") lines)"
