# Phase 4: Differential Test Harness

Compares game behavior with reversed hooks enabled vs disabled by hashing observable game state over multiple frames. Detects behavioral differences between reversed and original code.

## Status: Complete

All test modes work end-to-end:
- **Differential hash test** — deterministic baselines, all-disabled produces distinct hash, 29 categories tested
- **Scenario tests (Phase 4b)** — 2095 tests, ~36000 assertions, ~180 classes, 44 bugs found
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

### Current Test Suite: 2095 tests, ~36000 assertions, ~180 classes

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
| **Total tested** | **~1100** | **~8000** | |

### File Structure

```
headless_stubs/
├── TestFramework.h            # GAME_TEST/GAME_DIFF_TEST macros, assertions, HookDisableGuard, SuspendOtherThreads, ResumeOtherThreads
├── ScenarioHelpers.h          # RAII wrappers for spawning game objects (TestVehicle, TestPed)
├── game_tests.cpp             # Test runner with warmup phase, GAME_TEST_FILTER support (dispatched from soak_test.cpp)
├── tests/                     # Per-class test files (auto-discovered by build.sh) — 293 files
│   ├── test_CAEVehicleAudioEntity.cpp  # 4 tests — audio type/settings queries on spawned vehicle (1 bug found)
│   ├── test_CAmbientPed.cpp   # 19 diff tests on spawned civilian ped (ScenarioHelpers)
│   ├── test_CAutomobile.cpp   # 15 tests — wheel/door/physics queries on spawned automobile
│   ├── test_CBaseModelInfo.cpp # 9 diff tests — flag queries across loaded models (LOD, road, culling)
│   ├── test_CBoat.cpp         # 3 tests — wake sector queries
│   ├── test_CBridge.cpp       # 1 diff test
│   ├── test_CAEAudioHardware.cpp # 4 diff tests — channel count, scaling factors
│   ├── test_CCamera.cpp       # 17 tests (16 diff + 1 behavior)
│   ├── test_CMaths.cpp        # 6 tests — sin LUT comparison (original vs reversed static init) + behavioral
│   ├── test_CStuckCarCheck.cpp # 10 tests — array management, flag logic, differential lookups
│   ├── test_CVehicleAnimGroupData.cpp # 9 tests — timing/group/offset lookups across 30 groups (2 bugs found)
│   ├── test_cHandlingDataMgr.cpp  # 11 tests — handling lookups, drive type, unit conversion across 210 vehicles (1 bug found)
│   ├── test_CTimer.cpp            # 14 tests — timer getters, pause state, UpdateVariables with known inputs
│   ├── test_CCoronas.cpp          # 8 tests — corona registration, update, position queries on spawned coronas
│   ├── test_CTheCarGenerators.cpp # 8 tests — Init, CreateCarGenerator, RemoveCarGenerators, Get accessor
│   ├── test_CCompressedMatrixNotAligned.cpp # 6 tests — compress/decompress round-trips, 5 orientations
│   ├── test_CBirds.cpp            # 9 tests — Init, Shutdown, CreateNumberOfBirds, HandleGunShot (hit+miss)
│   ├── test_CVehicle2.cpp         # 14 tests — extended vehicle queries: CanBeDeleted, CanVehicleBeDamaged, flags, FindTyre (1 bug found)
│   ├── test_CPedGroup2.cpp        # 8 tests — group membership/distance/closest with spawned ped members
│   ├── test_Scenario_PedInVehicle.cpp  # 11 tests — ped placed as driver, vehicle+ped state queries
│   ├── test_Scenario_VehicleDamage.cpp # 12 tests — damage set/get round-trips, progression, health thresholds
│   ├── test_Scenario_WantedLevel.cpp   # 8 tests — wanted level 0-6 queries, chaos mapping (2 bugs found)
│   ├── test_CSetPieces.cpp        # 9 tests — Init, AddOne, Update early-returns, fixed-point coords
│   ├── test_Scenario_WeaponEquip.cpp  # 10 tests — give weapons, slot queries, type checks, clear
│   ├── test_CGarages2.cpp         # 10 tests — all 50 garages, IsCarSprayable, activate/deactivate, ChangeGarageType (1 bug found)
│   ├── test_CRopes2.cpp           # 15 tests — Init, FindRope, IsCarriedByRope field diagnosis, SetSpeedOfTopNode, FindPickupHeight (1 bug found)
│   ├── test_CWaterCreatures.cpp   # 8 tests — creature ID generation, position checks, info table, distribution (2 bugs found)
│   ├── test_CCarCtrl2.cpp         # 11 tests — parking, steer angle, car model loading, police/gang models (1 bug found)
│   ├── test_CPopulation2.cpp      # 11 tests — multipliers, distance queries, occupation, gang cars, gender (1 bug found)
│   ├── test_CAutomobile2.cpp      # 8 tests — collision offset, FixTyre, FixPanel, CloseAllDoors, ScanForCrimes
│   ├── test_CCarAI.cpp        # 6 diff tests — police mission/speed lookups (1 bug found)
│   ├── test_CCarEnterExit.cpp # 17 diff tests — door flags/health/seat/jack queries
│   ├── test_CConversations.cpp # 1 diff test — conversation state
│   ├── test_CCarCtrl.cpp      # 3 diff tests
│   ├── test_CCheat.cpp        # 1 diff test
│   ├── test_CClock.cpp        # 3 diff tests
│   ├── test_CClouds.cpp       # 2 diff tests
│   ├── test_CCollision.cpp    # 19 behavior tests — geometry math (hooks locked, can't diff)
│   ├── test_CControllerConfigManager.cpp  # 9 diff tests (~350 assertions)
│   ├── test_CCullZones.cpp    # 12 diff tests (zone flag queries)
│   ├── test_CClothes.cpp      # 2 diff tests — player motion group queries
│   ├── test_CDamageManager.cpp # 7 diff tests — engine/door/panel/light status (4 bugs found)
│   ├── test_CDarkel.cpp       # 6 diff tests (1 known bug)
│   ├── test_CEntity.cpp       # 10 diff tests (entity + physical queries on player ped)
│   ├── test_CEntryExitManager.cpp  # 2 diff tests
│   ├── test_CEventHandler.cpp # 2 diff tests — event type queries on player + spawned ped
│   ├── test_CExplosion.cpp    # 6 tests — explosion slot queries + area tests
│   ├── test_CFireManager.cpp  # 4 diff tests (1 bug found)
│   ├── test_CGameLogic.cpp    # 3 diff tests
│   ├── test_CGangs.cpp        # 1 diff test (1 bug found)
│   ├── test_CGangWars.cpp     # 5 diff tests
│   ├── test_CPedStats.cpp     # 1 diff test
│   ├── test_CGarages.cpp      # 5 diff tests
│   ├── test_CGeneral.cpp      # 5 behavior + 5 diff tests
│   ├── test_CGlass.cpp        # 1 diff test
│   ├── test_CMessages.cpp     # 8 tests — GXT string length/compare/copy
│   ├── test_CModelInfo.cpp    # 12 diff tests (~850 assertions)
│   ├── test_COcclusion.cpp    # 2 diff tests — position occlusion queries
│   ├── test_CPad.cpp          # 32 diff tests
│   ├── test_CPathFind.cpp     # 13 diff tests
│   ├── test_CPed.cpp          # 30 diff tests (player ped state/weapon queries)
│   ├── test_CPedClothesDesc.cpp # 3 diff tests — balaclava/tattoo/haircut on player
│   ├── test_CPedGroup.cpp     # 4 diff tests — group membership/distance queries
│   ├── test_CPedIntelligence.cpp  # 19 diff tests (task getters + relationship + event queries)
│   ├── test_CPedType.cpp      # 3 diff tests (1 bug found)
│   ├── test_CPhysical.cpp     # 10 diff tests — speed/collision/lighting on player + vehicle
│   ├── test_CPickups.cpp      # 4 diff tests
│   ├── test_CPools.cpp        # 7 diff tests — handle/reference round-trip (ped, vehicle)
│   ├── test_CPlaceable.cpp    # 7 diff tests — heading/roll/IsWithinArea on player + vehicle
│   ├── test_CPlayerInfo.cpp   # 4 diff tests
│   ├── test_CPlayerPed.cpp    # 7 diff tests
│   ├── test_CPopCycle.cpp     # 2 diff tests
│   ├── test_CPopulation.cpp   # 6 diff tests
│   ├── test_CRadar.cpp        # 5 diff tests
│   ├── test_CReplay.cpp       # 2 diff tests
│   ├── test_CRestart.cpp      # 1 diff test
│   ├── test_CStats.cpp        # 7 diff tests
│   ├── test_CStreaming.cpp    # 11 tests (10 diff + 1 behavior)
│   ├── test_CStreaming_Load.cpp # 10 tests — streaming I/O diagnostics (RequestModel, LoadAllRequestedModels, TXD loading)
│   ├── test_CStreaming_Vehicles.cpp # 41 tests — CVehicle queries on streamed bike/boat/heli/plane/bmx/trailer
│   ├── test_Scenario_PedInVehicleTypes.cpp # 19 tests — ped-in-vehicle across types, CCarEnterExit door/steal queries
│   ├── test_Scenario_DamageAcrossTypes.cpp # 9 tests — health thresholds, weapon matrix, lights across 5 vehicle types
│   ├── test_CTagManager.cpp   # 1 diff test
│   ├── test_CTaskManager.cpp  # 8 diff tests — task queries on player + spawned ped
│   ├── test_CTheScripts.cpp   # 3 diff tests
│   ├── test_CTheScripts2.cpp  # 3 diff tests — IsPedStopped/IsVehicleStopped
│   ├── test_CTheZones.cpp     # 1 diff test
│   ├── test_CTimeCycle.cpp    # 10 diff tests
│   ├── test_CTrafficLights.cpp  # 2 diff tests
│   ├── test_CVector.cpp       # 12 behavior tests
│   ├── test_CRopes.cpp        # 1 diff test
│   ├── test_CVehicle.cpp      # 18 diff tests on spawned vehicle (ScenarioHelpers)
│   ├── test_CVehicleModelInfo.cpp  # 7 diff tests (1 bug found)
│   ├── test_CWanted.cpp       # 6 diff tests
│   ├── test_CWaterLevel.cpp   # 5 diff tests — water height at ocean/river/land locations
│   ├── test_CWeapon.cpp       # 4 diff tests (player weapon slots)
│   ├── test_CWeaponInfo.cpp   # 1 diff test
│   ├── test_CWeather.cpp      # 4 diff tests
│   ├── test_CWorld.cpp        # 5 diff tests (ground/roof Z, line of sight)
│   ├── test_SurfaceInfos.cpp  # 49 tests — all 50 accessors across 179 surface IDs (~8000 assertions)
│   ├── test_CHudColours.cpp       # 3 diff tests — GetIntColour/GetRGB/GetRGBA across 20 indices (1 bug found)
│   ├── test_CTheZones2.cpp        # 10 diff tests — FindZoneByLabel, FindSmallestZone, zone geometry (2 bugs found)
│   ├── test_CTheZones3.cpp        # 2 behavior tests — GetZoneInfo (overloaded hook, can't diff)
│   ├── test_CColStore2.cpp        # 2 diff tests — HasCollisionLoaded, GetBoundingBox
│   ├── test_COcclusion2.cpp       # 3 diff tests — IsPositionOccluded radius/grid sweep, OccluderHidesBehind
│   ├── test_CAccidentManager2.cpp # 4 diff tests — GetNearestFreeAccident multi-position
│   ├── test_CShotInfo2.cpp        # 1 diff test — GetFlameThrowerShotPosn all 100 slots
│   ├── test_CPhysical3.cpp        # 5 diff tests — ApplyGravity, SkipPhysics on player + vehicle
│   ├── test_CVehicle3.cpp         # 12 diff tests — GetRemapIndex, CanBeDeleted, IsUpsideDown, GetPlaneNumGuns, etc.
│   ├── test_CPed2.cpp             # 12 diff tests — IsPointerValid, IsPedInControl, OurPedCanSeeThisEntity on player + NPC
│   ├── test_CPathFind3.cpp        # 4 diff tests — FindNodeClosestToCoorsFavourDirection (1 bug found)
│   ├── test_CPedStats3.cpp        # 3 diff tests — GetPedStatType across 26 names
│   ├── test_CWaterLevel2.cpp      # 4 behavior tests — GetWaterLevel at ocean/inland positions
│   └── test_InteriorManager.cpp   # 5 diff tests — IsGroupActive, GetPedsInterior, GetVectorsInterior
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

44 confirmed bugs found by differential testing. Each was discovered by calling the same function with hooks enabled (reversed code) vs disabled (original code) and comparing results.

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

**Not a bug — false positives confirmed by disassembly:**
- `CRadar::TransformRadarPointToScreenSpace` — divergence caused by `sret` calling convention issue in test infrastructure, not a reversal bug.
- `CVehicleRecording::RegisterRecordingFile` (#38 removed) — reversed code matches original exactly (confirmed by disassembly at `0x459F80`). Test fails due to state mutation: `NumPlayBackFiles++` on each call, so calling original then reversed always produces different return values. Note: phase4 table incorrectly listed address as `0x49AD20` (which is `CShopping::FindItem`).

## Test Infrastructure Limitations

### Functions that can't be differentially tested

- **Locked hooks** — `CCollision::*` functions have `{.locked=true}`, can't toggle. Tested as behavior tests only.
- **No hook registration** — `CAccident::IsFree()` exists but `InjectHooks()` is not implemented upstream.
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
- **Test timeout** — 600s needed for full 1914-test suite (~0.3s/test due to hook toggling overhead). Use `GAME_TEST_FILTER` for faster iteration.
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
