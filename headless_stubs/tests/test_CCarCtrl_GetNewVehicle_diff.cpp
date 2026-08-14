// test_CCarCtrl_GetNewVehicle_diff.cpp -- behavioural test for
// CCarCtrl::GetNewVehicleDependingOnCarModel @ 0x421440.
//
// A SEPARATE FILE from test_CCarCtrl.cpp, deliberately: that file holds six
// working tests and an upstream update must not silently drop ours, nor our
// mistake break theirs. build-tests.sh globs test_*.cpp so the name is enough.
//
// WHY THIS FUNCTION
//
// It is the one the C5 run reversed and PR #1 proposes upstream, and it has
// never been executed. The gate it passed is static -- compile + hook_ok +
// signature_ok -- and the project's own record shows that gate accepting a body
// of `return train;` (run 7) and an inverted `!` (upstream #1247).
//
// It is also the sharper case. Runs C4 and C5 passed the static gate on this
// address while performing NEITHER of the two flag writes the original
// performs, and run C3 wrote a REAL field (`vehicleFlags.bUseCarCheats`) at an
// impossible offset -- CVehicle ends at 0x5A0, 116 bytes short of the 0x614
// this function writes. An invented name fails at compile; a misplaced real one
// compiles, hooks, passes the signature check, and is wrong at runtime.
//
// WHAT IS ACTUALLY BEING COMPARED
//
// The function's whole observable effect is the OBJECT it returns:
//
//   1. the concrete type dispatched to (the switch), and
//   2. bikeFlags.bOnSideStand on the two bike arms (the writes at 0x42151B
//      and 0x42155C).
//
// So the comparison is on the vehicle's type and flag state, not on a return
// value. m_nVehicleType identifies the arm taken without RTTI, which the game
// is built without.
//
// THE THIRD WRITE IS NOT TESTED, AND CANNOT BE
//
// There is a third `or byte ptr [eax + 0x614], 0x10` at 0x421575. It is the
// `operator new` NULL-return arm: 0x42150C and 0x42154D both `je` there after
// `test eax, eax`, and 0x421573 zeroes eax before the write. The original's C++
// set the flag without checking new for NULL, so 2004 MSVC emitted the write on
// both paths. Modern C++ cannot reach it -- new throws rather than returning
// NULL -- so a faithful reimplementation performs TWO writes, and no test can
// exercise the third without forcing an allocation failure.
//
// EACH CALL ALLOCATES
//
// Unlike a scalar-returning diff test, every call here news a vehicle. They are
// not added to CWorld, so `delete` is the whole cleanup, but it must happen on
// every path or a sweep leaks a vehicle per iteration and perturbs the pool the
// next test observes.
//
// A NULL RETURN IS NOT A PASS
//
// If a model is not loaded, both sides may return nullptr and agree vacuously.
// The test records how many comparisons actually carried a vehicle and fails if
// that count is zero -- the same reasoning as the difftest-agent refusing to
// read a missing results file as a pass.

#include "StdInc.h"
#include "TestFramework.h"
#include "ScenarioHelpers.h"

namespace {

constexpr const char* kHookPath = "Global/CCarCtrl/GetNewVehicleDependingOnCarModel";

// What the function is observed to produce. Compared field by field so a
// failure names which half diverged rather than just "not equal".
struct Built {
    bool  created;      // did it return anything at all
    int32 type;         // m_nVehicleType -- which switch arm was taken
    bool  isBikeish;    // BIKE or BMX: the two arms carrying the write
    bool  onSideStand;  // bikeFlags.bOnSideStand -- the write under test
};

Built Observe(CVehicle* v) {
    Built b{};
    if (!v) {
        return b;
    }
    b.created = true;
    b.type    = static_cast<int32>(v->m_nVehicleType);

    // bikeFlags lives on CBike. Reading it off anything else would read past
    // the object on, say, a CBoat -- so gate on the type, not on the pointer.
    // Both arms matter: CBmx derives from CBike and 0x42155C is its write.
    b.isBikeish = v->IsBike() || v->IsBMX();
    if (b.isBikeish) {
        b.onSideStand = static_cast<CBike*>(v)->bikeFlags.bOnSideStand;
    }
    return b;
}

// Build once through whichever implementation is installed, observe it, and
// free it. Returning the observation rather than the pointer keeps the lifetime
// in one place.
Built BuildAndObserve(int32 modelId, uint8 createdBy) {
    CVehicle* v = CCarCtrl::GetNewVehicleDependingOnCarModel(modelId, createdBy);
    Built b = Observe(v);
    delete v;   // not CWorld::Add'ed, so this is the whole cleanup
    return b;
}


// Bike/BMX model IDs, for reference and for the run script's
// GAME_TEST_REQUEST_MODELS list. The two arms under test are reachable no other
// way: at game state 9 under FastLoader almost nothing is streamed in, and the
// first version of this test compared exactly ONE model (an automobile), so
// bOnSideStand was never touched and it passed anyway.
//
// This file deliberately does NOT request them itself. Tests run inside
// SuspendOtherThreads, which parks CdStreamThread, so any streaming call from
// here deadlocks -- measured 2026-08-12, LoadAllRequestedModels entered and
// never returned at both a 200s and a 600s budget, stopping at the identical
// iteration both times. The request happens during WARMUP instead (see
// RequestTestModels in game_tests.cpp), where the frame loop still pumps the
// streamer.
//
//   GAME_TEST_REQUEST_MODELS=481,509,510,448,461,463,468,521
//
// Types are VERIFIED at runtime below rather than trusted, so a wrong ID
// degrades coverage instead of testing the wrong thing.

} // namespace

GAME_DIFF_TEST(CCarCtrl, GetNewVehicleDependingOnCarModel) {
    // Sweep every loaded vehicle model rather than one: the interesting arms
    // are BIKE and BMX, and which models are streamed in at game state 9 is not
    // under this test's control. A single model would probably be an
    // automobile and would never reach the write being tested.
    int32 compared = 0;
    int32 bikesSeen = 0;
    int32 onSideStandTrue = 0;
    int32 origSetFlag = 0;   // the ORIGINAL's own behaviour, recorded separately

    // Census FIRST. If no bike is loaded the sweep below proves nothing, and a
    // bare assertion failure would not say WHY. Naming which of the requested
    // models arrived distinguishes "the streamer never ran" from "it ran and
    // these specific IDs were not among what it delivered".
    // (census scaffold removed: the streamed-model question is answered --
    // 8 bike models load with the WARMUP streamer pump. The real sweep follows.)

    for (int32 modelId = 400; modelId <= 611; modelId++) {
        if (!CStreaming::IsModelLoaded(modelId)) {
            continue;
        }

        // Original first, then reversed, on the same model.
        Built orig;
        {
            HookDisableGuard guard(kHookPath);
            orig = BuildAndObserve(modelId, RANDOM_VEHICLE);
        }
        Built rev = BuildAndObserve(modelId, RANDOM_VEHICLE);

        EXPECT_EQ(orig.created, rev.created);
        if (!orig.created || !rev.created) {
            continue;   // nothing built on either side; nothing to compare
        }

        // Which switch arm was taken.
        EXPECT_EQ(orig.type, rev.type);

        // The write this function was reversed for. Only meaningful on the bike
        // arms; on every other type both sides read false and it is a no-op.
        EXPECT_EQ(orig.onSideStand, rev.onSideStand);

        compared++;
        if (orig.isBikeish) {
            bikesSeen++;
            // The write under test actually fired. Counted so a pass can be
            // distinguished from "agreed that nothing happened".
            if (rev.onSideStand) {
                onSideStandTrue++;
            }
            // Recorded SEPARATELY from the reversed side. EXPECT_EQ only proves
            // the two agree; it cannot tell "both set it" from "neither did".
            // This is the evidence that the write is the ORIGINAL's behaviour,
            // which is the claim PR #1 actually makes.
            if (orig.onSideStand) {
                origSetFlag++;
            }
        }
    }

    // Vacuous-pass guards. A green result must mean something was compared AND
    // that the write this function was reversed for actually executed.
    //
    // compared > 0: nothing loaded means nothing was tested.
    EXPECT_GT(compared, 0);

    // bikesSeen > 0: an EXPECT rather than a printed note, because the run
    // script requests eight bike/BMX models via GAME_TEST_REQUEST_MODELS. If
    // none arrives, the streamer never delivered them and the two arms at
    // 0x42151B and 0x42155C were never entered -- so the run says nothing about
    // PR #1's actual change. The first version of this test passed with 4
    // assertions (one automobile) and looked green.
    EXPECT_GT(bikesSeen, 0);

    // onSideStandTrue > 0: the reversed side must have SET the flag on at least
    // one bike. Without this, a candidate that dropped both writes would still
    // pass -- the original would read false, the candidate would read false,
    // and EXPECT_EQ would agree. That is exactly the C4/C5 defect: they passed
    // the static gate while performing neither write.
    EXPECT_GT(onSideStandTrue, 0);

    // origSetFlag > 0: the ORIGINAL machine code at 0x421440 sets the flag on
    // at least one bike. This is the load-bearing assertion for the PR: it
    // demonstrates the two writes at 0x42151B/0x42155C are real behaviour
    // observed at runtime, not an inference from the disassembly that both
    // sides happen to share.
    EXPECT_GT(origSetFlag, 0);

    printf("[GetNewVehicleDependingOnCarModel] compared=%d bikes=%d "
           "orig_set=%d rev_set=%d\n",
           compared, bikesSeen, origSetFlag, onSideStandTrue);
}
