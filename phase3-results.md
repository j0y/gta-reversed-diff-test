# Phase 3: RenderWare Render Stubs

Patches RenderWare rendering functions to no-ops at build time, compiled directly into `gta_reversed.asi`. Each game frame skips all CPU-side rendering work while keeping game logic intact.

## How It Works

The stubs are injected via build-time patching — `build.sh` copies `headless_render_stubs.cpp` into the gta-reversed source tree and patches `dllmain.cpp` to call `InstallHeadlessRenderStubs()` after hook initialization. No modifications to the upstream gta-reversed repo.

The `Idle()` function (0x53E920) drives each game frame. Under `NOTSA_HEADLESS` (patch 05), the entire render pipeline is bypassed:

```
Idle(param) — HEADLESS shortcut (patch 05)
  ├── Deterministic timer     ← Fixed timestep (1.0 = 50fps, +20ms/frame)
  │   (replaces CTimer::Update — no wall-clock reads)
  ├── CGame::Process()        ← KEPT (game logic)
  └── Render2dStuff()         ← STUBBED → SoakTestOnFrame() → diff test callback
      (all other render calls skipped — DoRWStuffStartOfFrame, RenderScene,
       RenderEffects, camera ops, etc. hang with null D3D9)
```

The original `Idle` render flow (used in non-headless builds) has these stubs:

```
Idle(param) — non-headless (stubs still installed for safety)
  ├── CTimer::Update()        ← KEPT
  ├── CGame::Process()        ← KEPT
  ├── RwCameraClear()         ← STUBBED → return camera
  ├── RsCameraBeginUpdate()   ← STUBBED → return true
  ├── RenderScene()           ← STUBBED → no-op
  ├── RenderEffects()         ← STUBBED → no-op
  ├── Render2dStuff()         ← STUBBED → SoakTestOnFrame()
  └── RsCameraShowRaster()    ← STUBBED → return camera
```

## Build

No separate build step — the stubs are compiled into `gta_reversed.asi` automatically when `headless_stubs/` is mounted:

```bash
docker run --rm \
    -v ./gta-reversed:/src/gta-reversed:ro \
    -v ./headless_stubs:/src/headless_stubs:ro \
    -v conan-cache:/root/.conan2 \
    -v ./build-output:/build \
    gta-reversed-build /scripts/build.sh
```

If `/src/headless_stubs/headless_render_stubs.cpp` exists, `build.sh` automatically:
1. Copies it into the source tree
2. Adds it to CMakeLists.txt
3. Patches `dllmain.cpp` to call `InstallHeadlessRenderStubs()`

If the mount is omitted, gta_reversed.asi builds without stubs (Phase 0 behavior).

## Functions Hooked

All `__cdecl`. Each entry point is overwritten with a 5-byte `JMP rel32` to a stub.

| Function | Address | Stub |
|---|---|---|
| `RenderScene` | `0x53DF40` | return immediately |
| `RenderEffects` | `0x53E170` | return immediately |
| `Render2dStuff` | `0x53E230` | `SoakTestOnFrame()` → soak/diff test callback |
| `RwCameraClear` | `0x7EE340` | return first arg |
| `RwCameraBeginUpdate` | `0x7EE190` | return first arg |
| `RwCameraEndUpdate` | `0x7EE180` | return first arg |
| `RwCameraShowRaster` | `0x7EE370` | return first arg |
| `RsCameraShowRaster` | `0x619440` | return first arg |
| `RsCameraBeginUpdate` | `0x619450` | return true |

**NOT stubbed** (needed for game logic):
- `CTimer::Update()` (0x561B10) — game logic depends on timing
- `CGame::Process()` — this IS the game logic we want to test

## Implementation

`headless_stubs/headless_render_stubs.cpp` (~500 lines) now includes:

1. **Render stubs** — JMP patches for camera/render functions
2. **Logged RsRwInitialize** — replacement for the ASI's own `RwInitialize` that calls RW functions at game addresses with full logging
3. **Logged psSelectDevice** — headless-friendly device selection (skips dialog, uses subsystem 0 / video mode 0)
4. **WinMain progress probes** — injected via build.sh sed patches into WinMain.cpp
5. **Debug timer** — logs gGameState every 100ms, forces past splash video states only

### Stub Installation Timing

Render stubs are installed in `LoggedRsRwInitialize()` around the `RwEngineStart` call — not in `InstallHeadlessRenderStubs()`. This is because:
- Camera function stubs installed too early can break `RwEngineStart` internal initialization
- Camera function stubs installed too late means they're not active when the game loop starts

They are installed pre-RwEngineStart. RwEngineStart completes successfully and the game reaches state 9 (IDLE) — see phase4-results.md.

### Init Interception Technique

The reversed `NOTSA_WinMain` (locked hook) calls `RwInitialize` as an intra-DLL function call within the ASI. Patching the game address 0x5BF390 doesn't intercept this. Instead, `InstallHeadlessRenderStubs()` does:

```cpp
extern bool RwInitialize(void* param);
PatchJmp((uintptr_t)&RwInitialize, (uintptr_t)&LoggedRwInitialize);
```

This patches the ASI's own function code, redirecting internal calls.

### Build-time patches applied by build.sh

```bash
# Copy stubs source into source tree
cp /src/headless_stubs/headless_render_stubs.cpp "${WORK_DIR}/source/"

# Add to CMake source list
sed -i 's|list(APPEND SOURCE_FILES_LIST StdInc.cpp)|...\nlist(APPEND SOURCE_FILES_LIST headless_render_stubs.cpp)|'

# Patch dllmain.cpp: add forward declaration and call
sed -i 's|void InjectHooksMain(...)|...\nvoid InstallHeadlessRenderStubs();|'
sed -i 's|ApplyCommandLineHookSettings();|...\n        InstallHeadlessRenderStubs();|'
```

## File Structure

```
headless_stubs/
├── headless_render_stubs.cpp   # Render stub implementations (compiled into .asi)
├── soak_test.cpp               # Soak test + env var dispatch
└── differential_test.cpp       # Differential test state machine
```

## Build Output

```
build-output/
├── gta_reversed.asi    (~22MB - Phase 0 + Phase 3 render stubs, Release build)
└── d3d9.dll            (~24KB - Phase 1)
```
