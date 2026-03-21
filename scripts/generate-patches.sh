#!/bin/bash
set -euo pipefail

# Generate git patch files from the sed commands in build.sh.
# Run this inside the Docker container:
#   docker run --rm -v $PWD/gta-reversed:/src/gta-reversed:ro -v $PWD/patches:/src/patches gta-reversed-build bash /scripts/generate-patches.sh

SRC_DIR="/src/gta-reversed"
WORK="/tmp/patchgen"
OUT="/src/patches"

gen_patch() {
    local name="$1"
    cd "$WORK"
    git diff > "${OUT}/${name}"
    local lines=$(wc -l < "${OUT}/${name}")
    echo "  Generated ${name} (${lines} lines)"
    git checkout . 2>/dev/null
}

# --- Patch 06: Root CMakeLists.txt flags ---
echo "=== Generating 06-cmake-root-flags.patch ==="
rm -rf "$WORK" && cp -r "$SRC_DIR" "$WORK" && cd "$WORK"
git checkout . 2>/dev/null || true

# Disable /ZI
sed -i 's|add_compile_options(/ZI)|# add_compile_options(/ZI)  # disabled for Wine build|' CMakeLists.txt
# Disable /SAFESEH:NO
sed -i 's|add_link_options(/SAFESEH:NO)|add_link_options(/MAP)  # /SAFESEH:NO disabled, /MAP added for Wine build|' CMakeLists.txt
# Dynamic CRT /MD
sed -i 's|set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded\$<\$<CONFIG:Debug>:Debug>")|set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL$<$<CONFIG:Debug>:Debug>")|' CMakeLists.txt

gen_patch "06-cmake-root-flags.patch"

# --- Patch 07: source/CMakeLists.txt flags ---
echo "=== Generating 07-cmake-source-flags.patch ==="

# Fix include dirs
sed -i 's|target_include_directories(${RE_PROJECT_LIB_NAME} PRIVATE|target_include_directories(${RE_PROJECT_LIB_NAME} PRIVATE\n    ${CMAKE_CURRENT_SOURCE_DIR}|' source/CMakeLists.txt
sed -i '/^[[:space:]]*\/$/d' source/CMakeLists.txt
# Remove /ZI
sed -i 's|/ZI;||g' source/CMakeLists.txt
# Add /MAP to target link options
sed -i 's|"/SUBSYSTEM:WINDOWS"|"/SUBSYSTEM:WINDOWS"\n    "/MAP"|' source/CMakeLists.txt
# Warnings-as-errors off
sed -i 's|COMPILE_WARNING_AS_ERROR ON|COMPILE_WARNING_AS_ERROR OFF|' source/CMakeLists.txt
sed -i 's|/wd4200|/wd4200;/wd4005|' source/CMakeLists.txt
sed -i 's|-WX||g' source/CMakeLists.txt
# NOTSA_HEADLESS define
sed -i '/EXTRA_DEBUG_FEATURES/i\    NOTSA_HEADLESS' source/CMakeLists.txt
# Disable windowed mode
sed -i 's@NOTSA_WINDOWED_MODE@NOTSA_WINDOWED_MODE_DISABLED@' source/CMakeLists.txt
# Disable /GS + add /Zc:threadSafeInit-
sed -i 's@/Zc:preprocessor@/Zc:preprocessor;/GS-;/Zc:threadSafeInit-@' source/CMakeLists.txt
# Disable Tracy
sed -i 's|TRACY_ENABLE|TRACY_ENABLE_DISABLED_FOR_WINE|g' source/CMakeLists.txt
sed -i 's|Tracy::TracyClient||' source/CMakeLists.txt
# Disable game allocator
sed -i 's|USE_GTASA_ALLOCATOR|USE_GTASA_ALLOCATOR_DISABLED|' source/CMakeLists.txt

gen_patch "07-cmake-source-flags.patch"

# --- Patch 08: Disable spdlog ---
echo "=== Generating 08-disable-spdlog.patch ==="

sed -i 's|#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE|#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_OFF|' source/app/app_debug.h
sed -i '/^notsa::Logging::Logging() {$/,/^}$/{
    /^notsa::Logging::Logging() {$/!{
        /^}$/!s|.*|// &|
    }
}' source/app/app_debug.cpp
sed -i '/^auto notsa::Logging::Create/,/^}$/{
    /^auto notsa::Logging::Create/!{
        /^}$/!s|.*|// &|
    }
}' source/app/app_debug.cpp
sed -i '/^auto notsa::Logging::Create/{
    n
    s|^// |    return nullptr; // |
}' source/app/app_debug.cpp
sed -i 's|AddVectoredExceptionHandler(1, WindowsExceptionHandler)|/* AddVectoredExceptionHandler disabled for Wine */|' source/app/app_debug.cpp

gen_patch "08-disable-spdlog.patch"

# --- Patch 09: TLS elimination ---
echo "=== Generating 09-tls-elimination.patch ==="

sed -i 's|static thread_local|static /*thread_local*/|g' source/game_sa/General.h
# Comment out operator new/delete overrides in MemoryMgr.cpp
sed -i '/^void\* operator new(size_t size) {$/,/^}$/{ s|^|// | }' source/game_sa/MemoryMgr.cpp
sed -i '/^void\* operator new\[\](size_t size) {$/,/^}$/{ s|^|// | }' source/game_sa/MemoryMgr.cpp
sed -i '/^void operator delete(void\* p) {$/,/^}$/{ s|^|// | }' source/game_sa/MemoryMgr.cpp
sed -i '/^void operator delete\[\](void\* p) {$/,/^}$/{ s|^|// | }' source/game_sa/MemoryMgr.cpp

gen_patch "09-tls-elimination.patch"

# --- Patch 10: dllmain headless wiring ---
echo "=== Generating 10-dllmain-headless-wiring.patch ==="

sed -i 's|void InjectHooksMain(HMODULE hThisDLL);|#include "reversiblehooks/RootHookCategory.h"\nvoid InjectHooksMain(HMODULE hThisDLL);\nvoid InstallHeadlessRenderStubs();|' source/dllmain.cpp
sed -i 's|ApplyCommandLineHookSettings();|ApplyCommandLineHookSettings();\n        InstallHeadlessRenderStubs();\n        /* Phase 4: Hook changes for differential testing are handled at runtime\n           by differential_test.cpp (ApplyHookConfig at state 9), not here.\n           The --unhook-except=Global/CLoadingScreen flag already sets the correct\n           init-time hook state via ApplyCommandLineHookSettings above. */|' source/dllmain.cpp
sed -i 's|if (\*(RwCamera\*\*)0xC1703C)|if (false \&\& *(RwCamera**)0xC1703C)|' source/dllmain.cpp
sed -i 's|DisplayConsole();|DisplayConsole();\n        { FILE* _f = fopen("C:\\\\dllmain_reached.log", "w"); if(_f){fprintf(_f,"DllMain reached\\n");fclose(_f);} }|' source/dllmain.cpp

gen_patch "10-dllmain-headless-wiring.patch"

# --- Patch 11: WinMain probes ---
# Must apply AFTER 01-headless-wine-runtime.patch and 05-headless-game-loop.patch
# since they modify WinMain.cpp too. Commit those first so our diff is clean.
echo "=== Generating 11-winmain-probes.patch ==="
git apply /src/patches/01-headless-wine-runtime.patch 2>/dev/null || true
git apply /src/patches/05-headless-game-loop.patch 2>/dev/null || true
git add -A && git commit -qm "prereq patches for 11" 2>/dev/null || true

# PGL logging
sed -i '/bool ProcessGameLogic(INT nCmdShow) {/a\    { static int _pgc=0; if(_pgc<50||_pgc%1000==0){char _b[128];wsprintfA(_b,"PGL call=%d state=%d quit=%d loading=%d\\r\\n",_pgc,gGameState,RsGlobal.quit?1:0,FrontEndMenuManager.m_bStartGameLoading?1:0);HANDLE _h=CreateFileA("C:\\\\pgl.log",FILE_APPEND_DATA,3,0,4,0x80,0);if(_h!=(HANDLE)-1){DWORD _w;WriteFile(_h,_b,lstrlenA(_b),\&_w,0);CloseHandle(_h);}} _pgc++; }' \
    source/app/platform/win/WinMain.cpp
# MainLoop probes
sed -i 's@MainLoop(nCmdShow);@{ HANDLE _h=CreateFileA("C:\\\\mainloop.log",FILE_APPEND_DATA,3,0,4,0x80,0); if(_h!=(HANDLE)-1){const char*_s="MainLoop about to be called\\r\\n";DWORD _w;WriteFile(_h,_s,29,\&_w,0);CloseHandle(_h);} }\n    MainLoop(nCmdShow);@' \
    source/app/platform/win/WinMain.cpp
sed -i 's@RwInitialized = true;@RwInitialized = true;\n        { HANDLE _h=CreateFileA("C:\\\\mainloop.log",FILE_APPEND_DATA,3,0,4,0x80,0); if(_h!=(HANDLE)-1){char _b[64];wsprintfA(_b,"MainLoop entered, RwInitialized set\\r\\n");DWORD _w;WriteFile(_h,_b,39,\&_w,0);CloseHandle(_h);} }@' \
    source/app/platform/win/WinMain.cpp
# WinMain progress probes
sed -i 's@if (!SDL_Init@{ FILE* _wm = fopen("C:\\\\winmain_progress.log", "a"); if(_wm){fprintf(_wm,"[%lu] Before SDL_Init\\n",GetTickCount());fclose(_wm);} }\n    if (!SDL_Init@' \
    source/app/platform/win/WinMain.cpp
sed -i 's@SDL_Window\* sdlWnd = SDL_CreateWindow@{ FILE* _wm = fopen("C:\\\\winmain_progress.log", "a"); if(_wm){fprintf(_wm,"[%lu] Before SDL_CreateWindow\\n",GetTickCount());fclose(_wm);} }\n    SDL_Window* sdlWnd = SDL_CreateWindow@' \
    source/app/platform/win/WinMain.cpp
sed -i 's@if (RsEventHandler(rsRWINITIALIZE@{ FILE* _wm = fopen("C:\\\\winmain_progress.log", "a"); if(_wm){fprintf(_wm,"[%lu] Before rsRWINITIALIZE, window=%p\\n",GetTickCount(),(void*)PSGLOBAL(window));fclose(_wm);} }\n    if (RsEventHandler(rsRWINITIALIZE@' \
    source/app/platform/win/WinMain.cpp
sed -i 's@SystemParametersInfo(SPI_SETFOREGROUNDLOCKTIMEOUT@{ FILE* _wm = fopen("C:\\\\winmain_progress.log", "w"); if(_wm){fprintf(_wm,"[%lu] NOTSA_WinMain entered\\n",GetTickCount());fclose(_wm);} }\n    SystemParametersInfo(SPI_SETFOREGROUNDLOCKTIMEOUT@' \
    source/app/platform/win/WinMain.cpp
sed -i 's@if (rsEVENTERROR == RsEventHandler(rsINITIALIZE@{ FILE* _wm = fopen("C:\\\\winmain_progress.log", "a"); if(_wm){fprintf(_wm,"[%lu] Before rsINITIALIZE\\n",GetTickCount());fclose(_wm);} }\n    if (rsEVENTERROR == RsEventHandler(rsINITIALIZE@' \
    source/app/platform/win/WinMain.cpp

gen_patch "11-winmain-probes.patch"

echo ""
echo "=== All patches generated ==="
ls -la "${OUT}"/0*.patch "${OUT}"/1*.patch
