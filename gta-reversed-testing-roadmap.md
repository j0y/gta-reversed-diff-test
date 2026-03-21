# gta-reversed: Headless Linux Testing Infrastructure

## Goal

Build a reproducible, headless testing environment on Linux that can differentially test every reversed function in gta-reversed against the original GTA SA binary — without a GPU or display.

## Architecture Overview

```
┌─────────────────────────────────────────────────────┐
│  Docker Container (Linux)                           │
│                                                     │
│  ┌─────────────┐   ┌────────────────────────────┐  │
│  │ msvc-wine   │   │ Wine 32-bit prefix         │  │
│  │ (build)     │   │                            │  │
│  │ cl.exe v143 │   │  gta_sa.exe (original)     │  │
│  │ /std:c++23  │   │  ├── dinput8.dll (ASI ldr) │  │
│  │ link.exe    │   │  ├── gta_reversed.asi      │  │
│  └──────┬──────┘   │  ├── d3d9.dll (null stub)  │  │
│         │          │  └── game data files        │  │
│         ▼          │                            │  │
│  gta_reversed.asi  │  No GPU. No display.       │  │
│  d3d9.dll (stub)   │  Game logic runs at full   │  │
│                    │  speed. Rendering no-ops.   │  │
│                    └────────────────────────────┘  │
│                                                     │
│  Test harness (inside .asi):                        │
│  For each function:                                 │
│    1. Snapshot game state                           │
│    2. Call ORIGINAL (hook disabled)                  │
│    3. Snapshot result                               │
│    4. Restore state                                 │
│    5. Call REVERSED (hook enabled)                   │
│    6. Snapshot result                               │
│    7. Compare                                       │
└─────────────────────────────────────────────────────┘
```

---

## Current Status (2026-03-18)

| Phase | Status |
|---|---|
| **Phase 0**: Docker Build | ✅ Complete — MSVC 19.50, C++23, builds in ~2 min |
| **Phase 1**: Null D3D9 | ✅ Complete — all RW init passes, textures/surfaces stubbed |
| **Phase 2**: Headless Bootstrap | ✅ Complete — game reaches IDLE state (state 9), 24 blockers fixed total |
| **Phase 3**: RW Render Stubs | ✅ Complete — render functions no-oped, game logic runs |
| **Phase 4**: Differential Harness | ✅ Complete — deterministic baselines, all-disabled works, hooks.csv collected, 29 categories tested |
| **Phase 4b**: Scenario Tests | ✅ Working — 844 tests, ~15000 assertions, 95 classes, 75 test files, 23 bugs found. CStreaming loads arbitrary models; cross-type tests cover bike/boat/heli/plane/bmx/trailer |
| **Phase 5**: CI Pipeline | Not started |

---

## Phase 0: Docker Build Environment

**Goal:** MSVC C++23 compilation on Linux, fully reproducible.

### Steps

1. **Base Docker image**
   - Ubuntu 22.04+ base
   - Install Wine 32-bit, Python 3, CMake, Ninja

2. **Install MSVC via msvc-wine**
   ```bash
   git clone https://github.com/mstorsjo/msvc-wine
   python3 vsdownload.py --accept-license --dest /opt/msvc
   ./install.sh /opt/msvc
   ```
   This downloads the real MSVC 2022 (v143) toolchain: `cl.exe`, `link.exe`, STL headers, Windows SDK.

3. **Verify C++23 compilation**
   ```bash
   # Test that cl.exe works under Wine with C++23
   echo '#include <ranges>' > test.cpp
   echo '#include <span>' >> test.cpp
   echo 'int main() {}' >> test.cpp
   /opt/msvc/bin/x86/cl.exe /std:c++latest /EHsc test.cpp
   ```

4. **Build gta-reversed**
   ```bash
   cd /src/gta-reversed
   cmake -G Ninja -B build \
     -DCMAKE_C_COMPILER=/opt/msvc/bin/x86/cl.exe \
     -DCMAKE_CXX_COMPILER=/opt/msvc/bin/x86/cl.exe \
     -DCMAKE_LINKER=/opt/msvc/bin/x86/link.exe
   cmake --build build
   # Output: build/gta_reversed.asi
   ```

### Deliverables
- `Dockerfile` that produces a working MSVC C++23 build environment
- CI script that builds `gta_reversed.asi` from source
- Build verified by compiling the full project

### Risks
- msvc-wine may need patches for specific MSVC intrinsics or C++23 edge cases
- CMakeLists.txt may assume native Windows paths — may need minor fixups for the Wine-hosted cl.exe
- The project's precompiled header setup may behave differently under Wine

---

## Phase 1: Null D3D9 Device

**Goal:** A `d3d9.dll` stub that satisfies RenderWare initialization without touching any GPU.

### What It Does

The original game calls `LoadLibrary("D3D9.DLL")` → `Direct3DCreate9()` → `IDirect3D9::CreateDevice()`. RenderWare then uses the device to create textures, surfaces, render targets, etc. Our stub provides a fake device where every method succeeds but does nothing.

### COM Interfaces to Implement

| Interface | Method Count | Notes |
|---|---|---|
| `IDirect3D9` | ~20 | `CreateDevice` returns our null device |
| `IDirect3DDevice9` | ~120 | All render calls → `S_OK` / no-op |
| `IDirect3DTexture9` | ~30 | `LockRect` returns valid dummy buffer |
| `IDirect3DSurface9` | ~15 | `LockRect` returns valid dummy buffer |
| `IDirect3DVertexBuffer9` | ~15 | `Lock` returns valid dummy buffer |
| `IDirect3DIndexBuffer9` | ~15 | `Lock` returns valid dummy buffer |
| `IDirect3DVertexShader9` | ~5 | Dummy handle |
| `IDirect3DPixelShader9` | ~5 | Dummy handle |
| `IDirect3DStateBlock9` | ~5 | No-op |
| `IDirect3DSwapChain9` | ~10 | `Present` → `S_OK` |

### Key Implementation Details

- **`CreateDevice`** must return a non-NULL device — RenderWare checks this.
- **`CreateTexture`/`CreateSurface`** must return objects with valid `LockRect` — RenderWare writes texture data into locked surfaces during asset loading. Allocate real memory buffers (the data just goes nowhere).
- **`GetDeviceCaps`** must return a plausible `D3DCAPS9` struct — RenderWare queries caps to decide code paths. Fill with reasonable defaults (max texture size 4096, support for common formats, etc.).
- **`GetDisplayMode`** must return a valid resolution (e.g., 800x600 D3DFMT_X8R8G8B8).
- **Reference counting** (`AddRef`/`Release`) must work correctly — RenderWare tracks object lifetimes.
- **`Present`** is a no-op — this is where frames would go to the screen.
- **`IDirect3D9::GetAdapterCount`** must return at least 1.

### Steps

1. **Create `null_d3d9/` directory** in the project
2. **Implement the COM interfaces** — tedious but mechanical. Each method signature comes from `d3d9.h` in the Windows SDK. Most return `S_OK`. Methods that return objects (`CreateTexture`, etc.) allocate a dummy.
3. **Export `Direct3DCreate9`** — the only exported function. Returns our `NullDirect3D9` object.
4. **Compile to `d3d9.dll`** with MSVC (same Docker build environment)
5. **Test**: Place in game directory, run under Wine, verify game reaches main menu init without crashing

### Prior Art

Look for existing null/stub D3D9 implementations:
- Headless game server projects (e.g., SA-MP server attempts)
- D3D9 proxy/wrapper DLLs (reshade, ENB have the COM scaffolding)
- Wine's own d3d9 implementation (for interface signatures)

### Deliverables
- `d3d9.dll` (Windows x86) that implements all required interfaces as no-ops
- Verified: RenderWare initializes successfully with this DLL under Wine

### Risks
- RenderWare may call D3D9 methods in unexpected sequences during init
- Some methods may need to return specific non-trivial values (shader constants, render target surfaces)
- Debugging will require iterative testing — RW crashes → find which method needs a better stub → fix → repeat
- Estimated 3-5 iterations to get through full RW initialization

---

## Phase 2: Headless Game Bootstrap

**Goal:** GTA SA starts under Wine, loads assets, reaches a playable state, all without a display.

### Wine Setup

```bash
# Create 32-bit Wine prefix
WINEPREFIX=/opt/wine-gtasa WINEARCH=win32 wineboot --init

# Game directory
GAMEDIR="$WINEPREFIX/drive_c/Games/GTASA"
mkdir -p "$GAMEDIR"

# Copy game files (user-supplied, hash-verified)
cp /mnt/game/* "$GAMEDIR/"

# Verify binary
sha256sum "$GAMEDIR/gta_sa.exe"
# Must match: <known hash of GTA SA Compact exe, 5,189,632 bytes>

# Install our components
cp build/gta_reversed.asi "$GAMEDIR/"
cp build/d3d9.dll "$GAMEDIR/"
cp contrib/dinput8.dll "$GAMEDIR/"    # ASI loader
```

### Running Headless

```bash
export WINEPREFIX=/opt/wine-gtasa
export WINEARCH=win32
export WINEDLLOVERRIDES="d3d9=n"   # use our native d3d9.dll, not Wine's
unset DISPLAY                       # no X server

wine "$GAMEDIR/gta_sa.exe" 2>&1 | tee game.log
```

### Game Init Hooks (in the .asi)

The following must be hooked to get past startup without user interaction:

| What | Why | Hook Strategy |
|---|---|---|
| **Video mode selection dialog** | Blocks on MessageBox | Hook `CreateVidModeSelectDialog` → return default 800x600 |
| **Intro videos** | Blocks waiting for video playback | Hook `VideoPlayer::Play` → return immediately |
| **Menu screen** | Game starts at main menu, waits for input | Hook to auto-trigger "New Game" or load a save |
| **DirectInput init** | May fail without input devices | Hook `DirectInput8Create` → return stub or skip |
| **DirectSound init** | May fail without audio device | Hook `DirectSoundCreate` → return stub, or use Wine's null audio driver |
| **Window creation** | `CreateWindowEx` needs to succeed | Wine handles this even without DISPLAY (virtual desktop mode) |

### Wine Audio

```bash
# Configure Wine to use null audio driver
winetricks sound=disabled
# Or set in registry:
wine reg add "HKCU\Software\Wine\Drivers" /v Audio /d "" /f
```

### Deliverables
- Shell scripts / Docker commands to set up Wine prefix and game directory
- Hooks in the .asi that skip video mode dialog, intro videos, and auto-start gameplay
- Game process starts, initializes, and reaches a running game state under Wine
- Verified by log output: "Game initialized successfully" or similar markers

### Risks
- Wine's `CreateWindowEx` without DISPLAY may need `Xvfb` as fallback
- DirectInput initialization may crash before ASI hooks are installed (ASI loader loads DLLs after game init starts)
- Game settings file (`gta_sa.set`) must be pre-configured (resolution, audio settings) to avoid dialogs
- Some anti-piracy checks in the original exe may behave differently under Wine

---

## Phase 3: RenderWare Render Stubs (in .asi)

**Goal:** Even with the null D3D9 device, some rendering code may dereference device pointers or call into RW rendering subsystems. Belt-and-suspenders: hook the RW render functions to no-op as well.

### Hooks to Install

All hooks use the project's existing `RH_ScopedInstall` mechanism at known addresses:

```cpp
// Stub implementations
RwCamera* StubReturnCamera(RwCamera* camera, ...) { return camera; }
RwRaster* StubReturnRaster(RwRaster* raster, ...) { return raster; }
RwBool    StubReturnTrue(...)                      { return TRUE; }
void      StubNoOp(...)                            {}
```

| Function | Address | Stub |
|---|---|---|
| `RwCameraBeginUpdate` | `0x7EE190` | Return camera |
| `RwCameraEndUpdate` | `0x7EE180` | Return camera |
| `RwCameraClear` | `0x7EE340` | Return camera |
| `RwCameraShowRaster` | `0x7EE370` | Return camera |
| `RwRasterShowRaster` | `0x7FB1A0` | Return raster |
| `RsCameraBeginUpdate` | `0x619450` | Return TRUE |
| `RsCameraShowRaster` | `0x619440` | Return camera |
| `psCameraBeginUpdate` | `0x745210` | Return TRUE |
| `psCameraShowRaster` | `0x745240` | Return camera |
| `RwRenderStateSet` | `0x7FC2D0` | No-op |
| `RenderScene` | TBD | No-op |
| `RenderEffects` | TBD | No-op |
| `Render2dStuff` | TBD | No-op |

### Timer Behavior

`CTimer::Update()` (address `0x561B10`) must NOT be stubbed. It calculates `ms_fTimeStep` which all game logic depends on. With rendering no-oped, each "frame" will be very fast. This means `ms_fTimeStep` will be very small (sub-millisecond delta), which is fine — the game will just advance in tiny increments, like running at thousands of FPS.

If this causes issues (e.g., the game skips logic when delta is too small), we can hook `CTimer::Update` to force a fixed timestep:

```cpp
void CTimer::Update_Hooked() {
    ms_fTimeStep = 1.0f;        // Simulate ~30fps
    ms_fTimeStepNonClipped = 1.0f;
    ms_fOldTimeStep = 1.0f;
    m_snTimeInMilliseconds += 33; // Advance 33ms per frame
}
```

### Deliverables
- All render stubs installed and verified
- Game loop runs: `CTimer::Update()` → `CGame::Process()` → (render stubs) → repeat
- Log output confirms game logic is advancing (frame counter, game time)

---

## Phase 4: Differential Test Harness

**Goal:** Automatically test each reversed function against the original by comparing behavior.

### Core Mechanism

The project's reversible hook system already supports enabling/disabling individual hooks at runtime. This is the foundation:

```cpp
// Pseudocode for differential testing
void DifferentialTest(ReversibleHook& hook, TestInput& input) {
    // 1. Capture full game state
    GameStateSnapshot before = SnapshotGameState();

    // 2. Run ORIGINAL function (disable reversed hook)
    hook.Disable();  // restores original bytes at function address
    auto resultA = InvokeFunction(hook.GetAddress(), input);
    GameStateSnapshot afterOriginal = SnapshotGameState();

    // 3. Restore state
    RestoreGameState(before);

    // 4. Run REVERSED function (enable reversed hook)
    hook.Enable();   // installs JMP to reversed code
    auto resultB = InvokeFunction(hook.GetAddress(), input);
    GameStateSnapshot afterReversed = SnapshotGameState();

    // 5. Compare
    CompareSnapshots(afterOriginal, afterReversed, hook.GetName());
}
```

### Game State Snapshot Implementation

The game's mutable state lives in known memory regions. From the binary layout of `gta_sa.exe`:

```cpp
struct GameStateSnapshot {
    // .data and .bss sections of gta_sa.exe contain all global state
    // Typical layout for GTA SA Compact:
    // 0x608000 - 0xC90000 (~6.5MB of global data)
    std::vector<uint8_t> globalData;

    // Pool contents (dynamically allocated game objects)
    std::vector<uint8_t> pedPool;
    std::vector<uint8_t> vehiclePool;
    std::vector<uint8_t> objectPool;
    std::vector<uint8_t> buildingPool;
};

GameStateSnapshot SnapshotGameState() {
    GameStateSnapshot snap;
    // memcpy the entire global data section
    snap.globalData.assign(
        (uint8_t*)0x608000,
        (uint8_t*)0xC90000
    );
    // Snapshot pool contents
    // ... similar for each pool
    return snap;
}

void RestoreGameState(const GameStateSnapshot& snap) {
    memcpy((void*)0x608000, snap.globalData.data(), snap.globalData.size());
    // Restore pools...
}
```

### Comparison with Tolerance

```cpp
void CompareSnapshots(
    const GameStateSnapshot& a,
    const GameStateSnapshot& b,
    const char* functionName
) {
    for (size_t i = 0; i < a.globalData.size(); i++) {
        if (a.globalData[i] != b.globalData[i]) {
            uintptr_t addr = 0x608000 + i;

            // Check if this is a known float location
            if (IsFloatAligned(addr) && IsInFloatRegion(addr)) {
                float fa = *(float*)&a.globalData[i];
                float fb = *(float*)&b.globalData[i];
                if (ApproxEqual(fa, fb, /*epsilon=*/1e-5f)) {
                    continue; // Float rounding difference, acceptable
                }
            }

            // Check if this is a heap pointer (different alloc addresses)
            if (IsPointerAligned(addr) && BothLookLikeHeapPtrs(a, b, i)) {
                continue; // Pointer values differ but both valid
            }

            // Real difference found
            ReportMismatch(functionName, addr, a.globalData[i], b.globalData[i]);
        }
    }
}
```

### RNG Determinism

Before each test pair, reset the random state:

```cpp
// Reset all RNG sources to known seeds
void ResetRNG() {
    CGeneral::randomEngine.seed(42);
    // The game also uses rand() in some places
    srand(42);
}
```

### Test Input Generation

Different function categories need different test inputs:

| Category | Input Strategy |
|---|---|
| **Math functions** (CVector, CMatrix, etc.) | Parameterized: identity, unit vectors, edge cases (0, NaN, huge), random |
| **Game logic** (Process functions) | Let the game advance N frames, test at each frame |
| **State machines** (Task, AI) | Load specific save games that put entities in known states |
| **Parsers** (FileLoader) | Feed known game data files |

### Test Execution Flow

```
1. Game boots up headless
2. Wait for initialization complete
3. Load a save game (known state)
4. For each reversed function in hooks.csv:
   a. Reset RNG
   b. Snapshot state
   c. Run original → snapshot
   d. Restore state, reset RNG
   e. Run reversed → snapshot
   f. Compare, log results
5. Output: test_results.json with pass/fail per function
```

### Deliverables
- `GameStateSnapshot` implementation (memcpy-based)
- `DifferentialTest` runner that iterates over all hooks
- `CompareSnapshots` with float tolerance and pointer-value filtering
- Test result output (JSON/JUnit XML for CI integration)
- Initial pass over all ~7,265 reversed functions

### Risks
- Some functions have side effects beyond memory (file I/O, audio device writes) — these can't be snapshotted by memory comparison alone. Skip or handle specially.
- Functions that allocate memory will produce different heap pointer values — the pointer-filtering heuristic must be robust.
- Some functions may only be callable in specific game states (e.g., swimming AI only runs when ped is in water). Need multiple save games covering different scenarios.
- The snapshot/restore of ~6.5MB per function call is slow. For 7,265 functions this is ~47GB of memcpy per run. At ~10GB/s memcpy speed, that's ~5 seconds for the memcpy alone. Acceptable.

---

## Phase 4b: Developer-Authored Scenario Tests

**Goal:** A test framework that lets developers write readable, intentional tests for individual functions — not just "matches original" but "does what we expect."

### Why This Is Needed

Differential testing (Phase 4) answers: "does our code do the same thing as the original?" But it can't:

- Test functions that haven't been reversed yet (no original to compare against for new helper functions marked `// NOTSA`)
- Document *intended* behavior — a developer reading a diff test sees nothing about what the function should do
- Catch bugs in the **original** game code — if both original and reversed have the same bug, differential testing says "pass"
- Test edge cases the game never hits in normal gameplay

### Test API Design

Provide a GoogleTest-like API that runs **inside the game process** with full access to game state:

```cpp
// tests/scenarios/test_vector.cpp
#include <TestFramework.h>

GAME_TEST(CVector, Normalise_UnitLength) {
    CVector v(3.0f, 4.0f, 0.0f);
    v.Normalise();
    EXPECT_NEAR(v.Magnitude(), 1.0f, 1e-5f);
    EXPECT_NEAR(v.x, 0.6f, 1e-5f);
    EXPECT_NEAR(v.y, 0.8f, 1e-5f);
}

GAME_TEST(CVector, Normalise_ZeroVector) {
    CVector v(0.0f, 0.0f, 0.0f);
    v.Normalise();
    // Original game doesn't guard against this — produces NaN
    // Our reversed version should match that behavior (or we decide to fix it)
    EXPECT_TRUE(std::isnan(v.x) || v.IsZero());
}

GAME_TEST(CQuaternion, Slerp_Endpoints) {
    CQuaternion a(0, 0, 0, 1);  // identity
    CQuaternion b(0, 0, 0.707f, 0.707f);  // 90 deg around Z
    CQuaternion result;

    result.Slerp(a, b, 0.0f);
    EXPECT_QUAT_NEAR(result, a, 1e-5f);

    result.Slerp(a, b, 1.0f);
    EXPECT_QUAT_NEAR(result, b, 1e-5f);
}
```

### Tests That Need Game State (Scenarios)

For functions that operate on game objects, provide helpers to set up and tear down scenarios:

```cpp
// tests/scenarios/test_ped_process.cpp
#include <TestFramework.h>
#include <ScenarioHelpers.h>

GAME_TEST(CPed, ProcessControl_DeadPedDoesNotMove) {
    // ARRANGE: create a ped in the game world
    auto* ped = Scenario::CreatePed(MODEL_BALLAS1, {100.f, 200.f, 10.f});
    ped->m_fHealth = 0.0f;
    auto posBefore = ped->GetPosition();

    // ACT: run one tick of ped processing
    ped->ProcessControl();

    // ASSERT
    EXPECT_VEC_NEAR(ped->GetPosition(), posBefore, 0.01f);

    Scenario::CleanupPed(ped);
}

GAME_TEST(CPed, ProcessControl_FallingPedGainsDownwardVelocity) {
    auto* ped = Scenario::CreatePed(MODEL_CJ, {100.f, 200.f, 500.f}); // high in air
    ped->m_vecMoveSpeed = CVector(0, 0, 0);

    // Run several ticks
    for (int i = 0; i < 10; i++) {
        ped->ProcessControl();
    }

    EXPECT_LT(ped->m_vecMoveSpeed.z, -0.01f); // should be falling

    Scenario::CleanupPed(ped);
}

GAME_TEST(CAutomobile, ProcessControl_HandbrakeStopsVehicle) {
    auto* car = Scenario::CreateVehicle(MODEL_SULTAN, {500.f, 500.f, 10.f});
    car->m_vecMoveSpeed = CVector(0.1f, 0, 0);  // moving forward
    car->m_fBreakPedal = 0.0f;
    car->m_fHandbrakeForce = 1.0f;

    for (int i = 0; i < 60; i++) {  // ~2 seconds
        car->ProcessControl();
    }

    EXPECT_NEAR(car->m_vecMoveSpeed.Magnitude(), 0.0f, 0.01f);

    Scenario::CleanupVehicle(car);
}
```

### Scenario Helpers

```cpp
// tests/harness/ScenarioHelpers.h
namespace Scenario {
    // These use the game's own pool allocators — objects are "real" game objects
    CPed* CreatePed(eModelID model, CVector pos);
    CVehicle* CreateVehicle(eModelID model, CVector pos);
    CObject* CreateObject(eModelID model, CVector pos);

    void CleanupPed(CPed* ped);
    void CleanupVehicle(CVehicle* vehicle);
    void CleanupObject(CObject* obj);

    // Advance game logic by N frames with fixed timestep
    void AdvanceFrames(int n, float timeStep = 1.0f / 30.0f);

    // Load a specific save game and wait for streaming to finish
    void LoadSaveGame(const char* path);

    // Reset world to clean state (clear all dynamic objects)
    void ResetWorld();

    // Set weather, time, wanted level, etc.
    void SetWeather(eWeatherType weather);
    void SetGameTime(int hours, int minutes);
    void SetWantedLevel(int stars);
}
```

### Differential + Scenario Combined

Developers can also write scenario-based differential tests — set up a specific situation, then compare original vs reversed:

```cpp
GAME_DIFF_TEST(CTrafficLights, GetLightForCar_AtIntersection) {
    // Set up a specific intersection state
    Scenario::SetGameTime(12, 0);  // noon
    auto* car = Scenario::CreateVehicle(MODEL_SULTAN, {1200.f, -1300.f, 38.f});

    // This macro does: disable hook → call → snapshot → restore → enable hook → call → snapshot → compare
    EXPECT_MATCHES_ORIGINAL(CTrafficLights::GetLightForCar, car);

    Scenario::CleanupVehicle(car);
}
```

### Test Discovery and Execution

```bash
# Run all scenario tests
wine gta_sa.exe --test-scenarios

# Run specific test
wine gta_sa.exe --test-scenario "CPed/ProcessControl_DeadPedDoesNotMove"

# Run tests matching pattern
wine gta_sa.exe --test-scenario "CVector/*"

# Run both differential and scenario tests
wine gta_sa.exe --test-all

# List available tests
wine gta_sa.exe --list-tests
```

### How Tests Are Compiled

Scenario test `.cpp` files are compiled into the `.asi` DLL alongside the reversed code. They register themselves via static initialization (like GoogleTest):

```cpp
// TestFramework.h
#define GAME_TEST(Class, Name) \
    void GameTest_##Class##_##Name(); \
    static TestRegistrar g_reg_##Class##_##Name(#Class, #Name, GameTest_##Class##_##Name); \
    void GameTest_##Class##_##Name()
```

When the game starts with `--test-scenarios`, instead of entering normal gameplay, the harness iterates over registered tests and runs them.

### Deliverables
- `TestFramework.h` — test registration, assertion macros, test runner
- `ScenarioHelpers.h/.cpp` — game object creation/cleanup, frame advancement
- `GAME_TEST` macro for simple tests, `GAME_DIFF_TEST` for differential scenario tests
- Command-line flags for test selection and filtering
- Example tests for CVector, CMatrix, CQuaternion, CPed, CVehicle
- Documentation for contributors: "How to write a test"

### Risks
- Creating game objects outside normal game flow may trigger assertions or crash if required systems aren't initialized
- Object cleanup must be thorough — leaked pool objects will corrupt state for subsequent tests
- Test isolation: tests must not depend on execution order. `ResetWorld()` between tests may be needed but is slow.

---

## Phase 5: CI Pipeline

**Goal:** Automated build + test on every PR, running on Linux CI.

### Pipeline

```yaml
# .github/workflows/test.yml (conceptual)
jobs:
  build:
    runs-on: ubuntu-latest
    container: ghcr.io/gta-reversed/build-env:latest
    steps:
      - uses: actions/checkout@v4
      - name: Build with MSVC via Wine
        run: |
          cmake -G Ninja -B build
          cmake --build build
      - name: Upload artifacts
        uses: actions/upload-artifact@v4
        with:
          name: asi-dll
          path: |
            build/gta_reversed.asi
            build/d3d9.dll

  test:
    needs: build
    runs-on: [self-hosted, linux]  # needs game files on runner
    steps:
      - name: Download artifacts
        uses: actions/download-artifact@v4
      - name: Run headless tests
        run: |
          ./scripts/run-headless-tests.sh
      - name: Upload results
        uses: actions/upload-artifact@v4
        with:
          name: test-results
          path: test_results.json
```

### Self-Hosted Runner Requirement

The game binary can't be distributed via GitHub. Test runners must be self-hosted with the game files pre-installed. The Docker image contains everything except game data, which is mounted at runtime.

```bash
docker run \
  -v /opt/gtasa-game:/game:ro \
  -v ./build:/build:ro \
  ghcr.io/gta-reversed/test-env:latest \
  /scripts/run_tests.sh
```

### Test Reporting

- Output JUnit XML for GitHub Actions integration
- Per-function pass/fail visible in PR checks
- Regression detection: if a previously-passing function now fails, block merge

### Deliverables
- `Dockerfile.build` — build environment with MSVC
- `Dockerfile.test` — runtime environment with Wine + game bootstrap
- GitHub Actions workflow (or equivalent CI config)
- `run-headless-tests.sh` — orchestrates Wine startup, waits for game init, collects results
- Test result dashboard or PR check integration

---

## Phase 6: Expanding Coverage

**Goal:** Beyond basic differential testing, add targeted test suites for specific subsystems.

### 6a: Save Game Test Matrix

Create or collect save games that exercise different game states:

| Save | State | Functions Exercised |
|---|---|---|
| `new_game.b` | Fresh start, Grove Street | Basic ped, vehicle spawning |
| `swimming.b` | Player in water | Swimming tasks, buoyancy |
| `driving.b` | Player in vehicle on highway | Vehicle physics, traffic AI |
| `indoor.b` | Player in interior | Interior manager, collision |
| `wanted.b` | 4-star wanted level | Police AI, pursuit logic |
| `mission_X.b` | Mid-mission states | Mission script, objectives |

Each save game tests functions that only execute in those contexts.

### 6b: Targeted Math/Utility Tests

Standalone unit tests (don't need the game running) for pure functions. These compile on Linux natively with a thin compatibility shim:

- CVector, CVector2D — all methods
- CMatrix — rotation, transform, inverse, euler
- CQuaternion — slerp, multiply, normalize
- CGeneral — angle math, quadratic solver
- FixedFloat/FixedVector — conversion precision
- CPool — allocation, deallocation, ref counting
- Collision geometry — sphere, line, triangle

These run in seconds and catch regressions in foundational code.

### 6c: Long-Running Soak Tests

Let the game run for thousands of frames, logging any divergence:

```cpp
// Run 10,000 frames with all hooks enabled
// Every 100 frames, disable all hooks for 1 frame,
// then re-enable and compare state divergence
```

This catches cumulative drift — where each frame's tiny float difference compounds until gameplay diverges.

### 6d: Function-Specific Input Fuzzing

For functions that take simple inputs (coordinates, angles, IDs), fuzz the inputs:

```cpp
// Example: CGeneral::LimitAngle
for (int i = 0; i < 100000; i++) {
    float input = RandomFloat(-1e6, 1e6);
    hook.Disable();
    float original = CGeneral::LimitAngle(input);
    hook.Enable();
    float reversed = CGeneral::LimitAngle(input);
    ASSERT_FLOAT_EQ(original, reversed);
}
```

---

## Effort Estimates

| Phase | Work | Estimate |
|---|---|---|
| **Phase 0**: Docker + MSVC build | Dockerfile, cmake toolchain, verify build | 15-20 hours |
| **Phase 1**: Null D3D9 device | COM interfaces, stub methods, iterative debugging | 25-35 hours |
| **Phase 2**: Headless bootstrap | Wine setup, game init hooks, skip dialogs/videos | 15-20 hours |
| **Phase 3**: RW render stubs | Hook ~16 functions, verify game loop runs | 5-10 hours |
| **Phase 4**: Differential test harness | Snapshot engine, comparison logic, test runner | 40-50 hours |
| **Phase 4b**: Scenario test framework | Test API, scenario helpers, example tests, docs | 25-35 hours |
| **Phase 5**: CI pipeline | Docker images, GitHub Actions, reporting | 15-20 hours |
| **Phase 6**: Expanded coverage | Save games, unit tests, soak tests, fuzzing | Ongoing |
| **Total (Phases 0-5)** | | **~140-190 hours** |

## Coverage Potential

| Approach | Functions Testable | % of 7,265 Reversed |
|---|---|---|
| Differential testing (Phase 4) | ~5,000-6,000 | ~70-80% |
| Standalone math/utility tests (Phase 6b) | ~200-300 | ~3-4% |
| Save-game-specific testing (Phase 6a) | +500-1,000 | +7-14% |
| Soak testing (Phase 6c) | All that execute | Cumulative check |
| **Combined** | **~6,000-7,000** | **~85-95%** |

The remaining 5-15% are functions that never execute in any test scenario (rare edge cases, unused code paths, cut content) or functions with side effects that can't be captured by memory snapshots (file I/O, network).

---

## File Structure (Actual)

```
grandpassenger/
├── null_d3d9/                         # Null D3D9 stub (Phase 1)
│   ├── null_d3d9.cpp                  # All 11 COM class implementations (~1280 lines)
│   ├── null_d3d9.def                  # DLL export definitions
│   └── CMakeLists.txt                 # Builds d3d9.dll
├── headless_stubs/                    # Headless runtime code (Phase 3-4b)
│   ├── headless_render_stubs.cpp      # RW render stubs, logged init, debug timer
│   ├── soak_test.cpp                  # Soak test + env var dispatch (diff/game tests)
│   ├── differential_test.cpp          # Differential test state machine (Phase 4)
│   ├── TestFramework.h                # GAME_TEST/GAME_DIFF_TEST macros, assertions (Phase 4b)
│   ├── game_tests.cpp                 # Test runner (Phase 4b)
│   ├── tests/                         # Per-class test files (auto-discovered by build.sh)
│   │   ├── test_CGeneral.cpp          # Math functions (5 behavior + 5 diff)
│   │   ├── test_CVector.cpp           # Vector math (12 behavior)
│   │   ├── test_CTimeCycle.cpp        # Time cycle getters (10 diff)
│   │   ├── test_CStats.cpp            # Stat queries (6 diff)
│   │   ├── test_CWanted.cpp           # Wanted level (6 diff)
│   │   ├── test_CDarkel.cpp           # Kill frenzy (5 diff)
│   │   ├── test_CWeather.cpp          # Weather (4 diff)
│   │   ├── test_CClock.cpp            # Game clock (3 diff)
│   │   └── ... (15 files total)       # + CCheat, CGameLogic, CGarages, CPopCycle, CRestart, CTheZones
│   ├── soak_test.cpp                  # Soak test + env var dispatch
│   └── differential_test.cpp          # Differential test state machine
├── test_asi/                          # Minimal test ASI (debugging)
│   ├── test_asi.cpp
│   └── test_asi.def
├── patches/                           # Build-time patches for gta-reversed
│   ├── 01-headless-wine-runtime.patch # Wine compat, menu skip, audio, probes
│   ├── 02-headless-probes.patch       # LoadLevel logging
│   ├── 03-convert-buffer-probes.patch # CBO probes + CreatePipe pool fix
│   ├── 04-scripts-process-probes.patch# Script/opcode logging
│   └── 05-headless-game-loop.patch    # Idle shortcut, ForegroundApp, frame limiter bypass
├── docker/
│   ├── Dockerfile.build               # MSVC + Wine build environment
│   ├── toolchain-msvc-wine.cmake      # CMake toolchain for cl.exe under Wine
│   └── patch-cl-zi.sh                 # Patches cl wrapper: /Zi -> /Z7
├── scripts/                           # See tools.md for full reference
│   ├── docker-build.sh                # Host: build image + run commands
│   ├── run.sh                         # Host: run headless game in Docker
│   ├── build.sh                       # Container: main ASI build
│   ├── build-null-d3d9.sh             # Container: build null d3d9.dll
│   ├── run-headless.sh                # Container: headless game launcher
│   ├── run-differential-test.sh       # Container: differential test orchestrator
│   ├── setup-wine-prefix.sh           # Container: Wine prefix creation
│   ├── wine-crash-dump.sh             # Container: crash diagnosis
│   ├── resolve-crash.py               # Map file symbol resolver
│   ├── test-toolchain.sh              # Container: toolchain verification
│   ├── build-test-asi.sh              # Container: test ASI build
│   └── conanprofile-wine.txt          # Conan profile for MSVC-under-Wine
├── configs/
│   └── gta-reversed.ini               # FastLoader headless config
├── build-output/                      # Build artifacts
│   ├── gta_reversed.asi               # Phase 0 + 3 + 4 output (~22MB)
│   ├── gta_reversed.map               # Linker map (~36MB, ~114K symbols)
│   └── d3d9.dll                       # Phase 1 output (~24KB)
├── gta-reversed/                      # Upstream repo (mounted read-only)
├── gta_sa_compact.exe                 # Original game binary
└── GTASA/             # Game data files
```

### Planned (Phase 5, not yet implemented)

```
├── docker/
│   └── Dockerfile.test                # Separate runtime environment
└── .github/
    └── workflows/
        └── test.yml                   # CI pipeline
```
