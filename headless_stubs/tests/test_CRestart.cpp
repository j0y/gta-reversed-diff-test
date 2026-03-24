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

// --- Initialise ---

GAME_DIFF_TEST(CRestart, Initialise) {
    // Save a few key fields
    auto savedOverride = CRestart::bOverrideRestart;
    auto savedNumHospital = CRestart::NumberOfHospitalRestarts;
    auto savedNumPolice = CRestart::NumberOfPoliceRestarts;

    { HookDisableGuard guard("Global/CRestart/Initialise");
      CRestart::Initialise(); }
    auto origNumH = CRestart::NumberOfHospitalRestarts;
    auto origNumP = CRestart::NumberOfPoliceRestarts;

    CRestart::NumberOfHospitalRestarts = savedNumHospital;
    CRestart::NumberOfPoliceRestarts = savedNumPolice;
    CRestart::Initialise();
    auto revNumH = CRestart::NumberOfHospitalRestarts;
    auto revNumP = CRestart::NumberOfPoliceRestarts;

    EXPECT_EQ(origNumH, revNumH);
    EXPECT_EQ(origNumP, revNumP);

    // Restore
    CRestart::NumberOfHospitalRestarts = savedNumHospital;
    CRestart::NumberOfPoliceRestarts = savedNumPolice;
    CRestart::bOverrideRestart = savedOverride;
}

// SetRespawnPointForDurationOfMission takes different args. Skipped.
