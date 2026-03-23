// test_CRestart2.cpp — Deep differential tests for CRestart.
// Hook paths: Global/CRestart/<fn>
//
// Tests closest restart point lookups — these are the functions that
// determine where CJ respawns after death/arrest.

#include "StdInc.h"
#include "TestFramework.h"
#include "Restart.h"

// --- FindClosestHospitalRestartPoint ---

GAME_DIFF_TEST(CRestart, FindClosestHospitalRestartPoint_GroveStreet) {
    CVector pos(2495.0f, -1665.0f, 13.5f);
    CVector origPos, revPos;
    float origAngle, revAngle;

    { HookDisableGuard guard("Global/CRestart/FindClosestHospitalRestartPoint");
      CRestart::FindClosestHospitalRestartPoint(pos, origPos, origAngle); }
    CRestart::FindClosestHospitalRestartPoint(pos, revPos, revAngle);

    EXPECT_NEAR(origPos.x, revPos.x, 1e-3f);
    EXPECT_NEAR(origPos.y, revPos.y, 1e-3f);
    EXPECT_NEAR(origPos.z, revPos.z, 1e-3f);
    EXPECT_NEAR(origAngle, revAngle, 1e-3f);
}

GAME_DIFF_TEST(CRestart, FindClosestHospitalRestartPoint_Downtown) {
    CVector pos(1481.0f, -1764.0f, 18.8f);
    CVector origPos, revPos;
    float origAngle, revAngle;

    { HookDisableGuard guard("Global/CRestart/FindClosestHospitalRestartPoint");
      CRestart::FindClosestHospitalRestartPoint(pos, origPos, origAngle); }
    CRestart::FindClosestHospitalRestartPoint(pos, revPos, revAngle);

    EXPECT_NEAR(origPos.x, revPos.x, 1e-3f);
    EXPECT_NEAR(origPos.y, revPos.y, 1e-3f);
    EXPECT_NEAR(origAngle, revAngle, 1e-3f);
}

// --- FindClosestPoliceRestartPoint ---

GAME_DIFF_TEST(CRestart, FindClosestPoliceRestartPoint_GroveStreet) {
    CVector pos(2495.0f, -1665.0f, 13.5f);
    CVector origPos, revPos;
    float origAngle, revAngle;

    { HookDisableGuard guard("Global/CRestart/FindClosestPoliceRestartPoint");
      CRestart::FindClosestPoliceRestartPoint(pos, origPos, origAngle); }
    CRestart::FindClosestPoliceRestartPoint(pos, revPos, revAngle);

    EXPECT_NEAR(origPos.x, revPos.x, 1e-3f);
    EXPECT_NEAR(origPos.y, revPos.y, 1e-3f);
    EXPECT_NEAR(origPos.z, revPos.z, 1e-3f);
    EXPECT_NEAR(origAngle, revAngle, 1e-3f);
}

GAME_DIFF_TEST(CRestart, FindClosestPoliceRestartPoint_SanFierro) {
    CVector pos(-2170.0f, 250.0f, 35.0f);
    CVector origPos, revPos;
    float origAngle, revAngle;

    { HookDisableGuard guard("Global/CRestart/FindClosestPoliceRestartPoint");
      CRestart::FindClosestPoliceRestartPoint(pos, origPos, origAngle); }
    CRestart::FindClosestPoliceRestartPoint(pos, revPos, revAngle);

    EXPECT_NEAR(origPos.x, revPos.x, 1e-3f);
    EXPECT_NEAR(origPos.y, revPos.y, 1e-3f);
    EXPECT_NEAR(origAngle, revAngle, 1e-3f);
}

// --- OverrideNextRestart / CancelOverrideRestart ---

GAME_DIFF_TEST(CRestart, OverrideAndCancel2) {
    // Save state
    bool savedOverride = CRestart::bOverrideRestart;
    CVector savedPos = CRestart::OverridePosition;
    float savedHeading = CRestart::OverrideHeading;

    // Test override
    CVector testPos(100.0f, 200.0f, 30.0f);
    { HookDisableGuard guard("Global/CRestart/OverrideNextRestart");
      CRestart::OverrideNextRestart(testPos, 1.5f); }
    bool origOverride = CRestart::bOverrideRestart;

    // Reset and test reversed
    CRestart::bOverrideRestart = false;
    CRestart::OverrideNextRestart(testPos, 1.5f);
    bool revOverride = CRestart::bOverrideRestart;
    EXPECT_EQ(origOverride, revOverride);

    // Test cancel
    { HookDisableGuard guard("Global/CRestart/CancelOverrideRestart");
      CRestart::CancelOverrideRestart(); }
    bool origCancelled = CRestart::bOverrideRestart;

    CRestart::bOverrideRestart = true;
    CRestart::CancelOverrideRestart();
    bool revCancelled = CRestart::bOverrideRestart;
    EXPECT_EQ(origCancelled, revCancelled);

    // Restore
    CRestart::bOverrideRestart = savedOverride;
    CRestart::OverridePosition = savedPos;
    CRestart::OverrideHeading = savedHeading;
}
