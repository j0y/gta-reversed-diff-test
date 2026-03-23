# Tools & Scripts Reference

All scripts live in `scripts/`. They run either on the host or inside the `gta-reversed-build` Docker container.

---

## Workflow

The typical workflow is:

```
1. docker-build.sh build       # Full build: Docker image + compile gta_reversed.asi (~2 min)
2. docker-build.sh build-tests # Incremental: recompile changed tests only (~6s)
3. run.sh [timeout]            # Run headless game (from host)
```

The first `build` populates a Docker volume (`gta-build-tree`) with the full build tree. Subsequent `build-tests` runs reuse it — Ninja only recompiles the unity file(s) containing changed test/stub files, then re-links.

Or for debugging/testing:

```
docker-build.sh test         # Verify MSVC toolchain
wine-crash-dump.sh           # Diagnose crashes
run-differential-test.sh     # Compare reversed vs original code
```

---

## Host Scripts

These run directly on the host machine and manage Docker containers.

### docker-build.sh

Entry point for building the Docker image and running build commands.

```bash
./scripts/docker-build.sh              # Build Docker image only
./scripts/docker-build.sh test         # Build image + run toolchain tests
./scripts/docker-build.sh build        # Build image + build gta_reversed.asi
./scripts/docker-build.sh shell        # Build image + interactive shell
```

Mounts `gta-reversed/`, `headless_stubs/`, `patches/`, and `build-output/` into the container. Output artifacts appear in `build-output/`.

### run.sh

Convenience wrapper to run the headless game in Docker.

```bash
./scripts/run.sh              # Run with default 90s timeout
./scripts/run.sh 120          # Run with custom timeout
./scripts/run.sh logs         # Dump logs from last run
```

Mounts game data, build artifacts, scripts, and configs. Copies Wine log files to `/tmp/wine-logs/` on exit and prints key logs (scripts.log, state5.log, pgl.log).

---

## Container Build Scripts

These run inside the Docker container. Invoke them via `docker run ... gta-reversed-build /scripts/<script>`.

### build.sh

Main build script for `gta_reversed.asi`. Runs inside the container with `/src/gta-reversed` (source) and `/build` (output) mounted.

**What it does (in order):**
1. Verifies MSVC C++23 toolchain
2. Copies source to writable temp dir, runs `conan install`
3. Patches CMakeLists.txt (include paths, `/ZI` removal, dynamic CRT, warnings, `/MAP` flag)
4. Fixes case-sensitive directory names (Audio subdirs)
5. Injects headless render stubs + soak test + differential test harness (if `/src/headless_stubs/` mounted)
6. Applies patches from `/src/patches/` (01 through 05)
7. Adds WinMain/ProcessGameLogic logging probes
8. Disables spdlog, TLS, Tracy, game allocator overrides, windowed mode, `/GS` stack checks
9. Runs CMake configure + build with Ninja
10. Strips TLS directory from output PE
11. Copies `gta_reversed.asi` and `gta_reversed.map` to `/build/`

**Output:** `build-output/gta_reversed.asi` (~22MB), `build-output/gta_reversed.map` (~36MB)

```bash
docker run --rm \
    -v ./gta-reversed:/src/gta-reversed:ro \
    -v ./headless_stubs:/src/headless_stubs:ro \
    -v ./patches:/src/patches:ro \
    -v conan-cache:/root/.conan2 \
    -v ./build-output:/build \
    gta-reversed-build /scripts/build.sh
```

### build-null-d3d9.sh

Builds the null D3D9 stub DLL from `null_d3d9/`. Uses the MSVC Wine toolchain with CMake + Ninja.

**Output:** `build-output/d3d9.dll` (~24KB)

```bash
docker run --rm \
    -v ./null_d3d9:/src/null_d3d9:ro \
    -v ./scripts/build-null-d3d9.sh:/scripts/build-null-d3d9.sh:ro \
    -v ./build-output:/build \
    gta-reversed-build /scripts/build-null-d3d9.sh
```

### build-test-asi.sh

Builds a minimal 9KB test ASI with zero dependencies — no game headers, no static constructors, no spdlog. Used to verify the DLL loading pipeline (dinput8.dll -> LoadLibrary -> DllMain) works independently of the full ASI.

**What it tests:**
- DllMain is called with valid `hModule`
- `__argc` / `__argv` are accessible (MSVC CRT globals)
- `GetCommandLineA()` works
- `SetTimer` / timer callbacks fire (game message loop is running)

If `C:\test_asi.log` is created with timer entries, the ASI loading pipeline works and the problem is in the full ASI's code.

**Output:** `build-output/test_asi.asi`, `build-output/test_asi.map`

```bash
docker run --rm \
    -v ./test_asi:/src/test_asi:ro \
    -v ./scripts/build-test-asi.sh:/scripts/build-test-asi.sh:ro \
    -v ./build-output:/build \
    gta-reversed-build /scripts/build-test-asi.sh
```

---

## Container Runtime Scripts

### run-headless.sh

Main orchestration script for running GTA:SA headlessly under Wine.

**What it does:**
1. Creates Wine prefix via `setup-wine-prefix.sh` (if needed)
2. Copies game data to Wine drive_c
3. Installs build artifacts (asi, d3d9.dll, MSVC runtimes, config)
4. Starts Xvfb virtual display
5. Launches `wine gta_sa.exe` with timeout
6. Dumps all Wine C:\ log files on exit

**Environment variables:**

| Variable | Default | Description |
|---|---|---|
| `GAME_DATA` | `/game` | Game data directory |
| `BUILD_DIR` | `/build` | Build artifacts directory |
| `GAME_EXE` | `/gamebin/gta_sa_compact.exe` | Game binary |
| `WINEPREFIX` | `/opt/wine-gtasa` | Wine prefix location |
| `TIMEOUT` | `60` | Seconds before killing |
| `GAME_ARGS` | `--unhook-except=Global/CLoadingScreen` | Args passed to gta_sa.exe |

**Exit codes:** `0` = game exited normally, `124` = timeout reached (success), other = crash.

### setup-wine-prefix.sh

Creates and configures a 32-bit Wine prefix. Only runs once per prefix.

**Configuration applied:**
- `WINEARCH=win32`
- Null audio driver (`HKCU\Software\Wine\Drivers\Audio = ""`)
- DLL overrides: `d3d9=native`, MSVC runtimes = `native,builtin`

### run-differential-test.sh

Orchestrates differential testing — runs the game multiple times with different hook configurations and compares state hashes.

**Test phases:**
0. **Warmup** — 3 throwaway runs to prime Wine prefix, filesystem cache, and gta_sa.set
1. **Baseline** — N runs with all hooks enabled (checks determinism)
2. **All-disabled** — all hooks off except headless whitelist (checks test sensitivity)
3. **Category-level** — disable one top-level category at a time (finds divergent categories)

**Environment variables:**

| Variable | Default | Description |
|---|---|---|
| `GAME_TIMEOUT` | `120` | Per-run timeout (seconds) |
| `WARMUP_FRAMES` | `300` | Frames before measuring |
| `MEASURE_FRAMES` | `100` | Frames to hash |
| `BASELINE_RUNS` | `3` | Number of baseline runs |
| `WARMUP_RUNS` | `3` | Number of cold-start warmup runs |

**Output:** JSON report at `/build/diff_results/differential_test_results.json`

```bash
docker run --rm \
    -v ./GTASA:/game:ro \
    -v ./build-output:/build:ro \
    -v ./gta_sa_compact.exe:/gamebin/gta_sa_compact.exe:ro \
    -v ./scripts:/scripts:ro \
    -v ./configs:/configs:ro \
    gta-reversed-build /scripts/run-differential-test.sh
```

### Running scenario tests (Phase 4b)

Run the game with `GAME_TEST_ENABLE=1` to execute in-process tests at state 9.

```bash
docker run --rm \
    -v ./GTASA:/game:ro \
    -v ./build-output:/build:ro \
    -v ./gta_sa_compact.exe:/gamebin/gta_sa_compact.exe:ro \
    -v ./scripts:/scripts:ro \
    -v ./configs:/configs:ro \
    -v /tmp/wine-logs:/tmp/wine-logs \
    -e GAME_TEST_ENABLE=1 \
    -e TIMEOUT=300 \
    gta-reversed-build bash -c '/scripts/run-headless.sh 2>&1; cp /opt/wine-gtasa/drive_c/*.log /tmp/wine-logs/ 2>/dev/null; cp /opt/wine-gtasa/drive_c/*.txt /tmp/wine-logs/ 2>/dev/null; cp /opt/wine-gtasa/drive_c/Games/GTASA/*.txt /tmp/wine-logs/ 2>/dev/null; true'
```

**Output:** `/tmp/wine-logs/game_test_results.txt` with per-test PASS/FAIL and assertion counts. The extra `cp` commands are needed because `run-headless.sh` only dumps `.log` files — the `.txt` test results must be copied out explicitly.

**Environment variables:**

| Variable | Default | Description |
|---|---|---|
| `GAME_TEST_ENABLE` | (unset) | Set to `1` to run scenario tests |
| `GAME_TEST_FILTER` | (unset) | Only run tests whose `Class/Name` contains this substring (e.g., `CStreamingLoad`) |
| `GAME_TEST_RESULTS_FILE` | `game_test_results.txt` | Output file path |

Tests are organized as per-class files in `headless_stubs/tests/test_*.cpp` (auto-discovered by build.sh). Includes behavior tests, differential tests, scenario tests with spawned game objects, streaming I/O tests, cross-vehicle-type tests (bikes, boats, helis, planes loaded via CStreaming), task system tests (86 classes), event system tests (25 classes), animation tests, and collision tests. 1845 tests across ~150 classes, 38 bugs found. Timeout of 600s is needed for the full suite (~0.3s/test due to hook toggling overhead).

To add tests for a new class, create `headless_stubs/tests/test_ClassName.cpp` with `GAME_TEST()` or `GAME_DIFF_TEST()` macros.

**Filtered runs** (faster iteration on new tests):

```bash
docker run --rm \
    -v ./GTASA:/game:ro \
    -v ./build-output:/build:ro \
    -v ./gta_sa_compact.exe:/gamebin/gta_sa_compact.exe:ro \
    -v ./scripts:/scripts:ro \
    -v ./configs:/configs:ro \
    -v /tmp/wine-logs:/tmp/wine-logs \
    -e GAME_TEST_ENABLE=1 \
    -e GAME_TEST_FILTER=CStreamingLoad \
    -e TIMEOUT=120 \
    gta-reversed-build bash -c '/scripts/run-headless.sh 2>&1; cp /opt/wine-gtasa/drive_c/*.log /tmp/wine-logs/ 2>/dev/null; cp /opt/wine-gtasa/drive_c/*.txt /tmp/wine-logs/ 2>/dev/null; cp /opt/wine-gtasa/drive_c/Games/GTASA/*.txt /tmp/wine-logs/ 2>/dev/null; true'
```

The filter matches against `ClassName/TestName` (e.g., `GAME_TEST_FILTER=CStreaming` runs all CStreaming and CStreamingLoad tests). Skipped tests are reported in the output.

---

## Debugging Tools

### wine-crash-dump.sh

All-in-one crash diagnosis. Launches the game with Wine SEH tracing enabled, captures the crash, and resolves the crashing function name from the `.map` file.

**Output includes:**
- Whether DllMain was reached
- Exception address, fault address, register dump
- ASI offset from base
- Function name from .map file (if available)
- DLL load order

**Environment variables:**

| Variable | Default | Description |
|---|---|---|
| `GAME_ARGS` | `--unhook-except=Global/CLoadingScreen` | Args for gta_sa.exe |
| `TIMEOUT` | `15` | Seconds before killing |
| `MAP_FILE` | `/build/gta_reversed.map` | Path to linker map file |

```bash
docker run --rm \
    -v ./GTASA:/game:ro \
    -v ./build-output:/build:ro \
    -v ./gta_sa_compact.exe:/gamebin/gta_sa_compact.exe:ro \
    -v ./scripts:/scripts:ro \
    -v ./configs:/configs:ro \
    gta-reversed-build /scripts/wine-crash-dump.sh
```

### resolve-crash.py

Python script that parses an MSVC `.map` file and resolves a crash address to a function name. Supports basic MSVC name demangling.

**Three modes:**

```bash
# From ASI offset directly
python3 scripts/resolve-crash.py --map build-output/gta_reversed.map --offset 0xB69018

# From absolute crash address + runtime base
python3 scripts/resolve-crash.py --map build-output/gta_reversed.map \
    --base 0x758E0000 --addr 0x76449018

# Auto-extract from Wine stderr
wine gta_sa.exe 2>&1 | python3 scripts/resolve-crash.py \
    --map build-output/gta_reversed.map --wine-stderr
```

**How the .map file is generated:** `build.sh` adds `/MAP` to the MSVC linker flags. The linker produces `gta_reversed.map` (~114K symbols) alongside the `.asi`.

**Example session:**

```
$ docker run ... gta-reversed-build /scripts/wine-crash-dump.sh

=== Crash Analysis ===
DllMain: NOT REACHED (crash during CRT init / static constructors)

Crash EIP:     0x76449018
ASI base:      0x758E0000
Fault address: 0x00000000 (read)
ASI offset:    0xB69018

--- Symbol Resolution ---
  Function: spdlog::set_pattern
  Mangled:  ?set_pattern@spdlog@@YAX...
  At:       section 1:0x00B67EE0 + 0x138
```

---

## Verification

### test-toolchain.sh

Quick sanity check that verifies the entire MSVC-under-Wine toolchain works:

1. `cl.exe` is callable
2. C++23 compilation (ranges, span, string_view, optional, variant)
3. Linking produces an exe
4. DLL creation works
5. Windows SDK headers available (d3d9.h, dinput.h, dsound.h)
6. CMake + Ninja available
7. Conan available

```bash
docker run --rm gta-reversed-build /scripts/test-toolchain.sh
```

---

## Other Files

### conanprofile-wine.txt

Conan profile for building dependencies with MSVC under Wine. Configures x86, Release, MSVC 194, C++23, dynamic CRT, Ninja generator, and disables libusb in SDL3.

---

## Log Files

The headless game writes diagnostic logs to `C:\` in the Wine prefix. After a run via `run.sh`, these are copied to `/tmp/wine-logs/`.

| File | Contents |
|---|---|
| `rw_init.log` | RenderWare init sequence trace |
| `winmain_progress.log` | WinMain milestones (SDL_Init, CreateWindow, rsRWINITIALIZE) |
| `d3d9_null.log` | All D3D9 API calls with timestamps |
| `state5.log` | State 5+ init progress (each function timestamped) |
| `loadlevel.log` | CFileLoader::LoadLevel line-by-line progress |
| `scripts.log` | CTheScripts::Process opcode logging |
| `pgl.log` | ProcessGameLogic calls (state, quit flag, loading flag) |
| `mainloop.log` | MainLoop entry proof-of-life |
| `gamestate.log` | Debug timer (gGameState every 100ms) |
| `dllmain_reached.log` | DllMain proof-of-life |
| `headless_stubs.log` | InstallHeadlessRenderStubs proof-of-life |
| `d3d9_loaded.log` | null d3d9.dll DllMain proof-of-life |
| `fileloader.log` | LoadClumpFile / LoadAtomicFile probes |

**Collecting logs after a run:**

`run.sh` copies all logs to `/tmp/wine-logs/` automatically. To collect manually:

```bash
docker run --rm ... gta-reversed-build bash -c '
    /scripts/run-headless.sh || true
    for f in /opt/wine-gtasa/drive_c/*.log; do
        [ -f "$f" ] || continue
        echo "--- $(basename $f) ---"
        cat "$f"
    done
'
```
