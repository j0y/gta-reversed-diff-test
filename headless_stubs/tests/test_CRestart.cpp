// test_CRestart.cpp — Differential tests for CRestart.
// Hook paths: Global/CRestart/<fn>

#include "StdInc.h"
#include "TestFramework.h"

GAME_DIFF_TEST(CRestart, OverrideAndCancel) {
    // Test OverrideNextRestart + CancelOverrideRestart state toggle
    auto savedOverride = CRestart::bOverrideRestart;
    auto savedPos = CRestart::OverridePosition;
    auto savedAngle = CRestart::OverrideHeading;

    CVector testPos(100.f, 200.f, 10.f);

    // Original
    { HookDisableGuard guard("Global/CRestart/OverrideNextRestart");
      CRestart::OverrideNextRestart(testPos, 90.f); }
    auto origOverride = CRestart::bOverrideRestart;
    auto origPos = CRestart::OverridePosition;

    { HookDisableGuard guard("Global/CRestart/CancelOverrideRestart");
      CRestart::CancelOverrideRestart(); }
    auto origCancelled = CRestart::bOverrideRestart;

    // Reversed
    CRestart::OverrideNextRestart(testPos, 90.f);
    auto revOverride = CRestart::bOverrideRestart;
    auto revPos = CRestart::OverridePosition;

    CRestart::CancelOverrideRestart();
    auto revCancelled = CRestart::bOverrideRestart;

    // Restore
    CRestart::bOverrideRestart = savedOverride;
    CRestart::OverridePosition = savedPos;
    CRestart::OverrideHeading = savedAngle;

    EXPECT_EQ(origOverride, revOverride);
    EXPECT_VEC_NEAR(origPos, revPos, 1e-5f);
    EXPECT_EQ(origCancelled, revCancelled);
}
