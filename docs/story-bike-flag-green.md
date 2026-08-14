# How a bike finally stood on its side-stand headless

A short account of getting `test_CCarCtrl_GetNewVehicle_diff` to a meaningful
green on CT 103 — and what "green" actually asserts.

## The question

PR #1 of `paulo-sysframe/gta-reversed` reverses one function,
`CCarCtrl::GetNewVehicleDependingOnCarModel` (`0x421440`). Its one
behaviourally-load-bearing act: when it builds a `CBike`/`CBmx`, it sets
`bikeFlags.bOnSideStand = true`, reproducing the original's
`or byte ptr [eax+0x614], 0x10`. A newly created bike stands on its kickstand.

The static gate the project runs — compile + hook_ok + signature_ok — cannot see
that. It has waved through a body of `return train;`, an inverted `!`, and
reimplementations that dropped this very flag write. So the question was never
"does it compile" but "does it *behave* like the original." Answering that means
running the reversed function on a real bike and watching the flag.

## Why it wasn't already answered

To run the function on a bike, a bike model has to exist in the world. Headless,
almost nothing is streamed in. The test asks the game to stream eight bike/BMX
models via `GAME_TEST_REQUEST_MODELS`, then sweeps loaded models and compares.

That request had, it turned out, **never once delivered a model.**

## The three dead ends and the one real cause

**Iteration 0 (warmup=100):** census `loaded_vehicles=1, loaded_bikes=0`. No
bikes. The request *fired* (`requested 8 model(s)` in the log), so wiring was
fine. Something else.

**Iteration 1 (warmup=600):** identical — `1 vehicle, 0 bikes`. Six times the
warmup frames changed *nothing*. That was the tell. A timing problem gets better
with more time; this didn't move at all. So it wasn't a budget knob.

**The diagnosis.** The requested IDs were all real bikes in `vehicles.ide`
(pcj600, freeway, sanchez, bmx…). None ever `ConvertBufferToObject`'d. And the
streaming log showed only `CStreaming::Init` / `InitImageList` — **never**
`Update` or `LoadRequestedModels`. Map/object models streamed in fine through a
different, pumped path; on-demand `RequestModel` for a vehicle sat queued
forever. The warmup frame just `return`ed to "let the game process the frame
normally" — but the headless frame loop never drives the streamer's on-demand
queue. Nobody was turning the crank.

## The fix

In `game_tests.cpp`, WARMUP phase, service the queue explicitly:

```cpp
CStreaming::LoadRequestedModels();          // each warmup frame
...
CStreaming::LoadAllRequestedModels(false);  // final flush before tests run
```

These are the exact calls the game itself uses (`Pools.cpp`, `Game.cpp`). They
are safe *here* precisely because WARMUP runs **before** `SuspendOtherThreads` —
`CdStreamThread` is still alive. The deadlock the original author warned about
only bites *inside* tests, after that thread is parked. So the fix respects the
constraint instead of fighting it.

Rebuild. Re-run. Census: **`loaded_vehicles=10, loaded_bikes=8,
ids=[448 461 463 468 481 509 510 521]`.** All eight bikes, resident.

This was a genuine harness bug, not a test quirk: on-demand streaming had never
worked headless, which silently blocked *every* test that needs a streamed
model — not just this one.

## One last scaffold

The test still reported FAIL. Not a real failure — a temporary census block the
author had left in, calling `RecordFailure` unconditionally to print the census
to the results file, with a comment: *"removed once the census is answered."*
The census was now answered. Removed the block; the real sweep ran.

## The docker image

None of this touched the container. `gta-reversed-build:latest` (12.3 GB) is the
same image throughout: Wine + a **null `d3d9.dll`** + a `/dev/null`-style
framebuffer under Xvfb, running the real `gta_sa.exe` with `gta_reversed.asi`
hooked in. It was already working — it boots the game, streams the world, runs
the in-process test registry. What was broken lived in the harness C++ *inside*
the DLL, not in the image. The image never needed a fix; the diagnosis just
kept looking like it might, until the streaming log named the real culprit.

## How we assert it works

"Green" here is not "the process exited 0." The test is **differential**: for
each loaded model it runs the function twice from the same state —

```cpp
{
    HookDisableGuard guard(kHookPath);       // <- original 2004 machine code
    orig = BuildAndObserve(modelId, RANDOM_VEHICLE);
}
rev = BuildAndObserve(modelId, RANDOM_VEHICLE);  // <- our reversed C++

EXPECT_EQ(orig.created,     rev.created);      // both built a vehicle
EXPECT_EQ(orig.type,        rev.type);         // same switch arm taken
EXPECT_EQ(orig.onSideStand, rev.onSideStand);  // same flag — the write under test
```

`HookDisableGuard` temporarily un-hooks the reversed function so the *original*
machine code at `0x421440` runs; then the reversed version runs. The reference is
the original itself, never a value we hand-wrote — a test that trusts the
candidate to say what it does would just agree with the code it is meant to
check.

Then four guards make a green mean something:

| Guard | Stops the false pass where… |
| --- | --- |
| `EXPECT_GT(compared, 0)` | nothing loaded, so nothing was tested |
| `EXPECT_GT(bikesSeen, 0)` | only an automobile loaded, so the bike arm never ran |
| `EXPECT_GT(onSideStandTrue, 0)` | **our** code silently dropped the flag write |
| `EXPECT_GT(origSetFlag, 0)` | recorded separately — proves the **original** sets it too, so `EXPECT_EQ` isn't "both agree on false" |

That last pair is the crux. `EXPECT_EQ(orig.onSideStand, rev.onSideStand)` alone
would pass if *neither* side set the flag. Counting `origSetFlag` and
`onSideStandTrue` separately turns "they agree" into "they agree, and the flag
was actually set, by both."

## The result

```
PASS  CCarCtrl/Diff_GetNewVehicleDependingOnCarModel (34 assertions)
FAILED=0   STATUS=PASS
```

34 = 10 loaded vehicles × 3 `EXPECT_EQ` + 4 `EXPECT_GT` guards. A full sweep, no
early bail, reproduced across two runs. Eight bikes built through the reversed
`0x421440`; every one set `bOnSideStand` exactly as the original does.

PR #1 is not just *matched* — it is **exercised and faithful**, proven against
the original machine code on real bikes, headless, with no framebuffer.

## The principle underneath

The goal invoked the Totalitarian Principle — *what is not forbidden is
compulsory.* A bike created through this function is not forbidden, so it *must*
be reachable. But the useful reading turned out not to be "wait long enough and a
bike appears." It was: **this state is not forbidden — so find what is forbidding
it.** What forbade it was a streamer that never pumped. Remove that, and the
compulsory thing happens on the very next frame.
