# Phase 2: Headless Game Bootstrap

GTA:SA runs headlessly under Wine in Docker — no GPU, no display, no user interaction. The game initializes RenderWare, loads assets, and enters the main game loop.

## Quick Start

```bash
# Run headless (uses Phase 0 Docker image + Phase 0/1 build artifacts)
docker run --rm \
    -v ./GTASA:/game:ro \
    -v ./build-output:/build:ro \
    -v ./gta_sa_compact.exe:/gamebin/gta_sa_compact.exe:ro \
    -v ./scripts/run-headless.sh:/scripts/run-headless.sh:ro \
    -v ./scripts/setup-wine-prefix.sh:/scripts/setup-wine-prefix.sh:ro \
    -v ./configs:/configs:ro \
    -e TIMEOUT=60 \
    gta-reversed-build /scripts/run-headless.sh
```

The game starts, loads the ASI, initializes via the null D3D9 device, and runs until the timeout is reached.

## What Happens

```
1. Wine prefix created (32-bit, null audio, d3d9=native override)
2. Game directory assembled:
   gta_sa.exe + gta_reversed.asi + d3d9.dll + dinput8.dll + game data
3. Xvfb starts (virtual framebuffer for Wine window creation)
4. wine gta_sa.exe launches:
   a. dinput8.dll (ASI loader) loads
   b. d3d9.dll (our null stub) loads as native
   c. gta_reversed.asi loads and hooks game functions
   d. FastLoader config skips all videos/splashes
   e. Game initializes RenderWare with null D3D9
   f. Main game loop runs
```

## Game State Machine Progress

The game state machine transitions through these states:

| State | Name | Status |
|---|---|---|
| 0 | INITIAL | ✅ OK |
| 1 | PLAYING_LOGO | ✅ OK (FastLoader skips) |
| 3 | TITLE | ✅ OK (FastLoader skips) |
| 5 | FRONTEND_LOADING | ✅ OK (with workarounds) |
| 6 | FRONTEND_LOADED | ✅ OK |
| 7 | FRONTEND_IDLE | ✅ OK (auto-skip menu) |
| 8 | LOADING_STARTED | ✅ OK (Init1, LoadLevel, Init2, Init3 all complete) |
| 9 | IDLE | ✅ OK (game loop running, 5000+ iterations) |

### State 5 Workarounds

Two issues were found and fixed in state 5 (FRONTEND_LOADING):

1. **Renderer pipeline init fails with null D3D9** — `CCarFXRenderer::Initialise()` and `CCustomBuildingRenderer::Initialise()` call `CreatePipe()` which needs real D3D9 shader pipelines. Returns false → sets `RsGlobal.quit = true`. **Fix:** Skip the renderer init check under `NOTSA_HEADLESS`.

2. **`LoadDefaultDecisionMaker` stack corruption** — `RemoveDecisionMaker` used `plugin::Call<0x6043A0>(dm)` (__cdecl) for a member function that expects `__thiscall` (`this` in ECX, callee-cleanup). Over 20 calls, the stack was destroyed. **Fix:** Reversed `AddDecisionMaker`, `RemoveDecisionMaker`, `GetInstance`, and `LoadEventIndices` in `01-headless-wine-runtime.patch`. Note: `plugin::CallMethod` already uses `__thiscall` (verified in `PluginBase.h`) — only `plugin::Call` uses `__cdecl`.

### State 8 Progress (CGame::Initialise)

| Sub-step | Time | Status |
|---|---|---|
| Init1 (pools, zones, collision, models, paths, fx) | 9ms | ✅ |
| CColAccel::startCache | <1ms | ✅ |
| CFileLoader::LoadLevel("DATA\\DEFAULT.DAT") | 3ms | ✅ |
| CFileLoader::LoadLevel(gta3.dat) | 140ms | ✅ (146 lines: IDEs, IMGs, COLFILEs, IPLs) |
| CColAccel::endCache | <1ms | ✅ |
| Init2 → CStreaming (298 reads, TXDs, simple DFFs) | 100ms | ✅ |
| Init2 → ConvertBufferToObject(model 596, police car) | ~100ms | ✅ (fixed in Phase 4 — see phase4-results.md blocker #18) |

## Verified DLL Loading

Wine debug output confirms all components load correctly:
```
Loaded "C:\Games\GTASA\d3d9.dll" at 00220000: native
Loaded "C:\Games\GTASA\gta_reversed.asi" at 03080000: native
```

## Configuration

### gta-reversed.ini (configs/gta-reversed.ini)

The FastLoader configuration skips all startup UI:

```ini
[FastLoader]
Enable=1
SaveGameToLoad=-1      ; Auto-load first available save
NoEAX=1                ; Skip EAX splash
NoNVidia=1             ; Skip NVIDIA splash
NoLogo=1               ; Skip Logo.mpg
NoTitleOrIntro=1       ; Skip intro video
NoCopyright=1          ; Skip copyright screen
NoFading=1             ; Skip fading transitions
NoLoadScreen=1         ; Skip loading screen
NoLoadingTune=1        ; Skip theme music
RenderAtAllTimes=1     ; Render even when minimized
```

### Wine Configuration

- **Architecture**: win32 (32-bit prefix)
- **Audio**: Null driver (`HKCU\Software\Wine\Drivers\Audio = ""`)
- **DLL Override**: `d3d9=native` (use our stub, not Wine's wined3d)
- **Display**: Xvfb virtual framebuffer at :99, 800x600x24

## File Structure

```
/
├── scripts/
│   ├── run-headless.sh         # Main orchestration script
│   └── setup-wine-prefix.sh   # Wine prefix creation
├── configs/
│   └── gta-reversed.ini       # FastLoader headless config
├── build-output/
│   ├── gta_reversed.asi       # Phase 0 output
│   └── d3d9.dll               # Phase 1 output
├── gta_sa_compact.exe          # Original game binary
└── GTASA/      # Game data files
```

## Scripts

### run-headless.sh
Main orchestration:
1. Creates Wine prefix if needed
2. Copies game data to Wine drive_c
3. Installs build artifacts (asi, d3d9.dll, config)
4. Starts Xvfb
5. Launches `wine gta_sa.exe` with timeout
6. Dumps all Wine C:\ log files on exit
7. Reports success if game survives the timeout

### setup-wine-prefix.sh
One-time Wine prefix setup:
- Creates 32-bit prefix (`WINEARCH=win32 wineboot --init`)
- Disables audio via registry
- Sets d3d9 DLL override to native

## Environment Variables

| Variable | Default | Description |
|---|---|---|
| `GAME_DATA` | `/game` | Path to game data directory |
| `BUILD_DIR` | `/build` | Path to build artifacts |
| `GAME_EXE` | `/gamebin/gta_sa_compact.exe` | Path to game binary |
| `WINEPREFIX` | `/opt/wine-gtasa` | Wine prefix location |
| `TIMEOUT` | `60` | Seconds to run before killing |

## Init Sequence Progress

| Step | Function | Address | Status |
|---|---|---|---|
| 1 | `RwEngineInit` | 0x7F3170 | ✅ OK |
| 2 | `PluginAttach` | 0x53D870 | ✅ OK |
| 3 | `InputDeviceAttach` | 0x744A20 | ✅ OK |
| 4 | `RwEngineOpen` | 0x7F2F70 | ✅ OK (calls Direct3DCreate9) |
| 5 | `psSelectDevice` | logged replacement | ✅ OK (subsystem + video mode set) |
| 6 | `RwEngineStart` | 0x7F2E70 | ✅ OK (after disabling windowed mode proxy) |
| 7 | `CGame::InitialiseRenderWare` | 0x5BD600 | ✅ OK |
| 8 | `CGame::InitialiseEssentialsAfterRW` | 0x5BA160 | ✅ OK (renderer check skipped) |
| 9 | `CGame::InitialiseCoreDataAfterRW` | 0x5BFA90 | ✅ OK (decision makers reversed, audio skipped) |
| 10 | `CGame::Initialise` | 0x53BC80 | ✅ Init1, LoadLevel, Init2, Init3 all complete |

### Blockers Found and Fixed (all sessions)

1. **spdlog crash during CRT static init** — spdlog's `set_pattern()` and `init_thread_pool()` crashed because they depend on TLS and thread-safe statics, both disabled for Wine compatibility. Fix: set `SPDLOG_ACTIVE_LEVEL=OFF` (all log macros become no-ops) and gut the `Logging` constructor.

2. **Missing C++23 functions in Wine's msvcp140.dll** — Wine's built-in `msvcp140.dll` lacks `std::_Throw_Cpp_error`. Fix: copy real Microsoft MSVC runtime DLLs to Wine's `system32` with `native,builtin` override.

3. **`--unhook-except=Global/CLoadingScreen`** — the project's built-in command-line flag. Disables all hooks except FastLoader (category `Global/CLoadingScreen`), which skips startup dialogs/videos. All other reversed hooks are disabled so the game runs original code (no crashes from untested reversed code under Wine).

4. **`DIFF_TEST_DISABLE_ALL` now re-enables CLoadingScreen** — the build.sh patch for differential testing now calls `SetCategoryOrItemStateByPath("Global/CLoadingScreen", true)` after disabling all hooks, so FastLoader still works during baseline runs.

5. **CheckDirectSound hanging** — `DirectSoundCreate(NULL, &ds, NULL)` blocks under Wine null audio. Fix: patched `CheckDirectSound` to return TRUE.

6. **SDL3 SDL_Init hanging** — `SDL_Init(SDL_INIT_GAMEPAD | SDL_INIT_JOYSTICK | SDL_INIT_HAPTIC)` blocks under Wine. Fix: flags set to 0.

7. **Debug timer bypassing init** — Timer was forcing `gGameState=9` and `RwInitialized=true` after 300ms, completely skipping the RW init. Fix: timer only forces past splash video states (1-4), never state 0 (init).

8. **Logged RW init replacement** — `RsRwInitialize` and `psSelectDevice` replaced with logged, headless-friendly versions via PatchJmp on the ASI's own functions. Traces every step of the init sequence to `C:\rw_init.log`.

9. **`NOTSA_WINDOWED_MODE` D3D9 proxy crash** — The windowed mode proxy writes to the D3D9 device vtable, crashing with our read-only NullDevice vtable. Wine launched winedbg which blocked forever, appearing as a hang. Fixed: disabled `NOTSA_WINDOWED_MODE` define.

10. **`/GS` stack canary failure** — MSVC stack buffer overrun check triggered after init. Fixed: disabled `/GS` with `/GS-` flag.

11. **Renderer pipeline init fails** — `CCarFXRenderer::Initialise()` and `CCustomBuildingRenderer::Initialise()` need real D3D9 shader pipelines, fail with null device. Fixed: skip the check in `InitialiseEssentialsAfterRW`.

12. **`LoadDefaultDecisionMaker` stack corruption** — `RemoveDecisionMaker` used `plugin::Call<0x6043A0>` (__cdecl) for a `__thiscall` member function. Each call shifted the stack by 4 bytes; over 20 calls the stack was destroyed. Fixed: reversed `AddDecisionMaker`, `RemoveDecisionMaker`, `GetInstance`, and `LoadEventIndices` in the patch. (Note: previous session incorrectly blamed `plugin::CallMethod` — it already uses `__thiscall` per `PluginBase.h`.)

13. **`AudioEngine.Initialise()` fails** — Requires DirectSound/EAX, not available under Wine null audio. Fixed: skip under `NOTSA_HEADLESS`, force success.

14. **`g_ikChainMan.Init()` returns non-zero** — IK chain manager fails under headless. Fixed: call but force overall `InitialiseCoreDataAfterRW` result to true.

15. **`LoadingScreen` / `NewChunkLoaded` hangs** — `DisplayPCScreen()` renders sprites (needs real D3D9), `DoPCScreenChange` calls `AudioEngine.ServiceLoadingTune()` (audio uninitialized). Fixed: early return under `NOTSA_HEADLESS`.

16. **CdStream worker thread works fine** — `CreateThread` succeeds, thread processes all 298 read requests in ~100ms. TLS stripping does NOT affect worker threads. The initial sync I/O attempt was unnecessary.

17. **`ConvertBufferToObject` hangs for vehicle DFF** — Model ID 596 (police car). TXDs and simple DFFs load fine, but the first vehicle DFF hangs when converting the file buffer into RenderWare objects. **RESOLVED in Phase 4** — see phase4-results.md blocker #18 (CCustomCarEnvMapPipeline pool allocation fix + null D3D9 caps fix).

### Key Architecture Discovery

- `NOTSA_WinMain` is a **locked hook** — always runs reversed code regardless of `--unhook-except`
- All calls from WinMain → RsEventHandler → AppEventHandler → RwInitialize are **intra-DLL calls** within the ASI, NOT through game addresses
- PatchJmp on the ASI's own `RwInitialize` function intercepts these calls
- `psInitialize` is also locked — `CheckDirectSound` and `GetVideoMemInfo` must be patched for Wine
- SDL3 is compiled in (`NOTSA_USE_SDL3`) — SDL_Init with input flags must be zeroed
- `CFileMgr::OpenFile`, `LoadFile`, `ReadLine`, `CloseFile` are all **locked hooks** — always use reversed code (fopen_s/fgets)
- `CFileMgr::SetDir` is NOT locked — but called from within ASI, so ASI's version runs (uses `_chdir`)
- Renderer init functions (`CCarFXRenderer`, `CCustomBuildingRenderer`) create RW pipelines that need real D3D9

## Known Behavior

- The game runs silently — no graphical output (null D3D9 device)
- Console output from gta_reversed.asi may be buffered
- Exit code 124 from the run script means "timeout reached" = success
- Exit code 0 means the game exited normally
- Other exit codes indicate a crash

## Key Runtime Configuration

- **`--unhook-except=Global/CLoadingScreen`** — passed to `gta_sa.exe` via `GAME_ARGS` env var. Disables all reversed hooks except FastLoader (CLoadingScreen), which skips splash screens, dialogs, and intro videos. Configurable via `GAME_ARGS` env var.
- **Native MSVC runtime DLLs** — `msvcp140.dll` and others are copied from the Docker image's MSVC toolchain to both the game directory and Wine's `system32`. Wine's built-in stubs lack C++23 functions.
- **`SaveGameToLoad=0`** — in `gta-reversed.ini`. Set to `0` (don't auto-load) since no save files exist in the headless environment.

## Log Files

The headless stubs write diagnostic logs to `C:\` in the Wine prefix (`$WINEPREFIX/drive_c/`):

| File | Contents |
|---|---|
| `rw_init.log` | Timestamped trace of the entire RW init sequence |
| `winmain_progress.log` | WinMain progress probes (rsINITIALIZE, SDL_Init, SDL_CreateWindow, rsRWINITIALIZE) |
| `d3d9_null.log` | All D3D9 API calls with timestamps |
| `d3d9_loaded.log` | null d3d9.dll DllMain proof-of-life |
| `gamestate.log` | Debug timer: gGameState, ForegroundApp, RwInitialized every 100ms |
| `headless_stubs.log` | InstallHeadlessRenderStubs proof-of-life |
| `dllmain_reached.log` | DllMain proof-of-life |
| `state5.log` | State 5 init progress: each function call timestamped |
| `pgl.log` | ProcessGameLogic calls: state, quit flag, loading flag |
| `mainloop.log` | MainLoop entry proof-of-life |

## Dependencies

| Artifact | Source |
|---|---|
| `gta_reversed.asi` | Phase 0 build |
| `d3d9.dll` | Phase 1 build |
| `dinput8.dll` | ASI loader from game data |
| `gta_sa_compact.exe` | Original game binary |
| `GTASA/` | Game data files |
| MSVC runtime DLLs | `/opt/msvc/VC/Redist/...` in Docker image |
| Docker image | `gta-reversed-build` from Phase 0 (includes xvfb, rsync) |

## Log Files

The headless stubs write diagnostic logs to `C:\` in the Wine prefix (`$WINEPREFIX/drive_c/`):

| File | Contents |
|---|---|
| `rw_init.log` | Timestamped trace of the entire RW init sequence |
| `winmain_progress.log` | WinMain progress probes |
| `d3d9_null.log` | All D3D9 API calls with timestamps |
| `state5.log` | State 5+ init progress: each function call timestamped |
| `loadlevel.log` | CFileLoader::LoadLevel progress (line-by-line) |
| `pgl.log` | ProcessGameLogic calls: state, quit flag, loading flag |
| `mainloop.log` | MainLoop entry proof-of-life |

## Completed (resolved in Phase 4)

All blockers from Phase 2 have been resolved. The game now:
- Completes CGame::Initialise (Init1, LoadLevel, Init2, Init3)
- Reaches state 9 (IDLE) with the main game loop running
- Runs ProcessGameLogic continuously (5000+ iterations in 90s, no crash)

See phase4-results.md for the final 3 blockers (#19-21) resolved to reach this state.
