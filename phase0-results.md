# Phase 0: Building gta-reversed on Linux

Builds the gta-reversed `.asi` DLL on Linux using real MSVC (cl.exe 19.50, C++23) running under Wine inside Docker. No Windows machine or Visual Studio installation required.

## Quick Start

```bash
# 1. Build the Docker image (~10 min first time, downloads ~3GB MSVC toolchain)
docker build -f docker/Dockerfile.build -t gta-reversed-build .

# 2. Create a persistent Conan cache (saves ~10 min on rebuilds)
docker volume create conan-cache

# 3. Build gta_reversed.asi (with headless render stubs)
docker run --rm \
    -v ./gta-reversed:/src/gta-reversed:ro \
    -v ./headless_stubs:/src/headless_stubs:ro \
    -v conan-cache:/root/.conan2 \
    -v ./build-output:/build \
    gta-reversed-build /scripts/build.sh

# Output: build-output/gta_reversed.asi (PE32 DLL, x86, ~22MB)
```

First build takes ~15 minutes (Conan builds 8 dependencies from source). Subsequent builds with cached deps take ~2 minutes.

## What's in the Docker Image

| Component | Version | Purpose |
|---|---|---|
| Ubuntu | 24.04 | Base OS |
| Wine | 9.0 (32+64 bit) | Runs MSVC tools |
| msvc-wine | latest | Downloads and wraps MSVC toolchain |
| MSVC cl.exe | 19.50 (v143) | C++23 compiler |
| Windows SDK | 10.0.26100.0 | Windows headers and import libraries |
| CMake | >=3.30 (via pip) | Build system |
| Ninja | 1.11 (from apt) | Build executor |
| Conan | >=2.0 (via pip) | C++ package manager |

## File Structure

```
/
├── docker/
│   ├── Dockerfile.build            # Docker image definition
│   ├── toolchain-msvc-wine.cmake   # CMake toolchain for cl.exe under Wine
│   ├── patch-cl-zi.sh             # Patches cl wrapper: /Zi -> /Z7
├── patches/
│   ├── 01-headless-wine-runtime.patch  # Wine compat, menu skip, audio init, probes
│   ├── 02-headless-probes.patch        # LoadLevel line-by-line logging
│   ├── 03-convert-buffer-probes.patch  # CBO probes + CreatePipe pool fix
│   ├── 04-scripts-process-probes.patch # Audio/shadow stubs + script/opcode logging
│   └── 05-headless-game-loop.patch    # Idle shortcut, ForegroundApp, frame limiter
├── scripts/
│   ├── build.sh                   # Main build script (runs inside container)
│   ├── build-null-d3d9.sh        # Builds null d3d9.dll stub
│   ├── build-test-asi.sh         # Builds minimal test ASI
│   ├── run.sh                     # Convenience run wrapper (./scripts/run.sh [timeout])
│   ├── run-headless.sh            # Full headless game launcher
│   ├── run-differential-test.sh   # Differential test orchestrator
│   ├── setup-wine-prefix.sh      # Wine prefix creation
│   ├── wine-crash-dump.sh        # Crash diagnosis tool
│   ├── resolve-crash.py          # Map file symbol resolver
│   ├── conanprofile-wine.txt      # Conan profile for MSVC-under-Wine
│   ├── test-toolchain.sh         # Toolchain verification tests
│   └── docker-build.sh           # Convenience wrapper (builds + mounts patches/stubs)
├── gta-reversed/                  # Upstream repo (mounted read-only)
├── build-output/                  # Build artifacts appear here
│   └── gta_reversed.asi
├── gta_sa_compact.exe             # Original game binary
└── GTASA/         # Game data files
```

## How It Works

### Docker Image (Dockerfile.build)

1. Installs Wine (32-bit and 64-bit) on Ubuntu 24.04
2. Clones [msvc-wine](https://github.com/mstorsjo/msvc-wine) and downloads the full MSVC 2022+ Build Tools + Windows SDK (~3GB)
3. Runs `install.sh` to create Wine wrapper scripts for cl.exe, link.exe, etc.
4. Patches the cl wrapper to replace `/Zi` with `/Z7` (see workarounds below)
5. Installs CMake 4.2, Conan 2, and Ninja

### Build Script (build.sh)

The build script runs inside the container and does:

1. **Verify toolchain** — compiles a C++23 test file
2. **Copy source** — copies gta-reversed to a writable temp directory
3. **Conan install** — fetches and builds 8 dependencies (imgui, spdlog, tracy, SDL3, ogg, vorbis, libjpeg-turbo, nlohmann_json)
4. **Patch CMakeLists** — applies Wine-specific fixes (see below)
5. **Fix case-sensitivity** — renames lowercase Audio subdirs to TitleCase to match CMakeLists.txt
6. **Inject headless stubs** — copies render stubs into source and patches dllmain.cpp (if headless_stubs/ is mounted)
7. **Eliminate TLS/allocator issues** — disables Tracy linking, thread_local, game allocator overrides
8. **CMake configure** — uses Conan-generated preset with our toolchain, PCH and unity build enabled
9. **Fix Wine Z:/z: paths** — patches cmake_pch.hxx and unity files to use `z:` prefix
10. **Build** — compiles 66 translation units (1 PCH + 64 unity + 1 link) and links `gta_reversed.asi`
11. **Strip TLS directory** — zeroes the PE TLS data directory for Wine compatibility

### CMake Toolchain (toolchain-msvc-wine.cmake)

Tells CMake how to use MSVC through Wine:
- Sets compiler ID/version (reports as MSVC 19.40 since CMake doesn't know 19.50 yet)
- Points to MSVC and Windows SDK include/lib directories
- Skips ABI detection (TryCompile has path issues under Wine)
- Sets `CMAKE_SIZEOF_VOID_P=4` (32-bit target)

### Conan Profile (conanprofile-wine.txt)

```ini
[settings]
arch=x86
build_type=Release
compiler=msvc
compiler.cppstd=23
compiler.runtime=dynamic
compiler.version=194
os=Windows

[options]
sdl/*:libusb=False       # libusb uses MSBuild which we don't have

[conf]
tools.cmake.cmaketoolchain:generator=Ninja
tools.cmake.cmaketoolchain:user_toolchain=["/opt/toolchain-msvc-wine.cmake"]
tools.build:compiler_executables={"c": "/opt/msvc/bin/x86/cl", "cpp": "/opt/msvc/bin/x86/cl"}
```

## Workarounds Applied

The build requires several patches to the gta-reversed CMakeLists.txt files. These are applied automatically by `build.sh` at build time (the upstream repo is never modified).

### 1. Include Path Fix

**Problem:** The project uses `"/"` as an include directory, which on Windows means the source root but on Linux means the filesystem root.

**Fix:** Replace with `${CMAKE_CURRENT_SOURCE_DIR}` and remove the bare `/` entry.

### 2. /ZI Removal

**Problem:** `/ZI` (Edit and Continue debug info) requires mspdbsrv.exe which doesn't work under Wine for parallel builds.

**Fix:** Remove `/ZI` from both `CMakeLists.txt` (root) and `source/CMakeLists.txt`. The `/Zi` flag from CMake defaults is separately handled by the cl wrapper patch.

### 3. /Zi -> /Z7 Patch (patch-cl-zi.sh)

**Problem:** MSVC's `/Zi` flag uses `mspdbsrv.exe` to manage PDB files. This service doesn't work correctly under Wine when multiple cl.exe instances compile in parallel, causing `C1902: Program database manager mismatch`.

**Fix:** Patch the msvc-wine cl wrapper script directly to replace `/Zi` with `/Z7` and strip `/Fd` and `/FS` flags. `/Z7` embeds debug info directly in `.obj` files without needing a PDB server.

### 4. Wine Z:/z: Drive Letter Fix (PCH + Unity)

**Problem:** CMake generates `#include "/unix/path"` in `cmake_pch.hxx` and `unity_*.cxx` files. cl.exe under Wine resolves bare `/` paths as uppercase `Z:\`, but wine-msvc.sh converts `-I` argument paths to lowercase `z:\`. MSVC under Wine treats `Z:` and `z:` as different drives for include path search, so headers included from PCH/unity files can't be found via the `-I` paths.

**Fix:** After cmake configure, `sed` all `#include "/` to `#include "z:/` in `cmake_pch.hxx` and all `unity_*_cxx.cxx` files.

### 5. Case-Sensitivity Fix (Audio directories)

**Problem:** CMakeLists.txt references `Audio/Hardware`, `Audio/Loaders`, `Audio/Config`, `Audio/Entities`, `Audio/Managers` (TitleCase) but the actual directories on the Linux filesystem are lowercase (`hardware`, `loaders`, etc.). Windows is case-insensitive so this works natively but fails on Linux.

**Fix:** Rename the lowercase directories to TitleCase in the copied source tree (`mv hardware Hardware`, etc.).

### 6. Dynamic CRT (/MD)

**Problem:** The project defaults to static CRT (`/MT`) but Conan dependencies are built with dynamic CRT (`/MD`). Mixing causes linker errors.

**Fix:** Override `CMAKE_MSVC_RUNTIME_LIBRARY` to `MultiThreadedDLL` to match the Conan profile's `compiler.runtime=dynamic`.

### 7. operator new/delete Override Removal

**Problem:** `MemoryMgr.cpp` has unconditional (not behind `#ifdef`) global `operator new(size_t)` and `operator delete(void*)` overrides that call game functions at hardcoded addresses via `plugin::CallAndReturn<0x82119A, ...>()`. During `DLL_PROCESS_ATTACH`, the game's heap isn't initialized yet, so any allocation during static constructors calls `RtlAllocateHeap(NULL)` → access violation at address `0x460`.

**Fix:** Comment out these unconditional overrides in the copied source. The `#ifdef MEMORY_MGR_OVERLOAD_NEW_DELETE` guarded overrides above them are already inactive.

### 8. TLS Elimination

**Problem:** Several linked libraries (TracyClient.lib, spdlog.lib, etc.) contain `__declspec(thread)` / `thread_local` variables that generate a PE `.tls` section. Wine has limitations with TLS for DLLs loaded at runtime via `LoadLibrary` (the ASI loader).

**Fix:** Multi-pronged:
- Unlink `Tracy::TracyClient` (keep include path only, disable `TRACY_ENABLE` define)
- Replace `thread_local` with `static` in `General.h` RNG
- Add `/Zc:threadSafeInit-` to disable magic statics TLS
- Zero the TLS data directory in the PE after linking (Python post-processing)

### 9. Warnings-as-Errors Disabled

**Problem:** `NOMINMAX` macro redefinition (C4005) and other minor warnings become errors under `-WX`.

**Fix:** Disable `COMPILE_WARNING_AS_ERROR`, remove `-WX`, and suppress C4005.

### 10. libusb Disabled in SDL3

**Problem:** The libusb Conan recipe uses MSBuild (`.vcxproj`) which requires `msbuild.exe` — not available in our environment.

**Fix:** Set `sdl/*:libusb=False` in the Conan profile. USB input devices aren't needed for headless testing.

## Toolchain Verification

Run the verification script to confirm cl.exe, link.exe, Windows SDK, and all tools work:

```bash
docker run --rm gta-reversed-build /scripts/test-toolchain.sh
```

## Updating gta-reversed

The upstream repo is mounted read-only. To update:

```bash
cd gta-reversed
git pull
# Then rebuild — the Docker image doesn't need to change
```

## Known Limitations

- **Build type:** Currently Release only. Debug builds would need a separate Conan profile.
- **No incremental builds:** Each `docker run` rebuilds from scratch (source is copied fresh). Could be improved with persistent build directory.
- **Image size:** ~11GB due to full MSVC toolchain + Wine + 32-bit libraries.
- **Game allocator disabled:** `operator new`/`delete` use the standard CRT heap instead of the game's allocator. This is fine for headless testing but means memory behavior differs slightly from the native game.
- **Tracy disabled:** Profiling is not available in the Wine build.
- **spdlog disabled:** All `NOTSA_LOG_*` macros compile to no-ops. spdlog uses TLS and thread-safe statics internally, both broken by Wine compatibility patches (`/Zc:threadSafeInit-` and TLS directory zeroing). The `Logging` constructor is gutted and `Logging::Create()` returns nullptr.
- **MSVC runtime DLLs required:** Wine's built-in `msvcp140.dll` is missing C++23 functions. Real Microsoft runtime DLLs must be copied to the Wine prefix's `system32` directory (handled by `run-headless.sh`).
- **Linker map file:** Build now generates `gta_reversed.map` alongside the `.asi` via `/MAP` linker flag. Contains ~114K symbols for crash debugging.
- **SDL3 input init disabled:** `SDL_Init` flags are zeroed to skip gamepad/joystick/haptic — these hang under Wine.
- **CheckDirectSound bypassed:** `DirectSoundCreate` hangs under Wine null audio driver. Patched to return TRUE.
- **GetVideoMemInfo bypassed:** `DirectDrawCreateEx` may fail under Wine. Patched to return 256MB.
- **build.sh must be mounted:** The Docker image's built-in `/scripts/build.sh` may be outdated. Always mount from host: `-v ./scripts/build.sh:/scripts/build.sh:ro`.
