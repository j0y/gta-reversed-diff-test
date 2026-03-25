// test_CStuckCarCheck2.cpp — Additional differential tests for CStuckCarCheck.
// Hook paths: Global/CStuckCarCheck/<fn>
//
// Extends test_CStuckCarCheck.cpp with diff tests for:
// - HasCarBeenStuckForAWhile with handle 0 and invalid handles
// - IsCarInStuckCarArray with handle 0 and invalid handles
// - AttemptToWarpVehicle with spawned vehicle

#include "StdInc.h"
#include "TestFramework.h"
#include "StuckCarCheck.h"

static auto& StuckCars2 = CTheScripts::StuckCars;

// --- HasCarBeenStuckForAWhile with edge-case handles ---

GAME_DIFF_TEST(CStuckCarCheck, HasCarBeenStuckForAWhile_HandleZero) {
    bool orig, rev;
    { HookDisableGuard guard("Global/CStuckCarCheck/HasCarBeenStuckForAWhile");
      orig = StuckCars2.HasCarBeenStuckForAWhile(0); }
    rev = StuckCars2.HasCarBeenStuckForAWhile(0);
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CStuckCarCheck, HasCarBeenStuckForAWhile_NegativeHandle) {
    bool orig, rev;
    { HookDisableGuard guard("Global/CStuckCarCheck/HasCarBeenStuckForAWhile");
      orig = StuckCars2.HasCarBeenStuckForAWhile(-1); }
    rev = StuckCars2.HasCarBeenStuckForAWhile(-1);
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CStuckCarCheck, HasCarBeenStuckForAWhile_LargeHandle) {
    bool orig, rev;
    { HookDisableGuard guard("Global/CStuckCarCheck/HasCarBeenStuckForAWhile");
      orig = StuckCars2.HasCarBeenStuckForAWhile(999999); }
    rev = StuckCars2.HasCarBeenStuckForAWhile(999999);
    EXPECT_EQ(orig, rev);
}

// --- IsCarInStuckCarArray with edge-case handles ---

GAME_DIFF_TEST(CStuckCarCheck, IsCarInStuckCarArray_HandleZero) {
    bool orig, rev;
    { HookDisableGuard guard("Global/CStuckCarCheck/IsCarInStuckCarArray");
      orig = StuckCars2.IsCarInStuckCarArray(0); }
    rev = StuckCars2.IsCarInStuckCarArray(0);
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CStuckCarCheck, IsCarInStuckCarArray_NegativeHandle) {
    bool orig, rev;
    { HookDisableGuard guard("Global/CStuckCarCheck/IsCarInStuckCarArray");
      orig = StuckCars2.IsCarInStuckCarArray(-1); }
    rev = StuckCars2.IsCarInStuckCarArray(-1);
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CStuckCarCheck, IsCarInStuckCarArray_LargeHandle) {
    bool orig, rev;
    { HookDisableGuard guard("Global/CStuckCarCheck/IsCarInStuckCarArray");
      orig = StuckCars2.IsCarInStuckCarArray(999999); }
    rev = StuckCars2.IsCarInStuckCarArray(999999);
    EXPECT_EQ(orig, rev);
}

// --- IsCarInStuckCarArray with synthetic slot containing handle 0 ---

GAME_DIFF_TEST(CStuckCarCheck, IsCarInStuckCarArray_HandleZeroInSlot) {
    CStuckCarCheck backup = StuckCars2;

    // Insert handle 0 into a slot — verify both paths find it
    StuckCars2.m_aStuckCars[7].CarHandle = 0;

    bool orig, rev;
    rev = StuckCars2.IsCarInStuckCarArray(0);
    { HookDisableGuard guard("Global/CStuckCarCheck/IsCarInStuckCarArray");
      orig = StuckCars2.IsCarInStuckCarArray(0); }
    EXPECT_EQ(orig, rev);

    StuckCars2 = backup;
}

// --- HasCarBeenStuckForAWhile with synthetic stuck entry ---

GAME_DIFF_TEST(CStuckCarCheck, HasCarBeenStuckForAWhile_HandleZeroStuck) {
    CStuckCarCheck backup = StuckCars2;

    StuckCars2.m_aStuckCars[7].CarHandle = 0;
    StuckCars2.m_aStuckCars[7].CarStuck = true;

    bool orig, rev;
    rev = StuckCars2.HasCarBeenStuckForAWhile(0);
    { HookDisableGuard guard("Global/CStuckCarCheck/HasCarBeenStuckForAWhile");
      orig = StuckCars2.HasCarBeenStuckForAWhile(0); }
    EXPECT_EQ(orig, rev);

    StuckCars2 = backup;
}

// --- AttemptToWarpVehicle ---

GAME_DIFF_TEST(CStuckCarCheck, AttemptToWarpVehicle_NullVehicle) {
    // With nullptr vehicle, both code paths should agree (likely early-out false)
    CVector pos(2488.0f, -1666.0f, 13.0f);
    bool orig, rev;
    { HookDisableGuard guard("Global/CStuckCarCheck/AttemptToWarpVehicle");
      orig = StuckCars2.AttemptToWarpVehicle(nullptr, &pos, 0.0f); }
    rev = StuckCars2.AttemptToWarpVehicle(nullptr, &pos, 0.0f);
    EXPECT_EQ(orig, rev);
}

// AttemptToWarpVehicle(nullptr, nullptr, 0) crashes original code — skip
