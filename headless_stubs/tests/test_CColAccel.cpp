// test_CColAccel.cpp — Differential tests for CColAccel.
// Hook paths: Collision/CColAccel/<fn>
//
// Collision acceleration cache queries.

#include "StdInc.h"
#include "TestFramework.h"
#include "Collision/ColAccel.h"

// --- isCacheLoading ---

GAME_DIFF_TEST(CColAccel, isCacheLoading) {
    bool orig, rev;
    { HookDisableGuard guard("Collision/CColAccel/isCacheLoading");
      orig = CColAccel::isCacheLoading(); }
    rev = CColAccel::isCacheLoading();
    EXPECT_EQ(orig, rev);
}

// getIplDef and getColDef crash in original code at state 9 —
// the collision cache is not in the expected state after init.
// Only isCacheLoading is safe to call differentially.
