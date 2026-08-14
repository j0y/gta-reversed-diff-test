# Auditing the bOnSideStand differential test

Everything needed to reproduce and scrutinise the green run described in
[../stories/2026-08-14-bike-flag-green.md](../stories/2026-08-14-bike-flag-green.md),
and to check that the test proves what it claims rather than passing vacuously.

## Exact base (what to check out)

| Repo | Commit |
| --- | --- |
| outer (`j0y/gta-reversed-diff-test`) | `5650884d9bfae876f4d51101b8295661f280e50c` |
| nested (`gta-reversed`, detached) | `b1399d66e9c3a958ba92227c1f97d076e136ec5a` |
| docker image `gta-reversed-build:latest` | `a7ef7b4a70b5` (12.3 GB) |

The game binary (`gta_sa_compact.exe`) and `GTASA/` data are copyrighted and not
in any repo; an auditor needs their own copy on CT 103 at
`/opt/difftest/gta-reversed-diff-test/`.

## The three changes on top of that base

Apply these to the clean checkout. Each is one concern; nothing else in the
working tree (deleted CColStore/CStats tests, .ini tweaks) is required.

1. **`01-game_tests-streaming-pump.diff`** — the harness bug fix. WARMUP now
   pumps `CStreaming::LoadRequestedModels()` per frame and flushes with
   `LoadAllRequestedModels(false)` before tests run. Without it, requested models
   never stream in headless (census `loaded_bikes=0`); with it, all 8 load. This
   is the only change that touches shared harness code — audit it hardest.
2. **`02-CarCtrl-pr1-reversal.diff`** — PR #1 itself. The reversal of
   `GetNewVehicleDependingOnCarModel` (`0x421440`), including the
   `bikeFlags.bOnSideStand = true` writes and dropping `{ .reversed = false }` so
   the hook is actually installed. This is the *code under test*.
3. **`03-test_CCarCtrl_GetNewVehicle_diff.cpp`** — the test (an untracked file;
   drop it into `headless_stubs/tests/`). This is the *auditor's instrument*;
   read it before trusting its verdict.

## Reproduce

```bash
# on CT 103, from /opt/difftest/gta-reversed-diff-test
export SSH_AUTH_SOCK=$(find /tmp -maxdepth 2 -type s -name 'agent.*' | head -1)   # if driving remotely; see ADR 0001

# 1. build the DLL with the three changes applied
./scripts/docker-build.sh build-tests          # expect 0 errors, fresh build-output/gta_reversed.asi

# 2. run the bike test, requesting the 8 bike/BMX models and pumping enough
#    warmup frames for the streamer to deliver them
docker run --rm \
  -v "$PWD/GTASA:/game:ro" -v "$PWD/build-output:/build:ro" \
  -v "$PWD/gamebin/gta_sa_compact.exe:/gamebin/gta_sa_compact.exe:ro" \
  -v "$PWD/scripts:/scripts:ro" -v "$PWD/configs:/configs:ro" \
  -v /tmp/wine-logs:/tmp/wine-logs \
  -e GAME_TEST_ENABLE=1 \
  -e GAME_TEST_FILTER=CCarCtrl \
  -e GAME_TEST_REQUEST_MODELS=481,509,510,448,461,463,468,521 \
  -e GAME_TEST_WARMUP_FRAMES=600 \
  -e TIMEOUT=300 \
  gta-reversed-build bash -c '/scripts/run-headless.sh > /tmp/wine-logs/audit-run.log 2>&1; cp /opt/wine-gtasa/drive_c/*.txt /tmp/wine-logs/ 2>/dev/null; cp /opt/wine-gtasa/drive_c/Games/GTASA/*.txt /tmp/wine-logs/ 2>/dev/null; true'

# 3. read the verdict (authoritative — NOT the test's printf, which does not reach the file)
grep -E 'GetNewVehicleDependingOnCarModel|PASSED=|FAILED=|STATUS=' /tmp/wine-logs/game_test_results.txt
```

`bike_iter.sh` (in the harness scratch / repo) wraps steps 2–3 and prints a JSON
verdict; the manual commands above are the ground truth it automates.

## Expected output

```
  PASS  CCarCtrl/Diff_GetNewVehicleDependingOnCarModel (34 assertions)
PASSED=20   FAILED=0   STATUS=PASS
```

`34 = 10 loaded vehicles × 3 EXPECT_EQ (created, type, onSideStand) + 4 EXPECT_GT
guards`. A full sweep with no early bail.

## How to check the test is not lying (the important part)

A green run is only as trustworthy as the test. Verify each of these against
`03-...cpp`:

- **The reference is the original, not a constant.** The `orig` side runs inside
  `HookDisableGuard guard(kHookPath)`, which un-hooks the reversed function so the
  original machine code at `0x421440` executes. Grep for `HookDisableGuard`. If it
  compared against a hardcoded `true`, it would prove nothing.
- **Falsification test — break it on purpose.** In
  `02-CarCtrl-pr1-reversal.diff`, delete the two `bike->bikeFlags.bOnSideStand =
  true;` lines, rebuild, rerun. The test **must** flip to FAIL (the reversed side
  now reads false while the original still sets it). If it stays green, the test
  is vacuous. This is the single most convincing check.
- **The four guards fire.** `EXPECT_GT(compared,0)`, `EXPECT_GT(bikesSeen,0)`,
  `EXPECT_GT(onSideStandTrue,0)`, `EXPECT_GT(origSetFlag,0)`. The last two are
  counted separately so that `EXPECT_EQ(orig,rev)` cannot pass by *both* sides
  reading false — `origSetFlag>0` proves the original genuinely sets the flag.
- **Bikes actually loaded.** Confirm `ids=[448 461 463 468 481 509 510 521]` in a
  census run, or that the sweep's `bikesSeen` reached the two bike arms. With
  `GAME_TEST_WARMUP_FRAMES` too low (e.g. the default 100) the streamer delivers
  nothing and the run would be vacuous — the pump fix (change 1) is what avoids
  this.

## Scope of the claim (what a green does and does not establish)

- **Does** prove: our reversed `0x421440`, called at construction, sets
  `bOnSideStand` identically to the original, on every loaded bike/BMX.
- **Does not** prove: that ambient traffic organically reaches this function, nor
  that the flag has its intended downstream physics effect. Those are separate
  validations (organic differential trace; end-to-end play). See the story's
  closing section.
