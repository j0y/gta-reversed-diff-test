#!/bin/bash
set -euo pipefail

# Create and configure a 32-bit Wine prefix for headless GTA:SA
# This only needs to run once per prefix.

WINEPREFIX="${WINEPREFIX:-/opt/wine-gtasa}"
export WINEPREFIX
export WINEARCH=win32
export WINEDEBUG=-all

echo "=== Setting up Wine prefix at ${WINEPREFIX} ==="

if [ -d "${WINEPREFIX}" ]; then
    echo "Prefix already exists, skipping creation."
else
    echo "Creating 32-bit Wine prefix..."
    wineboot --init
    wineserver --wait
    echo "Wine prefix created."
fi

# Disable audio — DirectSoundCreate needs to succeed but we don't need real audio
echo "Configuring null audio driver..."
wine reg add "HKCU\Software\Wine\Drivers" /v Audio /d "" /f 2>/dev/null

# Set DLL overrides: use native DLLs instead of Wine's built-in stubs
echo "Setting DLL overrides..."
wine reg add "HKCU\Software\Wine\DllOverrides" /v d3d9 /d native /f 2>/dev/null
# MSVC C++ runtime — Wine's stubs are missing C++23 functions.
# Use "native,builtin" so Wine tries native first, falls back to builtin.
wine reg add "HKCU\Software\Wine\DllOverrides" /v msvcp140 /d "native,builtin" /f 2>/dev/null
wine reg add "HKCU\Software\Wine\DllOverrides" /v msvcp140_1 /d "native,builtin" /f 2>/dev/null
wine reg add "HKCU\Software\Wine\DllOverrides" /v msvcp140_2 /d "native,builtin" /f 2>/dev/null
wine reg add "HKCU\Software\Wine\DllOverrides" /v vcruntime140 /d "native,builtin" /f 2>/dev/null
wine reg add "HKCU\Software\Wine\DllOverrides" /v concrt140 /d "native,builtin" /f 2>/dev/null
wine reg add "HKCU\Software\Wine\DllOverrides" /v ucrtbase /d "native,builtin" /f 2>/dev/null

wineserver --wait
echo "=== Wine prefix setup complete ==="
