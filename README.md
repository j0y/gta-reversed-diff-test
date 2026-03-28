# gta-reversed differential testing

Headless Linux testing infrastructure for [gta-reversed](https://github.com/gta-reversed/gta-reversed). Builds and runs the GTA:SA reverse-engineering project entirely inside Docker using real MSVC (cl.exe 19.50, C++23) under Wine — no Windows machine, GPU, or display required.

## What it does

1. **Builds** `gta_reversed.asi` on Linux with the real MSVC compiler running under Wine
2. **Runs** GTA:SA headlessly — null D3D9 device, null audio, virtual framebuffer
3. **Tests** reversed functions by toggling hooks at runtime and comparing original vs reversed behavior
4. **Reports** bugs found by differential testing (53 confirmed bugs so far across 2947 tests)

## Quick Start

### Prerequisites

- Docker
- ~15GB disk (Docker image ~11GB + build cache)
- GTA:SA game files + [Compact exe](https://gtaforums.com) (5,189,632 bytes) — you must own the game

### Setup

```bash
# Clone upstream gta-reversed into the project directory
git clone https://github.com/gta-reversed/gta-reversed.git

# Place your game files
cp /path/to/gta_sa_compact.exe .
cp -r /path/to/GTASA-game-data/ GTASA/
```

### Build

```bash
# Build Docker image + compile gta_reversed.asi (~15 min first time, ~2 min after)
./scripts/docker-build.sh build

# Build null D3D9 stub
./scripts/docker-build.sh build-null-d3d9
```

Output: `build-output/gta_reversed.asi` (~22MB) and `build-output/d3d9.dll` (~24KB)

### Run headless

```bash
./scripts/run.sh           # Run with 90s timeout
./scripts/run.sh 120       # Custom timeout
./scripts/run.sh logs      # Show logs from last run
```

### Run tests

```bash
# Scenario tests (2947 tests, ~39800 assertions)
./scripts/docker-build.sh build-tests   # Incremental rebuild (~6s)
./scripts/run.sh test                    # Run full test suite (600s)
./scripts/run.sh test CVector            # Run one class (120s)
./scripts/run.sh test CVector,CPed,CVehicle3  # Run multiple classes (120s)

# Differential hash test
./scripts/run.sh diff                    # Multi-run differential testing
```

## Architecture

```
Docker Container (Linux)
├── msvc-wine (MSVC 19.50 C++23 via Wine)
│   └── Builds gta_reversed.asi + d3d9.dll
│
└── Wine 32-bit prefix (runtime)
    ├── gta_sa.exe (original binary)
    ├── dinput8.dll (ASI loader)
    ├── gta_reversed.asi (hooks + test harness)
    ├── d3d9.dll (null stub — all rendering no-oped)
    └── Game data files

Test harness (inside .asi at game state 9):
  For each function:
    1. Disable hook → call original code
    2. Enable hook  → call reversed code
    3. Compare results
```

## Project Structure

```
/
├── docker/                     # Docker build environment
│   ├── Dockerfile.build        # MSVC + Wine + CMake + Conan
│   ├── toolchain-msvc-wine.cmake
│   └── patch-cl-zi.sh
├── scripts/                    # Build & run scripts (see tools.md)
│   ├── docker-build.sh         # Host entry point
│   ├── build.sh                # Main ASI build (container)
│   ├── run.sh                  # Convenience run wrapper (host)
│   ├── run-headless.sh         # Headless game launcher (container)
│   ├── run-differential-test.sh
│   └── ...
├── headless_stubs/             # Test harness code (compiled into .asi)
│   ├── TestFramework.h         # GAME_TEST / GAME_DIFF_TEST macros
│   ├── ScenarioHelpers.h       # RAII game object helpers
│   ├── game_tests.cpp          # Test runner
│   ├── differential_test.cpp   # Hash-based differential testing
│   ├── headless_render_stubs.cpp
│   ├── soak_test.cpp
│   └── tests/                  # Per-class test files (350 files, auto-discovered)
├── null_d3d9/                  # Null D3D9 stub (all 11 COM interfaces)
├── test_asi/                   # Minimal test ASI for pipeline debugging
├── patches/                    # Build-time patches for gta-reversed
│   ├── 01-headless-wine-runtime.patch
│   ├── ...
│   └── 11-winmain-probes.patch
├── configs/
│   └── gta-reversed.ini        # FastLoader headless config
├── tools.md                    # Full script reference
├── gta-reversed-testing-roadmap.md  # Architecture & phase planning
├── phase0-results.md           # Phase 0: Docker build
├── phase1-results.md           # Phase 1: Null D3D9
├── phase2-results.md           # Phase 2: Headless bootstrap
├── phase3-results.md           # Phase 3: RW render stubs
└── phase4-results.md           # Phase 4: Differential & scenario tests
```

**Not tracked in git** (user-supplied / build artifacts):
- `gta-reversed/` — upstream repo, clone separately
- `gta_sa_compact.exe` — original game binary
- `GTASA/` — game data files
- `build-output/` — build artifacts

## Test Results

2947 tests across ~270 classes with ~39,800 assertions. 53 confirmed bugs found in gta-reversed via differential testing, including:

- Wrong array dimensions (`CVehicleModelInfo::GetWheelUpgrade`)
- Wrong memory addresses (`CGangs::GetWillAttackPlayerWithCops`)
- Enum size mismatches (`CDarkel::ReadStatus`)
- Off-by-one loop bounds (`CPedType::FindPedType`)
- Swapped axes (`CVehicle::FindTyreNearestPoint`)
- Wrong field offsets (`CRopes::IsCarriedByRope`)
- Wrong restart point lookup (`CRestart::FindClosestHospitalRestartPoint`, `FindClosestPoliceRestartPoint`)
- Wrong distance calculation (`CGameLogic::CalcDistanceToForbiddenTrainCrossing`)
- String truncation bug (`CMessages::StringCopy`)
- Empty area edge case (`CTagManager::GetPercentageTaggedInArea`)
- Wrong airborne detection (`CAutomobile::IsInAir`)
- Wrong item table layout (`CShopping::FindItem`, `GetPrice`, `HasPlayerBought`, `GetExtraInfo`, `GetItemIndex`)
- Wrong newline parsing (`CMessages::CutString`)
- Wrong pickup text lookup (`CPickup::FindStringForTextIndex`)
- Wrong ped hold object (`CPedGroupMembership::GetObjectForPedToHold`)
- Wrong HUD colour packing (`CHudColours::GetIntColour`)
- Wrong zone label lookup (`CTheZones::FindZoneByLabel`)
- Wrong zone distance formula (`CTheZones::Calc2DDistanceBetween2Zones`)
- Wrong pathfinding node selection (`CPathFind::FindNodeClosestToCoorsFavourDirection`)
- Wrong IK chain acceptance (`IKChainManager_c::CanAcceptLookAt`)
- Wrong cab driver model (`CStreaming::GetDefaultCabDriverModel`)
- Wrong nearest object search (`CWorld::FindNearestObjectOfType`)
- Wrong entry/exit index lookup (`CEntryExitManager::GetEntryExitIndex`)
- Wrong stat threshold check (`CStats::CheckForThreshold`)
- Wrong glass alpha calculation (`CGlass::CalcAlphaWithNormal`)
- Wrong menu option count (`CMenuManager::GetNumberOfMenuOptions`)
- Wrong attractor ped creation check (`CPopulation::PedMICanBeCreatedAtThisAttractor`)
- Wrong item name tag lookup (`CShopping::GetNameTag`)
- Wrong `std::clamp` argument order (`CPostEffects::ScriptDarknessFilterSwitch`)
- Wrong plate text generation (`CCustomCarPlateMgr::GeneratePlateText`)
- Pursuit counter desync causing dangling pointers (`CWanted::CanCopJoinPursuit`)

Full details in [phase4-results.md](phase4-results.md).

## Documentation

| File | Contents |
|---|---|
| [tools.md](tools.md) | Script reference — every build/run/debug command |
| [gta-reversed-testing-roadmap.md](gta-reversed-testing-roadmap.md) | Architecture, phase design, coverage analysis |
| [phase0-results.md](phase0-results.md) | Docker build environment, workarounds |
| [phase1-results.md](phase1-results.md) | Null D3D9 COM stub implementation |
| [phase2-results.md](phase2-results.md) | Headless bootstrap, 17 blockers resolved |
| [phase3-results.md](phase3-results.md) | RenderWare render stubs |
| [phase4-results.md](phase4-results.md) | Differential harness, scenario tests, bugs found |

## How It Works (Phases)

| Phase | Status | Description |
|---|---|---|
| **0**: Docker Build | Done | MSVC 19.50 C++23 on Linux via msvc-wine |
| **1**: Null D3D9 | Done | Standalone d3d9.dll — all 11 COM interfaces as no-ops |
| **2**: Headless Bootstrap | Done | Game reaches state 9 (IDLE), 24 blockers fixed |
| **3**: RW Render Stubs | Done | Rendering no-oped, game logic runs at full speed |
| **4**: Differential Harness | Done | Deterministic baselines, 29 categories tested |
| **4b**: Scenario Tests | Done | See [phase4-results.md](phase4-results.md) for current counts |
| **5**: CI Pipeline | Not started | GitHub Actions with self-hosted runner |

## License

The testing infrastructure in this repository (scripts, Docker config, null D3D9 stub, headless stubs, test harness) is original work.

The [gta-reversed](https://github.com/gta-reversed/gta-reversed) project is a separate upstream dependency with its own license. Game files are copyrighted by Rockstar Games and are not included.
