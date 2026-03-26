# Phase 4: Differential Test Harness

Compares game behavior with reversed hooks enabled vs disabled by hashing observable game state over multiple frames. Detects behavioral differences between reversed and original code.

## Status: Complete

All test modes work end-to-end:
- **Differential hash test** — deterministic baselines, all-disabled produces distinct hash, 29 categories tested
- **Scenario tests (Phase 4b)** — 2947 tests, ~39800 assertions, ~270 classes, 53 bugs found
- hooks.csv (7994 hooks) and hooks_paths.csv (718 paths) collected automatically

## Architecture

Two-level system:

```
Outer: run-differential-test.sh (orchestrates multiple game launches)
  │
  ├─ Run 1: baseline (all hooks enabled) → hash1
  ├─ Run 2: baseline (all hooks enabled) → hash2  (variance check)
  ├─ Run 3: all hooks disabled           → hash3  (sensitivity check)
  ├─ Run 4: disable category "Entity"    → hash4
  ├─ Run 5: disable category "Audio"     → hash5
  │   ...
  └─ Compare hashes → find divergent categories → drill down

Inner: differential_test.cpp (state machine inside game loop)
  │
  ├─ WAIT_GAME:    wait for gGameState >= 7
  ├─ APPLY_CONFIG: read env vars, disable hooks
  ├─ WARMUP:       run N frames (streaming stabilization)
  ├─ MEASURE:      hash game state each frame (FNV-1a over ~900 bytes)
  └─ DONE:         write results file, ExitProcess(0)
```

## Configuration (environment variables)

| Variable | Default | Description |
|---|---|---|
| `DIFF_TEST_ENABLE` | (unset) | Set to `1` to activate differential test mode |
| `DIFF_TEST_WARMUP_FRAMES` | 300 | Frames before measuring (streaming stabilization) |
| `DIFF_TEST_MEASURE_FRAMES` | 100 | Frames to hash |
| `DIFF_TEST_DISABLE_HOOKS` | "" | Comma-separated hook paths to disable |
| `DIFF_TEST_DISABLE_ALL` | "0" | If "1", disable ALL hooks in DllMain (original code) |
| `DIFF_TEST_RESULTS_FILE` | `diff_test_results.txt` | Output file path |

## Game State Hashing

FNV-1a hash over known global addresses (~900 bytes/frame):

| Region | Address | Size | Contents |
|---|---|---|---|
| CTimer | `0xB7CB4C` | 64 bytes | Frame counter, time, timestep |
| CWeather | `0xC81320` | 256 bytes | Weather state |
| CClock | `0xB70148` | 16 bytes | Game hours/minutes/days |
| TheCamera | `0xB6F028` | 64 bytes | Camera matrix |
| CPlayerInfo | `0xB7CD98` | 512 bytes | Money, wanted level, etc. |

## Hook Control

Uses gta-reversed's existing reversible hook API:

- `ReversibleHooks::GetRootCategory().SetAllItemsEnabled(false)` — disable all
- `ReversibleHooks::SetCategoryOrItemStateByPath(path, false)` — disable by path
- `item->State(bool)` — toggle individual hooks
- `ForEachItem(fn)` — iterate all hooks

When `DIFF_TEST_DISABLE_ALL=1`, hooks are disabled in DllMain (before game loop) via a build-time patch to `dllmain.cpp`.

## Output Format

Key-value text file:

```
DIFF_TEST_VERSION=1
STATUS=PASS
WARMUP_FRAMES=300
MEASURE_FRAMES=100
DISABLED_HOOKS=
DISABLE_ALL=0
WALL_TIME_MS=12345
FINAL_HASH=abcdef1234567890
FRAME_HASHES=hash1,hash2,hash3,...
```

The outer script compares `FINAL_HASH` across runs. Divergence from baseline indicates a behavioral difference.

## File Structure

```
headless_stubs/
├── headless_render_stubs.cpp   # Frame callback injection (Phase 3)
├── soak_test.cpp               # Soak test + DIFF_TEST_ENABLE dispatch
├── differential_test.cpp       # Differential test state machine (Phase 4)
├── soak_test.cpp              # Soak test + env var dispatch
└── differential_test.cpp      # Differential test state machine

scripts/
├── build.sh                    # Injects all stubs into .asi build
├── run-headless.sh             # Phase 2: basic headless run
└── run-differential-test.sh    # Phase 4: multi-run differential testing
```

## Build

No separate build step — compiled into `gta_reversed.asi` automatically when `headless_stubs/` is mounted:

```bash
docker run --rm \
    -v ./gta-reversed:/src/gta-reversed:ro \
    -v ./headless_stubs:/src/headless_stubs:ro \
    -v conan-cache:/root/.conan2 \
    -v ./build-output:/build \
    gta-reversed-build /scripts/build.sh
```

`build.sh` automatically:
1. Copies `differential_test.cpp` into the source tree
2. Adds it to CMakeLists.txt
3. Patches `dllmain.cpp` with early hook disable for `DIFF_TEST_DISABLE_ALL`

## How SoakTestOnFrame Dispatches

```
SoakTestOnFrame() [called every frame from Render2dStuff + psCameraShowRaster]
  │
  ├─ DIFF_TEST_ENABLE set? → DiffTestOnFrame()
  └─ otherwise             → existing soak test logic
```

## Status: DETERMINISTIC BASELINES ACHIEVED

The game fully initializes, reaches state 9 (IDLE), and the frame callback fires every frame. The differential test produces deterministic FNV-1a hashes — all baseline runs match identically.

### Example output

```
warmup_1: PASS hash=5ac33fc792aa0787
warmup_2: PASS hash=bd4326e51e4c4a5a
baseline_1: PASS hash=a1a1117b7238c34e
baseline_2: PASS hash=a1a1117b7238c34e
baseline_3: PASS hash=a1a1117b7238c34e
  Baseline consistent: hash=a1a1117b7238c34e
all_disabled: PASS hash=6f64eb7c99f8fc1a
  All-disabled diverges from baseline (good — test is sensitive)
Phase 3: 29 top-level categories tested, 0 crash, 29 diverge
  hooks.csv: 7994 hooks in 699 categories
  hooks_paths.csv: 718 unique category paths
```

### Blockers resolved (6 new, 24 total)

**Blocker #19: Audio engine hang in CPed constructor**
- `CPed::CPed()` → `m_weaponAudio.Initialise(this)` → `CAEWeaponAudioEntity::Initialise()` → `AEAudioHardware.LoadSoundBank()` — audio engine was completely skipped in headless, so any audio call hung.
- **Fix:** Stubbed `CAEAudioHardware::Initialise()` to skip DirectSound but initialize data structures (m_nNumChannels=48, m_bInitialised=true, m_bDisableEffectsLoading=true). Added early returns in `IsSoundBankLoaded()` (returns true), `Service()`, `CAudioEngine::Service()`, and `InitialisePostLoading()`.

**Blocker #20: CRealTimeShadowManager::Init hang**
- Creates RW cameras and D3D render targets for shadow rendering — hangs in headless.
- **Fix:** Skip shadow creation in headless, set `m_bInitialised = true`.

**Blocker #21: AudioEngine::Service / InitialisePostLoading hang**
- `CAudioEngine::Service()` accesses `m_pStreamThread.GetTrackPlayTime()` (uninitialized stream thread). `InitialisePostLoading()` calls `Service()` + busy-waits on `IsSoundBankLoaded`.
- **Fix:** Early return for both in headless.

### Fixed: Vehicle DFF loading (blocker #18)

`CCustomCarEnvMapPipeline::CreatePipe()` allocated env map / specular map material pools only AFTER checking `IsEnvironmentMappingSupported()`. Under headless, this check failed (null D3D9 had no TextureOpCaps / ShadeCaps), so pools stayed NULL. When `RpClumpStreamRead` parsed vehicle materials, plugin callbacks (`pluginEnvMatStreamReadCB`, `pluginSpecMatStreamReadCB`) tried to allocate from NULL pools → hang.

**Fix:**
1. null_d3d9: added TextureOpCaps (BLENDFACTORALPHA, MULTIPLYADD) and ShadeCaps (SPECULARGOURAUDRGB) to FillDeviceCaps
2. patch 03: moved pool allocation before the pipeline/caps check with null guards
3. patch 01: headless path now calls `CCarFXRenderer::Initialise()` (pools created even when pipeline fails)

**Blocker #22: ProcessGameLogic never reaches Idle (PeekMessage + ForegroundApp + frame limiter)**
- Three compounding issues prevented `Idle()` from ever being called:
  1. **PeekMessage early return**: reversed `ProcessGameLogic` processed ONE Windows message and returned `true`, never reaching the game state switch. Under Wine, messages arrive constantly.
  2. **ForegroundApp false**: headless Wine has no window focus, so the background-pause check skipped game logic.
  3. **Frame limiter blocking**: with no real rendering time, the frame limiter condition never passed. `Idle()` was never dispatched.
- **Fix:** `patches/05-headless-game-loop.patch` — under `NOTSA_HEADLESS`: force `ForegroundApp = true`, skip frame limiter, call `RsEventHandler(rsIDLE)` unconditionally.
- Note: the PeekMessage drain is also in the patch but inactive because `NOTSA_USE_SDL3` is defined (SDL handles the message pump via `ProcessEvents()`).

**Blocker #23: Idle() hangs on RW render pipeline**
- The reversed `Idle()` calls `DoRWStuffStartOfFrame_Horizon` → `CameraSize` → `RwCameraClear` → `RsCameraBeginUpdate` → rendering. With the null D3D9, `Scene.m_pRwCamera` pipeline state is incomplete, causing hangs in the RW rendering chain.
- **Fix:** `patches/05-headless-game-loop.patch` — under `NOTSA_HEADLESS`, `Idle()` shortcuts to: `CTimer::Update()` → `CGame::Process()` → `Render2dStuff()` (our stub) → return. Skips all rendering.

**Blocker #24: WINEPREFIX not exported in run-differential-test.sh**
- The script set `WINEPREFIX` but never `export`-ed it. Wine fell back to `~/.wine` (64-bit), game never started.
- **Fix:** Added `export WINEPREFIX` to the script.

### All 24 blockers resolved.
See phase2-results.md for blockers 1-18, above for 19-24.

## Determinism: Solved

Three changes were required to achieve deterministic baselines:

### 1. Fixed timestep in headless Idle (patch 05)
Replaced `CTimer::Update()` (reads wall-clock via QueryPerformanceCounter) with deterministic timer advancement:
- Timestep = 1.0 (one physics step per frame, 50fps equivalent)
- Time advances by 20ms per frame
- All timer history variables shifted correctly
- `game_FPS` set to constant 50.0

### 2. Deterministic state reset (differential_test.cpp)
`ResetDeterministicState()` called at warmup start AND measurement start:
- Resets all CTimer variables to known values (BaseTime=10000, Frame=500)
- Seeds `srand(42)` for CGeneral::GetRandomNumber()
- Erases non-deterministic timer state accumulated during init (states 0-8)
- Re-seeding at measurement boundary isolates from any streaming-order variance during warmup

### 3. Cold-start warmup runs (run-differential-test.sh)
Two throwaway game launches before baselines:
- First run: Wine prefix creates `gta_sa.set`, populates registry, primes filesystem cache
- Second run: stabilizes remaining one-time-init effects
- All subsequent runs produce identical hashes

## Resolved: All-disabled crashes (blocker #25)

**Root cause (two issues):**

1. `ApplyHookConfig()` in `differential_test.cpp` called `SetAllItemsEnabled(false)` without re-enabling headless infrastructure hooks. During gameplay, original code called into D3D9/DirectSound (not available headless).

2. The dllmain DIFF_TEST_DISABLE_ALL block re-enabled hooks during DllMain, before game init. Some reversed functions (Audio, Shadows) assume game subsystems are initialized — enabling them during DllMain caused a crash at 0x7F2ACF (RW engine null pointer deref before RwEngineOpen).

**Fix (two parts):**

1. **Removed dllmain DIFF_TEST_DISABLE_ALL block** — the `--unhook-except=Global/CLoadingScreen` flag already sets the correct init-time hook state. Hook changes for differential testing now happen exclusively at runtime (state 9) via `differential_test.cpp`.

2. **Added headless whitelist in `differential_test.cpp`** — after `SetAllItemsEnabled(false)` at state 9, these hooks are re-enabled:
   - `Global/CGame` — InitialiseEssentialsAfterRW/InitialiseCoreDataAfterRW skip renderer check, force audio success
   - `Global/CLoadingScreen` — NewChunkLoaded/LoadingScreen skip rendering under headless
   - `Audio/CAudioEngine` — Service/InitialisePostLoading return early under headless
   - `Audio/Hardware/CAEAudioHardware` — Initialise/Service/IsSoundBankLoaded stubbed for headless
   - `Shadows/CRealTimeShadowManager` — Init skips RW camera/shadow creation under headless

**Result:** All-disabled run produces `PASS` with distinct hash (`6f64eb7c99f8fc1a`), diverging from baseline (`a1a1117b7238c34e`).

## Resolved: hooks.csv collection (blocker #26)

**Root cause:** `ReversibleHooks::OnInjectionEnd()` generates `hooks.csv` via `WriteHooksToFile("hooks.csv")` during DllMain. The file is written to the game's CWD (game directory under Wine). The test script checked for it but didn't collect it from warmup runs, and subsequent game launches may overwrite it.

**Fix:** `run-differential-test.sh` now collects `hooks.csv` after Phase 0 warmup runs and copies it to the results directory. Phase 3 category testing reads from the collected copy. The CSV is also copied to `/build/hooks.csv` for external tooling.

## Phase 4b: Scenario Test Framework

In-process test framework running inside the game at state 9 with full access to game state.

### Test Modes

Triggered by environment variables, dispatched from `SoakTestOnFrame()`:

| Variable | Mode | Description |
|---|---|---|
| `DIFF_TEST_ENABLE=1` | Differential hash test | Phase 4 state hashing |
| `GAME_TEST_ENABLE=1` | Scenario tests | Phase 4b per-function tests |
| `GAME_TEST_FILTER=<substr>` | Filtered scenario tests | Only run tests whose `Class/Name` contains the substring |
| (neither) | Soak test | Default frame-counting mode |

### Test API

```cpp
// Behavior test — verify reversed code does what we expect
GAME_TEST(CVector, Normalise_UnitLength) {
    CVector v(3.0f, 4.0f, 0.0f);
    v.Normalise();
    EXPECT_NEAR(v.Magnitude(), 1.0f, 1e-5f);
}

// Differential test — compare reversed vs original game code
GAME_DIFF_TEST(CGeneral, LimitAngle) {
    float inputs[] = { 0.f, 90.f, -90.f, 270.f, 360.f };
    for (float angle : inputs) {
        EXPECT_MATCHES_ORIGINAL("Global/CGeneral/LimitAngle",
            CGeneral::LimitAngle, 1e-7f, angle);
    }
}

// Manual hook toggle for complex multi-step comparisons
GAME_DIFF_TEST(CGeneral, SolveQuadratic) {
    float origX1, origX2, revX1, revX2;
    { HookDisableGuard guard("Global/CGeneral/SolveQuadratic");
      CGeneral::SolveQuadratic(1.f, -3.f, 2.f, origX1, origX2); }
    CGeneral::SolveQuadratic(1.f, -3.f, 2.f, revX1, revX2);
    EXPECT_NEAR(origX1, revX1, 1e-5f);
}
```

### Assertion Macros

| Macro | Description |
|---|---|
| `EXPECT_TRUE(expr)` | Boolean check |
| `EXPECT_FALSE(expr)` | Negated boolean |
| `EXPECT_EQ(a, b)` | Exact equality |
| `EXPECT_NE(a, b)` | Inequality |
| `EXPECT_LT(a, b)` / `EXPECT_GT(a, b)` | Ordering with values in failure message |
| `EXPECT_NEAR(a, b, eps)` | Float comparison with tolerance |
| `EXPECT_VEC_NEAR(vec, expected, eps)` | CVector 3-component comparison |
| `EXPECT_MATCHES_ORIGINAL(path, fn, eps, ...)` | One-liner differential: disable hook → call → re-enable → call → compare |

### Differential Test Helpers

| API | Description |
|---|---|
| `HookDisableGuard guard(path)` | RAII: disables hook, re-enables on scope exit |
| `CallOriginal(path, fn, args...)` | Call with hook disabled, auto-restore |
| `SetHookEnabled(path, bool)` | Direct hook toggle by path |

Hook paths are from `hooks_paths.csv` (e.g., `"Global/CGeneral/LimitAngle"`).

### Current Test Suite: 2947 tests, ~39800 assertions, ~270 classes

**Behavior tests (19):**
- CVector: Constructor_Default, Constructor_XYZ, Magnitude_345, Magnitude_Zero, Normalise_UnitLength, Normalise_AlreadyUnit, Addition, Subtraction, ScalarMultiply, Magnitude2D
- CGeneral: LimitAngle_InRange, LimitAngle_Wrap, GetRandomNumber_Range, GetRandomNumberInRange_Float, GetATanOfXY_Cardinals
- CVector2D: Magnitude, Normalise
- CStreaming: GetDefaultCabDriverModel_Valid
- CCamera: Find3rdPersonQuickAimPitch_NoExcept

**Tier 1 differential tests — static/global queries (no game objects needed):**
- CBridge: ShouldLightsBeFlashing
- CCamera: 16 getters (GetFading, FindCamFOV, GetScreenFadeStatus, CalculateGroundHeight, GetArrPosForVehicleType, etc.)
- CCarCtrl: ChooseBoatModel, ChoosePoliceCarModel, ChooseGangCarModel
- CCheat: AddToCheatString
- CClock: GetGameClockMinutesUntil, GetIsTimeInRange, NormaliseGameClock
- CClouds: MovingFog_GetFXIntensity, VolumetricCloudsGetMaxDistance
- CControllerConfigManager: 9 action/key/mouse binding queries (~350 assertions)
- CCullZones: 12 zone flag queries (CamCloseIn, NoPolice, FewerCars, FewerPeds, etc.)
- CDarkel: 6 tests (1 known bug — ReadStatus enum size)
- CEntryExitManager: FindNearestEntryExit, GetEntryExitIndex
- CGameLogic: IsCoopGameGoingOn, FindCityClosestToPoint, SkipCanBeActivated
- CGangs: GetWillAttackPlayerWithCops (bug found)
- CGangWars: 5 gang war state queries
- CGarages: IsModelIndexADoor, IsPointWithinHideOutGarage, IsGarageOpen, IsGarageClosed, GetGarageNumberByName
- CGeneral: LimitAngle, LimitRadianAngle, GetATanOfXY, SolveQuadratic, GetAngleBetweenPoints
- CGlass: HasGlassBeenShatteredAtCoors
- CModelInfo: 12 vehicle type checkers (71 model IDs each, ~850 assertions)
- CPad: 32 instance queries
- CPathFind: 13 region/node queries
- CPickups: ModelForWeapon (46), GetActualPickupIndex, IsPickUpPickedUp, PlayerCanPickUpThisWeaponTypeAtThisMoment
- CPopCycle: GetCurrentPercOther_Peds, IsPedInGroup
- CPopulation: 6 ped population queries
- CRadar: 5 radar transform/color queries
- CReplay: ShouldStandardCameraBeProcessed, NumberFramesAvailableToPlay
- CRestart: OverrideAndCancel
- CStats: 6 stat queries
- CStreaming: 10 model/zone queries
- CTagManager: GetPercentageTagged
- CTheScripts: IsPlayerOnAMission, HasCarModelBeenSuppressed, HasVehicleModelBeenBlockedByScript
- CTheZones: GetLevelFromPosition
- CTimeCycle: 10 ambient color/clip queries
- CTrafficLights: LightForPeds, IsMITrafficLight
- CVehicleModelInfo: 7 model property queries (1 bug found)
- CWanted: 6 wanted level queries
- CWeaponInfo: FindWeaponType (10 names)
- CWeather: 4 weather queries
- CWorld: FindGroundZForCoord, FindLowestZForCoord, GetIsLineOfSightClear, FindGroundZFor3DCoord, FindRoofZFor3DCoord

**Tier 2 differential tests — on existing game objects (player ped at state 9):**
- CEntity: GetIsOnScreen, GetIsBoundingBoxOnScreen, IsVisible, GetIsStatic, IsEntityOccluded, GetDistanceFromCentreOfMassToBaseOfModel, GetColModel
- CFireManager: GetNumOfNonScriptFires, PlentyFiresAvailable (bug found), GetNumFiresInRange, GetNumFiresInArea
- CHud: (skipped — hook paths not found at runtime)
- CPed: 18 ped state queries (IsAlive, IsPedInControl, CanBeArrested, etc.) + GetBikeRidingSkill, GetWalkAnimSpeed, GetWeaponSlot, DoWeHaveWeaponAvailable
- CPedIntelligence: 10 task getters + GetUsingParachute, IsInACarOrEnteringOne
- CPedType: FindPedType (bug found), GetPedFlag (32 types), PoliceDontCareAboutCrimesAgainstPedType (32 types)
- CPhysical: GetSpeed, GetHasCollidedWithAnyObject, CanPhysicalBeDamaged
- CPlayerInfo: IsRestartingAfterMissionFailed/Arrest/Death, IsPlayerInRemoteMode
- CPlayerPed: GetWantedLevel, IsHidden, CanPlayerStartMission
- CWeapon: IsTypeMelee, IsType2Handed, IsTypeProjectile, HasWeaponAmmoToBeUsed (all 13 weapon slots)

**Tier 3 differential tests — on spawned game objects (ScenarioHelpers):**
- CVehicle (spawned police car model 596): IsLawEnforcementVehicle, IsUpsideDown, IsOnItsSide, CanBeDriven, UsesSiren, CanPedEnterCar, CanPedLeanOut, GetVehicleAppearance, GetVehicleLightsStatus, CanPedOpenLocks, Entity_IsVisible, Entity_GetColModel
- CSpawnedPed (spawned civilian model 7): IsAlive, IsPlayer, IsPedInControl, IsPedShootable, CanBeDeleted, CanPedReturnToState, CanSetPedState, IsWearingGoggles, CanPedHoldConversation, UseGroundColModel, CanBeArrested, CanUseTorsoWhenLooking, GetBikeRidingSkill, GetWalkAnimSpeed, Entity_GetDistToBase, Entity_GetColModel, Physical_CanBeDamaged

### ScenarioHelpers (headless_stubs/ScenarioHelpers.h)

RAII helpers for creating real game objects in tests:
- `Scenario::FindLoadedVehicleModel()` / `FindLoadedPedModel()` — find models already streamed at state 9
- `Scenario::TestVehicle(modelId, pos)` — creates CAutomobile, adds to world, deletes on destruction
- `Scenario::TestPed(pedType, modelId, pos)` — creates CCivilianPed, adds to world, deletes on destruction
- At state 9: model 596 (police car) and model 7 (male civilian) are loaded and usable

### Float Precision Notes

**CMaths sin LUT:**
The reversed code (Maths.cpp) overwrites the original sin lookup table at `0xBB3E00` via a static initializer that runs before `InjectHooks()`. The table is generated from `std::sin` (MSVC C++23 constexpr), while the original game computed it at runtime (MSVC 2004 CRT `sin()`). Differences are ~1-5 ULP per entry (0.000001–0.000005). The reversed `InitMathsTables` is a no-op since the table is already populated. `GetSinFast`/`GetCosFast` hooks exist but are not findable at runtime (functions are inlined everywhere — no callers in original binary). Test calls original `InitMathsTables` directly at `0x59AC90` to restore the original table for comparison. Currently passes with `1e-5` tolerance.

**Float precision differences (not bugs — inherent to float math):**
- `CGeneral::LimitAngle(180.0f)` returns `-180.0f` (boundary wraps — both represent same angle)
- `CGeneral::LimitRadianAngle` — float precision ~1e-5 near 2π
- `CGeneral::GetAngleBetweenPoints` — float precision ~1e-4

### Infrastructure Improvements

- **`SuspendOtherThreads`** — suspends CdStream worker and background threads during test execution (prevents state changes between original/reversed calls)
- **`HookDisableGuard`** fixed — explicitly enables hook first (ensures reversed code is active), then disables (original), restores on destruction. Handles `--unhook-except` pre-disabled state correctly.
- **SEH wrapper** — `RunSingleTest()` catches exceptions per-test without affecting other tests

### Coverage Analysis

| Category | Tested | Available | Notes |
|---|---|---|---|
| Tier 1 — static/global queries | 283 | ~650 | Simple args, no game objects |
| Tier 2 — on player ped | 114 | ~400+ | Player ped, weapons, intelligence, physics, tasks, events, clothes |
| Tier 3 — on spawned objects | 84 | ~4000+ | ScenarioHelpers: CAutomobile + CVehicle + CCivilianPed |
| Tier 4 — behavior (locked hooks) | 19 | ~3000 | CCollision geometry, CAutomobile doors, CExplosion |
| Tier 1 — new (2026-03-20) | 79 | | SurfaceInfos_c, CMaths, CStuckCarCheck, CVehicleAnimGroupData, cHandlingDataMgr |
| Tier 1 — new (2026-03-20) | 31 | | CTimer, CCoronas, CTheCarGenerators, CCompressedMatrixNotAligned, CBirds |
| Tier 3 — new (2026-03-20) | 14 | | CVehicle2 (extended queries on spawned vehicle) |
| Tier 3 — scenarios (2026-03-20) | 31 | | Ped-in-vehicle, vehicle damage, wanted level |
| Tier 3 — groups (2026-03-20) | 8 | | CPedGroup2 (spawned members) |
| Streaming I/O (2026-03-20) | 10 | | CStreaming: RequestModel, LoadAllRequestedModels, TXD/DFF loading |
| Streamed vehicles (2026-03-20) | 41 | | CVehicle queries on streamed bike/boat/heli/plane/bmx/trailer |
| Ped-in-vehicle-types (2026-03-20) | 19 | | CanPedStepOutCar, CCarEnterExit door/health/steal across types |
| Damage across types (2026-03-20) | 9 | | Health thresholds, weapon matrix, lights on 5 vehicle types |
| Tier 1 — new (2026-03-24) | 30 | | CHudColours, CTheZones (extended), CColStore, COcclusion, CPedStats, CWaterLevel |
| Tier 2 — new (2026-03-24) | 22 | | CPed2 (IsPointerValid, OurPedCanSeeThisEntity), CPhysical3 (ApplyGravity, SkipPhysics) |
| Tier 3 — new (2026-03-24) | 17 | | CVehicle3 (12 queries incl. GetPlaneNumGuns), InteriorManager (5 queries) |
| Global — new (2026-03-24) | 4 | | CPathFind3, CAccidentManager2, CShotInfo2 |
| Sret direct-call (2026-03-24) | 12 | | CRadar transforms (CVector2D), CVehicle gun positions (CVector), CPed bones |
| Overloaded hooks (2026-03-24) | 27 | | IsPassenger/IsDriver-Ped/-ModelID, GetZoneInfo-, GetWaterLevel-, FindTxdSlot-name, GetKey, GetModelInfo, GetWeaponSkill, etc. |
| New classes (2026-03-24) | 8 | | CCranes, CRoadBlocks, CEscalators, CPlaneTrails, CPlane (zone queries) |
| Physics mutation (2026-03-24) | 15 | | ApplyForce, ApplyTurnForce, ApplyMoveSpeed, ApplyFrictionMoveForce on spawned vehicles |
| Deep queries (2026-03-24) | 65 | | CPedIntelligence (15 task+friend queries), CPlayerPed (6), CTheScripts (5), CStreaming (11), CCamera (9), CEntity (9), CVehicle (18+), CWorld (9) |
| New classes (2026-03-24) | 12 | | CVisibilityPlugins, CCutsceneMgr, CIplStore, CGroupEventHandler, CAnimManager, CTxdStore, CPopulation (extended), CExplosion, CFireManager |
| Vehicle subtypes (2026-03-25) | 31 | | CBike2 (8 queries), CBoat2 (11 spawned boat + CVehicle), CHeli2 (12 heli static + CAutomobile) |
| New classes (2026-03-25) | 22 | | CFont (string width), CCover, CGlass (extended), CAnimManager (group/block names), CClothes (motion groups) |
| Extended queries (2026-03-25) | 22 | | CWanted (5 force queries), CPlayerPed (8 target/weapon queries), CPlayerInfo (2), CReplay (5 pool queries) |
| Spawned objects (2026-03-25) | 6 | | CObject5 (CanBeDeleted, CanBeTargetted, CanBeUsedToTakeCoverBehind + entity queries) |
| Misc (2026-03-25) | 1 | | CCarGenerator (range check on 10 generators) |
| Population queries (2026-03-25) | 5 | | CPopulation (IsSkateable, CanSolicitPlayerOnFoot, PedMICanBeCreated variants) |
| Explosion queries (2026-03-25) | 5 | | CExplosion (GetExplosionType x16, TestForExplosionInArea x10, behavior x32) |
| World queries (2026-03-25) | 5 | | CWorld (FindPlayerSlot, FindNearestObjectOfType, FindObjectsInRange at new positions) |
| New classes (2026-03-25) | 9 | | CDecisionMaker (name lookups), CPickup (text index + visibility), CCarAI (EntitiesGoHeadOn), CRoadBlocks, CWaterLevel (depth), CEntryExit, CGarages (camera zone), CCarEnterExit (behavior) |
| Shadows/FX (2026-03-25) | 8 | | CShadows (CalcPedShadowValues, AffectColourWithLighting, TidyUpShadows, RemoveOilInArea), CSpecialFX (ReplayStarted), CPointLights (behavior) |
| Task system (2026-03-25) | 59 | | CTaskSimple* (12 classes: GetTaskType, Clone, MakeAbortable), CTaskComplex* (6 classes: GetTaskType, Clone, flags) |
| Weapon queries (2026-03-25) | 7 | | CWeapon (CanBeUsedFor2Player-Static/-Method, TargetWeaponRangeMultiplier), CWeaponInfo (GetWeaponInfo-original, GetSkillStatIndex) |
| CDarkel extended (2026-03-25) | 5 | | ResetModelsKilledByPlayer, ThisVehicleShouldBeKilledForFrenzy, QueryModelsKilledByPlayer (800 models), CheckDamagedWeaponType (441 combos) |
| Event system (2026-03-25) | 10 | | CEventScriptCommand (6 queries), CEventGroup, CEventHandlerHistory (3 queries), CEventGroupEvent (6 queries) |
| Stat queries (2026-03-25) | 12 | | CStats (GetFatAndMuscleModifier, FindRadioStation, FindCriminalRatingString, CheckForThreshold, ConvertToMins/Secs), CPedStats (GetPedStatByArrayIndex, GetPedStatInfo) |
| Streaming extended (2026-03-25) | 6 | | CStreaming (IsObjectInCdImage behavior, WeAreTryingToPhaseVehicleOut, FindMIPedSlotForInterior, HasSpecialCharLoaded, HasVehicleUpgradeLoaded) |
| Collision/attractors (2026-03-25) | 10 | | CColStore (FindColSlot, AddCollisionNeededAtPosn, AddRef/RemoveRef), CPedAttractorManager (IsPedRegisteredWithEffect) |
| Train/Plane (2026-03-25) | 14 | | CTrain (FindNearestTrain, DisableRandomTrains, FindNumCarriagesPulled, FindSideStationIsOn, IsInTunnel), CPlane (CountPlanesAndHelis, AreWeInNoPlaneZone, AreWeInNoBigPlaneZone, SwitchAmbientPlanes) |
| Radar extended (2026-03-25) | 4 | | CRadar (CalculateBlipAlpha, GetActualBlipArrayIndex, GetRadarTraceColour 36 combos, LimitRadarPoint) |
| Pickups/Glass (2026-03-25) | 7 | | CPickups (TestForPickupsInBubble, WeaponForModel, GivePlayerGoodiesWithPickUpMI, FindPickUpForThisObject), CGlass (CalcAlphaWithNormal 1 bug, FindFreePane, IsObjectGlass) |
| Garages/generators (2026-03-25) | 12 | | CGarages (AllRespraysCloseOrOpen, Activate/DeActivate, IsGarageOpen/Closed x50, GetGarageNumberByName), CCarGenerator (CheckForBlockage, CalcNextGen) |
| Shadows extended (2026-03-25) | 5 | | CShadows (GunShotSetsOilOnFire, UpdateStaticShadows) |
| Replay/Clothes/Wanted (2026-03-25) | 12 | | CReplay (vehicle pool queries), CClothes (RequestMotionGroupAnims, RebuildPlayerIfNeeded), CWanted (SetMaximumWantedLevel, CanCopJoinPursuit, SetPursuitCop) |
| Ped/Entity/Physical (2026-03-25) | 29 | | CPed (GetWeaponSkill, CanSeeEntity, UseGroundColModel, PedIsReadyForConversation, DoWeHaveWeaponAvailable), CEntity (LivesInThisNonOverlapSector, DoesNotCollideWithFlyers, GetModellingMatrix), CPhysical (GetLightingTotal, GetLightingFromCol, AddToMovingList/RemoveFromMovingList) |
| Camera/HUD/Menu (2026-03-25) | 14 | | CCamera (ConsiderPedAsDucking, IsExtraEntityToIgnore, Using1stPersonWeaponMode, GetScreenRect), CHud (behavior), CMenuManager (StretchX/Y, GetNumberOfMenuOptions 1 bug) |
| Zones/Text (2026-03-25) | 16 | | CTheZones (GetNavigationZone, GetMapZone, PointLiesWithinZone, FindSmallestZoneForPosition, ZoneContainment), CText (Get with 5 GXT keys) |
| Vehicle deep (2026-03-25) | 27 | | CVehicle7 (11 queries), CAutomobile6 (8 door/road/crime queries), CBike3 (4 lean/horn), CPhysical6 (11 force/speed/lighting) |
| Misc (2026-03-25) | 11 | | CAccidentManager (4 queries), CStuntJumpManager (4 behavior), CReplay (vehicle pool), CClothes, CWanted |
| Localisation (2026-03-25) | 14 | | CLocalisation (12 bool getters: Metric, Blood, Porn, ScreamsFromKills, Prostitutes, etc.) |
| TagManager/FxManager (2026-03-25) | 12 | | CTagManager (IsTag, GetTagPos x5, GetNearestTag), FxManager_c (FindFxSystemBP, GetFrustumInfo) |
| Interior/Water/Stuck (2026-03-25) | 22 | | InteriorManager (4 queries), WaterCreatureManager (5 CanAddWaterCreatureAtPos), CStuckCarCheck (10 edge-case handles) |
| Ropes/Coronas/Clothes (2026-03-25) | 19 | | CRopes (5 rope/pickup queries on spawned vehicle), CCoronas (8 sun/moon/update), CPedClothesDesc (11 initialise/balaclava/tattoo/haircut) |
| CWorld systematic (2026-03-25) | 17 | | ProcessLineOfSight (3), ProcessVerticalLine, FindObjectsKindaColliding (2), FindObjectsIntersectingCube, FindMissionEntitiesIntersectingCube, FindObjectsOfTypeInRange, FindLodOfTypeInRange, FindPlayerSlotWithVehiclePointer (2), GetSector (4), GetRepeatSector (4), ResetLineTestOptions, SprayPaintWorld (2) |
| CStreaming systematic (2026-03-25) | 17 | | GetNextFileOnCd (behavior), IsVeryBusy, DisableCopBikes (2), GetDefaultFiremanModel, GetDefaultMedicModel, AreAnimsUsedByRequestedModels (20), AreTexturesUsedByRequestedModels (20), IsCarModelNeededInCurrentZone (43), RemoveLeastUsedModel, AddToLoadedVehiclesList, GetDefaultCopModel (5), GetDefaultCopCarModel (5), PossiblyStreamCarOutAfterCreation, SetModelIsDeletable, SetMissionDoesntRequireModel |
| CPopulation systematic (2026-03-25) | 7 | | CanJeerAtStripper (6), FindDummyDistForModel (5), FindSpecificDriverModelForCar_ToUse (4), PedMICanBeCreatedAtThisAttractor (16 combos, 1 bug), ChooseCivilianCoupleOccupations, UpdatePedCount, IsCorrectTimeOfDayForEffect |
| CPathFind systematic (2026-03-25) | 14 | | ThisNodeHasToBeSwitchedOff, ThisNodeWillLeadIntoADeadEnd, FindLinkBetweenNodes (2), ReturnInteriorNodeIndex, FindNearestExteriorNodeToInteriorNode, AreNodesLoadedForArea (3), MarkRegionsForCoors, SetPathsNeededAtPosition, TestForPedTrafficLight (20 pairs), TestCrossesRoad (20 pairs), IsWaterNodeNearby (3 positions) |
| CRadar systematic (2026-03-25) | 12 | | DisplayThisBlip (192 assertions), LimitToMap (3), SetMapCentreToPlayerCoords, CalculateCachedSinCos, ChangeBlipBrightness/Scale/Display/Colour, SetCoordBlipAppearance, ClearBlip |
| CCarAI systematic (2026-03-25) | 12 | | CarHasReasonToStop, GetCarToGoToCoors (2), GetCarToGoToCoorsAccurate, GetCarToGoToCoorsStraightLine, GetCarToGoToCoorsRacing, GetCarToParkAtCoors, MellowOutChaseSpeed/Boat, MakeWayForCarWithSiren, TellOccupantsToLeaveCar, FindPoliceCarSpeedForWantedLevel (7 levels) |
| CPickups systematic (2026-03-25) | 17 | | PickUpShouldBeInvisible (20 more), TryToMerge_WeaponType (2), DetonateMinesHitByGunShot, PassTime, RemovePickUpsInArea, RemoveUnnecessaryPickups, PickedUpHorseShoe/Oyster, PictureTaken, CreateSomeMoney, RemoveMissionPickUps, AddToCollectedPickupsArray, GetUniquePickupIndex, FindPickUpForThisObject, GetPosn |
| CShopping systematic (2026-03-25) | 11 | | GetNameTag (1 bug), RemoveLoadedPrices/Shop, ShutdownForRestart, StoreRestoreClothesState, SetCurrentProperty, LoadStats, GetKey (60 combos), GetPriceSectionFromName, HasPlayerBought/GetPrice (computed keys) |
| New classes (2026-03-26) | 22 | | CPostEffects (7 script switches + IsVisionFXActive), C3dMarkers (5 marker slot queries), CCustomCarPlateMgr (5 plate generation + region design, 1 bug), CMenuSystem (3 menu state queries), CRenderer (2 ShouldModelBeStreamed) |
| Sin LUT divergence (2026-03-26) | 4 | | SinLUT_Divergence: table comparison (252/256 entries differ), driving curve drift at 50/1000/5000 frames |
| New classes (2026-03-26) | 17 | | IKChainManager (5 IK queries, confirms bug #42), CStreamedScripts (4 script lookups, 1 bug), CLoadingScreen (3 state queries), CTrailer (2 tow hitch/bar positions), CEventGroup2 (3 event group queries on player ped) |
| **Total tested** | **~1693** | **~8000** | |

### File Structure

```
headless_stubs/
├── TestFramework.h            # GAME_TEST/GAME_DIFF_TEST macros, assertions, HookDisableGuard, SuspendOtherThreads, ResumeOtherThreads
├── ScenarioHelpers.h          # RAII wrappers for spawning game objects (TestVehicle, TestPed)
├── game_tests.cpp             # Test runner with warmup phase, GAME_TEST_FILTER support (dispatched from soak_test.cpp)
├── tests/                     # Per-class test files (auto-discovered by build.sh)
├── soak_test.cpp              # Env var dispatch (DIFF_TEST/GAME_TEST/soak)
├── differential_test.cpp      # Phase 4 hash-based differential testing
└── headless_render_stubs.cpp  # Frame callback injection
```

To add tests for a new class, drop a `test_ClassName.cpp` file in `headless_stubs/tests/`. Build.sh auto-discovers `test_*.cpp` files.

### Running Tests

```bash
docker run --rm \
    -v ./GTASA:/game:ro \
    -v ./build-output:/build:ro \
    -v ./gta_sa_compact.exe:/gamebin/gta_sa_compact.exe:ro \
    -v ./scripts:/scripts:ro \
    -v ./configs:/configs:ro \
    -e GAME_TEST_ENABLE=1 \
    -e TIMEOUT=90 \
    gta-reversed-build /scripts/run-headless.sh
# Results: $GAMEDIR/game_test_results.txt
```

## Bugs Found in gta-reversed

53 confirmed bugs found by differential testing. Each was discovered by calling the same function with hooks enabled (reversed code) vs disabled (original code) and comparing results.

| # | Function | Bug |
|---|---|---|
| 1 | `CDarkel::ReadStatus` | `eDarkelStatus` is `enum class` (4 bytes) but original reads 1 byte from `0x96A704`. Upper bytes contain garbage. **Fix:** `enum class eDarkelStatus : uint8`. |
| 2 | `CVehicleModelInfo::GetWheelUpgrade` | Array dimensions swapped: declared `int16[15][4]` but original uses `[4][15]` layout (stride 30 vs 8). **Fix:** swap to `StaticRef<int16[NUM_WHEELS][NUM_WHEEL_UPGRADES]>`. |
| 3 | `CGangs::GetWillAttackPlayerWithCops` | Wrong memory address. `GangAttackWithCops` declared at `0xC091D9` but original at `0x5DE500` reads elsewhere. Returns garbage `11` instead of `0`. |
| 4 | `CPedType::FindPedType` | Off-by-one in loop bound. Invalid names: original returns `32` (not-found sentinel), reversed returns `31`. |
| 5 | `CFireManager::PlentyFiresAvailable` | Wrong threshold comparison (`<` vs `<=` or wrong max constant). Original returns `true`, reversed returns `false`. |
| 6 | `CAEVehicleAudioEntity::GetVehicleAudioSettings` | Wrong audio type for police cars. Models 596/597 return `VehicleAudioType=0` instead of `82`. Lookup table data incorrect. |
| 7 | `CDamageManager::GetDoorStatus` | Door 0 (BONNET): original=`4` (damaged), reversed=`0` (ok). Wrong bitfield read. |
| 8 | `CDamageManager::GetCarNodeIndexFromDoor` | Door 0 (BONNET): original=`-1` (no node), reversed=`16`. Lookup table mismatch. |
| 9 | `CDamageManager::GetCarNodeIndexFromPanel` | Panel 0: original=`-1`, reversed=`15`. Same lookup table issue as #8. |
| 10 | `CDamageManager::GetComponentGroup` | Component 0: original group=0/idx=245, reversed group=6/idx=0. Completely wrong mapping. |
| 11 | `CCarAI::FindPoliceCarSpeedForWantedLevel` | Returns `14` instead of `12` at wanted level 0. Wrong constant in switch case. |
| 12 | `CVehicleAnimGroupData::GetInOutTimings` | `eInOutTimingMode` index mapping differs from original at `0x645630`. `revStart=0.41, origStart=0` for same input. |
| 13 | `CVehicleAnimGroupData::GetAnimDoorOffset` | Returns cached zero instead of lazily computing from animation data. Original at `0x645600` calls `ComputeAnimDoorOffsets`. Difference: `rev=0, orig=0.951172`. |
| 14 | `cHandlingDataMgr::FindExactWord` | Uses `strncmp` (prefix match) instead of `strcmp` (exact match). Original at `0x6F4F30` requires exact. "BUS" can wrongly match "BUSTOP". |
| 15 | `CVehicle::FindTyreNearestPoint` | Left/right and front/rear axes swapped. Uses `GetForward()` where it should use `GetRight()` and vice versa. Developers left TODO comments acknowledging the issue. |
| 16 | `CWanted::NumOfHelisRequired` | Different heli count at some wanted level. Threshold doesn't match original at `0x561FA0`. |
| 17 | `CWanted::UpdateWantedLevel` | Chaos-to-wanted mapping diverges from original at `0x561C90`. Thresholds (50/180/550/1200/2400/4600) don't match for at least one level. |
| 18 | `CGarages::ChangeGarageType` | Sets different `m_nType` than original at `0x4476D0`. Wrong field or struct offset. |
| 19 | `CRopes::IsCarriedByRope` | Checks `m_pAttachedEntity` (offset 0x314) but original at `0x555F80` checks `m_pRopeAttachObject` (offset 0x318). **Fix:** swap field. |
| 20 | `WaterCreatureManager_c::GetRandomWaterCreatureId` | RNG range or probability thresholds differ from original at `0x6E4040`. Likely off-by-one in `GetRandomNumberInRange(0, 3)`. |
| 21 | `WaterCreatureManager_c::Init` | Free list count differs from original at `0x6E3F90`. Loop bound or linked list init mismatch. |
| 22 | `CCarCtrl::ChooseCarModelToLoad` | Different model for same car group + RNG seed. Different RNG call or range bound vs original at `0x421900`. |
| 23 | `CPopulation::FindNumberOfPedsWeCanPlaceOnBenches` | Different count than original at `0x612240`. Bench placement logic differs. |
| 24 | `CRestart::FindClosestHospitalRestartPoint` | `closestIdx >= 0u` signed/unsigned comparison always true — OOB array read when no restart point matches city-unlock filter. **Fix:** change to `closestIdx >= 0`. |
| 25 | `CRestart::FindClosestPoliceRestartPoint` | Same signed/unsigned bug as #24 at `Restart.cpp:167`. |
| 26 | `CGameLogic::CalcDistanceToForbiddenTrainCrossing` | **Two bugs:** (a) dot product uses `vecPoint` instead of `diff` vector, (b) output param writes input point instead of crossing data. |
| 27 | `CTagManager::GetPercentageTaggedInArea` | Division by zero when `iTotalTags == 0` (empty area). Missing zero-guard before `iTagged / iTotalTags * 100`. |
| 28 | `CMessages::StringCopy` | `GxtCharStrcpy` copies entire source (buffer overflow) before truncating at `len-1`. Original uses bounded copy. |
| 29 | `CAutomobile::IsInAir` | Wrong logical operator: `AreAllWheelsNotTouchingGround() && m_vecMoveSpeed.IsZero()` — `&&` should likely be `||` or speed check is inverted/absent. |
| 30 | `CShopping::FindItem` | `NOTSA_UNREACHABLE()` for non-existent keys — original falls through with defined return value. Shared root cause with #31–34. |
| 31 | `CShopping::GetItemIndex` | Same `NOTSA_UNREACHABLE()` issue as #30, searches `ms_keys` instead of `ms_prices`. |
| 32 | `CShopping::GetPrice` | Cascades from #30 (`GetPrice` calls `FindItem`). |
| 33 | `CShopping::HasPlayerBought` | Cascades from #31 (`HasPlayerBought` calls `GetItemIndex`). |
| 34 | `CShopping::GetExtraInfo` | Cascades from #30 (`GetExtraInfo` calls `FindItem`). |
| 35 | `CMessages::CutString` | Function body entirely missing — `NOTSA_UNREACHABLE()` stub, wrongly marked "unused" but hook is registered. |
| 36 | `CPickup::FindStringForTextIndex` | `ePickupPropertyText` enum is 0-based but original uses 1-based indexing (`dec eax; je`). All indices off by one. Confirmed by disassembly at `0x455540`. |
| 37 | `CPedGroupMembership::GetObjectForPedToHold` | Wrong item order (`SMOKE, INVALID, DRINK` should be `SMOKE, DRINK, INVALID`), wrong RNG method (`RandomChoiceFromList` vs percentage thresholds). Confirmed by disassembly at `0x5F6950`. |
| 38 | `CHudColours::GetIntColour` | Returns different packed uint32 RGBA than original at `0x58FD50` for some HUD colour indices. `GetRGB`/`GetRGBA` (which return `CRGBA` struct) are correct — only the integer packing diverges. |
| 39 | `CTheZones::FindZoneByLabel` | Returns different zone index than original at `0x572C40` for some navigation zone labels. Info zone lookups are correct. |
| 40 | `CTheZones::Calc2DDistanceBetween2Zones` | Significantly wrong distance — returns 805 where original at `0x5725B0` returns 242 for the same zone pair. Likely using wrong zone fields (center vs corner) or wrong distance formula. |
| 41 | `CPathFind::FindNodeClosestToCoorsFavourDirection` | Returns different node ID than original at `0x44FCE0` for same position and direction. Node area matches but node index diverges — likely wrong tie-breaking or distance comparison in the direction-favouring logic. |
| 42 | `IKChainManager_c::CanAcceptLookAt` | Returns different result than original at `0x6188B0` when querying whether a ped can accept a look-at IK chain. |
| 43 | `CStreaming::GetDefaultCabDriverModel` | Returns different model ID than original. Uses RNG internally — reversed code likely has different RNG call sequence or range. |
| 44 | `CWorld::FindNearestObjectOfType` | `FindNearestObjectOfTypeSectorList` at `World.cpp:1139` uses `<=` but original at `0x565450` uses strict `<` (`fcom` + `test ah,5` + `jp`). When two entities are equidistant, reversed replaces the first with the second, original keeps the first. **Fix:** change `dist <= radius` to `dist < radius`. |
| 45 | `CEntryExitManager::GetEntryExitIndex` | Original at `0x43EFD0` iterates pool slots from LAST to FIRST (`esi = m_nSize`, `dec esi` each loop). Reversed uses `GetAllValidWithIndex()` which iterates FIRST to LAST. Returns different index when multiple entries share the same name. **Fix:** reverse iteration direction. |
| 46 | `CGlass::CalcAlphaWithNormal` | Original at `0x71ACF0` uses constant `0.57f` (at `0x872728`) as normalization factor. Reversed uses `1/SQRT_3 ≈ 0.5774`. Difference is ~1.3%, amplified to ~8% by the 6th power (e.g., factor=1.5 → original alpha=112, reversed=119). Formula structure is correct. **Fix:** replace `/ SQRT_3` with `* 0.57f`. |
| 47 | `CPopulation::PedMICanBeCreatedAtThisAttractor` | Original at `0x6110E0` returns `true` (mov al,1 at `0x611176`) when no attractor name matches. Reversed returns `false`. **Fix:** change final `return false;` to `return true;`. |
| 48 | `CShopping::GetNameTag` | Original at `0x49ADA0` is a combined search+pointer function. `FindItem` at `0x49AD20` returns `-1` on not-found (`or eax, 0xFFFFFFFF`). Reversed uses `NOTSA_UNREACHABLE()` which crashes. Same root cause as bugs #30–34. **Fix:** replace `NOTSA_UNREACHABLE()` with `return -1;`. |
| 49 | `CPostEffects::ScriptDarknessFilterSwitch` | `std::clamp(0, alpha, 255)` has arguments in wrong order. First arg should be the value to clamp, not the lower bound. Always returns `0` when alpha > 0 instead of clamping alpha to [0,255]. **Fix:** change to `std::clamp(alpha, 0, 255)`. |
| 50 | `CCustomCarPlateMgr::GeneratePlateText` | Produces different plate text than original at `0x6FD5B0` for same RNG seed. Reversed uses `CGeneral::GetRandomNumberInRange('A', 'Z')` — likely different inclusive/exclusive range semantics or different number of RNG calls vs original. |
| 51 | `CStreamedScripts::GetProperIndexFromIndexUsedByScript` | Returns different index than original at `0x470810` for some scmIndex values. Reversed iterates `GetActiveScripts()` via `rngv::enumerate` — iteration range or `m_IndexUsedByScriptFile` matching may differ from the original loop. |

**Not a bug — false positives confirmed by disassembly:**
- `CRadar::TransformRadarPointToScreenSpace` — divergence caused by `sret` calling convention issue in test infrastructure, not a reversal bug.
- `CVehicleRecording::RegisterRecordingFile` (#38 removed) — reversed code matches original exactly (confirmed by disassembly at `0x459F80`). Test fails due to state mutation: `NumPlayBackFiles++` on each call, so calling original then reversed always produces different return values. Note: phase4 table incorrectly listed address as `0x49AD20` (which is `CShopping::FindItem`).
- `CStats::CheckForThreshold` (#46 removed) — comparison logic is equivalent between original and reversed. Test divergence caused by `*pValue` mutation: the function writes `*pValue = range` when returning true, so the second call (original or reversed) sees the mutated value.
- `CMenuManager::GetNumberOfMenuOptions` (#48 removed) — all enum values and constants match original. Test divergence caused by static cache (`s_PrevScreen` at `0x8CDFF0`): first call populates the cache, second call returns stale cached value.

## Compiler Float Divergence (Known Limitation)

The reversed code is compiled with MSVC C++23 (19.50, SSE2 64-bit floats). The original binary was compiled with MSVC 2004 (x87 80-bit intermediates). This causes per-instruction float precision differences that compound over multiple frames. Additionally, 210 call sites use `std::sin`/`std::cos` (C++23 CRT) where the original used the 2004 CRT implementation.

**Impact on per-function tests:** Negligible. Single-call divergence is ~1e-5 to 1e-4, well within test tolerances. All 53 bugs found are real logic errors, not float noise. The `QuatToEuler` tests needed `1e-3` tolerance (vs `1e-5` for simpler functions) due to `atan2`/`asin` amplifying the CRT difference — this is expected.

**Impact on mission-level testing:** Byte-identical memory snapshots between reversed and original code are not achievable with a different compiler. Mission-level testing should use structural comparison (player position within tolerance, integer state exact) rather than raw memory hashing.

**Sin LUT note:** The reversed `Maths.cpp` overwrites the game's sin lookup table at `0xBB3E00` with `std::sin` (C++23) values via static initializer. 252/256 entries differ by up to 0.000005. This does NOT affect differential testing — both hooks-enabled and hooks-disabled runs read the same overwritten table, so divergence between them still indicates a real bug. The `test_SinLUT_Divergence.cpp` tests document the table difference for reference.

## Test Infrastructure Limitations

### Functions that can't be differentially tested

- **Locked hooks** — `CCollision::*` functions have `{.locked=true}`, can't toggle. Tested as behavior tests only.
- **No hook registration** — `CAccident::IsFree()` exists but `InjectHooks()` is not implemented upstream. Also: `CStats::GetFatAndMuscleModifier`, `FindMostFavoriteRadioStation`, `FindCriminalRatingString`; `CStreaming::IsInitialised`, `IsObjectInCdImage`, `GetDiscInDrive`, `CarIsCandidateForRemoval`; `CPedStats::GetPedStatByArrayIndex`, `GetPedStatInfo`; `CColStore::FindColSlot`; `CPointLights::*` (all plugin::Call wrappers). These are tested as behavior tests only.
- **Not reversed** — `CWaterLevel::GetWaterLevelNoWaves` marked `{.reversed = false}` upstream.
- **Inlined functions** — `CPad::IsEnterJustPressed`, `IsMouseLButtonPressed`, etc. are not hooked. `CVector::InjectHooks()` disabled upstream.
- **Hook paths missing at runtime** — `CHud` hooks registered but paths not found.

### Functions that crash when toggling hooks

- **CAutomobile VMT door functions** — `IsDoorReady`, `IsDoorFullyOpen`, `IsDoorClosed`, `GetDooorAngleOpenRatio` crash when original code accesses uninitialized door state on spawned vehicles.
- **CBaseModelInfo::Get2dEffect** — crashes in original code when models have no 2D effects (index 0 OOB).
- **CCarEnterExit::ComputeDoorFlag** — original code at `0x64E550` accesses CAutomobile door data unconditionally, crashes on bikes/boats/helis.
- **CVehicleModelInfo::GetMaximumNumberOfPassengersFromNumberOfDoors / IsUpgradeAvailable** — crash in original code when models not fully streamed.
- **CPickups::GetUniquePickupIndex** — crashes in original code, accesses uninitialized pickup array slots.
- **CWeaponInfo::GetSkillStatIndex** — crashes in original code for some weapon types (OOB access).

### Struct-return calling convention limitation

Functions that return structs by value (e.g., `CVector2D`, `CVector`) use a hidden `sret` first pointer parameter in MSVC x86. When `HookDisableGuard` toggles a hook, the `sret` pointer may not be preserved correctly across the transition between original and reversed code — the original assembly writes the return value to a different stack location than the reversed C++ code reads from.

**Confirmed affected:** `CRadar::TransformRadarPointToScreenSpace` — differential test via `HookDisableGuard` shows input echoed back (sret pointer lost), but direct call at `0x583480` with explicit sret produces correct identical results.

**Workaround:** For struct-returning functions, use direct `__asm` calls with explicit sret instead of `HookDisableGuard`:
```cpp
CVector2D result;
const CVector2D* pIn = &input;
CVector2D* pOut = &result;
__asm {
    mov eax, pIn
    push eax
    mov eax, pOut
    push eax
    mov eax, 0x583480  // original function address
    call eax
    add esp, 8
}
// result now has the original's output
```

**Potentially affected functions:** Any hook where the return type is a struct > 4 bytes (`CVector2D`, `CVector`, `CRect`, `CMatrix`, etc.). Scalar returns (`bool`, `int32`, `float`, pointers) are not affected.

### Functions needing special handling

- **CExplosion mutable state** — `GetExplosionActiveCounter` and `DoesExplosionMakeSound` mutated by `CExplosion::Update()` between hook toggles. Tested as behavior tests.
- **CAEVehicleAudioEntity queries** — most audio queries need a complex `tVehicleParams` struct. Only param-free queries tested differentially.
- **State-dependent functions** — `GetDefaultCabDriverModel` (RNG), `Find3rdPersonQuickAimPitch` (camera state changes). Tested as behavior-only.

### Operational notes

- **Streaming I/O** — works in headless mode, but tests must use `ResumeThreadsGuard` to unfreeze the CdStream worker (test runner suspends all threads via `SuspendOtherThreads`). At state 9: 1 vehicle loaded, 211 unloaded with CD data, 3 peds loaded, 7 IMG files open, ~35MB memory headroom.
- **Deleted test files persist in Docker volume** — `build-tests.sh` doesn't sync deletions. Remove manually: `docker run --rm -v gta-build-tree:/tmp/gta-build gta-reversed-build rm /tmp/gta-build/source/test_Foo.cpp` then full rebuild.
- **New test files require full rebuild** — The `build-tests` incremental command only recompiles existing unity TUs. New files need `docker volume rm gta-build-tree && ./scripts/docker-build.sh build` to regenerate the unity layout and cmake configuration. After full rebuild, new tests register and run correctly (verified: all `.CRT$XCU` initializers execute under Wine).
- **Some cheats crash headlessly** — `FatCheat`, `MuscleCheat`, `ParachuteCheat` modify the player model (needs RenderWare clump operations). `EverybodyAttacksPlayerCheat`, `MayhemCheat` trigger AI that crashes without rendering. Only flag-toggle cheats (weather, time, BlackCars, Driveby, NotWanted) are safe.
- **Test timeout** — 600s needed for full 2900-test suite (~0.3s/test due to hook toggling overhead). Use `GAME_TEST_FILTER` for faster iteration.
- **Comma-separated filter** — `GAME_TEST_FILTER` now accepts comma-separated class names (e.g., `CVector,CPed2,CVehicle3`). OR logic: a test runs if any token matches its `Class/Name`. Useful for running multiple new test files in a single launch instead of one-at-a-time.
- **`run.sh test`** — `./scripts/run.sh test [filter]` handles all Docker mounts, env vars, and result display. No need to type the full `docker run` command. `./scripts/run.sh test` for full suite, `./scripts/run.sh test CVector,CPed2` for filtered.
- **Overloaded hook paths** — functions registered via `RH_ScopedOverloadedInstall` (e.g., `CTheZones::GetZoneInfo`, `CWaterLevel::GetWaterLevel`) have hook paths that include an empty suffix and can't be found by `HookDisableGuard`. Test these as behavior tests only, or use the overload's distinguishing name if one was provided.

## Next Steps

### Expand ScenarioHelpers (streaming now works)
- `RequestModel` + `LoadAllRequestedModels` confirmed working at state 9 (see `test_CStreaming_Load.cpp`)
- Spawn more vehicle types (CBoat, CBike, CBmx) — can now load any model on demand, not limited to pre-loaded models
- Spawn peds with different types (COP, GANG1, CIVFEMALE) — load model, spawn, test, unload
- Assign tasks to spawned peds (CTaskSimpleStandStill, etc.) to test task queries in non-idle states
- Put spawned ped into spawned vehicle to test in-vehicle queries
- **Important:** Tests that call streaming I/O must use `ResumeThreadsGuard` to unfreeze the CdStream worker thread (the test runner suspends all threads via `SuspendOtherThreads`)

### More test targets
- **CPedGroup with populated groups** — add members to test group queries with real data
- **CAEVehicleAudioEntity with tVehicleParams** — construct params from spawned vehicle to test audio queries
- **CAccident** — needs InjectHooks() upstream, or direct memory address testing
- **CEvent subclasses** — test specific event types (CEventDamage, CEventVehicleCollision)
- **Mission script integration** — trigger script events and verify game state

### CI pipeline (Phase 5)
- Dockerfile.test for runtime environment
- GitHub Actions workflow
- Per-PR regression testing with JUnit XML output
