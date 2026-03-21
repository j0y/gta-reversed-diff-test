// test_CStuckCarCheck.cpp — Behavior + differential tests for CStuckCarCheck.
// Hook paths: Global/CStuckCarCheck/<fn>
//
// CStuckCarCheck manages an array of 16 tStuckCar entries tracking
// whether script vehicles are stuck and should be warped.
// Most methods are simple array lookups/modifications.

#include "StdInc.h"
#include "TestFramework.h"
#include "StuckCarCheck.h"

// CTheScripts::StuckCars is the game's global instance
static auto& StuckCars = CTheScripts::StuckCars;

// --- Behavior tests ---

GAME_TEST(CStuckCarCheck, Init_ClearsAllSlots) {
    // After Init, all slots should have CarHandle == -1
    // Save and restore state since we're modifying global
    CStuckCarCheck backup = StuckCars;

    StuckCars.Init();
    for (int i = 0; i < STUCK_CAR_CHECK_SIZE; i++) {
        EXPECT_EQ(StuckCars.m_aStuckCars[i].CarHandle, -1);
        EXPECT_EQ(StuckCars.m_aStuckCars[i].CarStuck, false);
    }

    StuckCars = backup;
}

GAME_TEST(CStuckCarCheck, IsCarInStuckCarArray_NotPresent) {
    // A handle that's not in the array should return false
    // Use a handle that's extremely unlikely to be present
    EXPECT_FALSE(StuckCars.IsCarInStuckCarArray(0x7FFFFFFF));
}

GAME_TEST(CStuckCarCheck, HasCarBeenStuckForAWhile_NotPresent) {
    // A handle not in the array should return false
    EXPECT_FALSE(StuckCars.HasCarBeenStuckForAWhile(0x7FFFFFFF));
}

GAME_TEST(CStuckCarCheck, InitAddRemove_Roundtrip) {
    // Test the full lifecycle: init -> add -> check -> remove -> check
    CStuckCarCheck backup = StuckCars;

    StuckCars.Init();

    // Nothing should be present
    EXPECT_FALSE(StuckCars.IsCarInStuckCarArray(12345));

    // We can't use AddCarToCheck with a real handle (needs valid pool entry),
    // but we can manually insert and test the lookup functions.
    StuckCars.m_aStuckCars[0].CarHandle = 12345;
    StuckCars.m_aStuckCars[0].CarStuck = false;

    EXPECT_TRUE(StuckCars.IsCarInStuckCarArray(12345));
    EXPECT_FALSE(StuckCars.HasCarBeenStuckForAWhile(12345));

    // Set stuck flag
    StuckCars.m_aStuckCars[0].CarStuck = true;
    EXPECT_TRUE(StuckCars.HasCarBeenStuckForAWhile(12345));

    // Clear stuck flag
    StuckCars.ClearStuckFlagForCar(12345);
    EXPECT_FALSE(StuckCars.HasCarBeenStuckForAWhile(12345));
    EXPECT_TRUE(StuckCars.IsCarInStuckCarArray(12345)); // still in array

    // Remove from array
    StuckCars.RemoveCarFromCheck(12345);
    EXPECT_FALSE(StuckCars.IsCarInStuckCarArray(12345));

    StuckCars = backup;
}

GAME_TEST(CStuckCarCheck, ClearStuckFlag_OnlyAffectsTarget) {
    CStuckCarCheck backup = StuckCars;

    StuckCars.Init();
    StuckCars.m_aStuckCars[0].CarHandle = 100;
    StuckCars.m_aStuckCars[0].CarStuck = true;
    StuckCars.m_aStuckCars[1].CarHandle = 200;
    StuckCars.m_aStuckCars[1].CarStuck = true;

    StuckCars.ClearStuckFlagForCar(100);

    EXPECT_FALSE(StuckCars.m_aStuckCars[0].CarStuck);
    EXPECT_TRUE(StuckCars.m_aStuckCars[1].CarStuck); // untouched

    StuckCars = backup;
}

GAME_TEST(CStuckCarCheck, RemoveCarFromCheck_OnlyAffectsTarget) {
    CStuckCarCheck backup = StuckCars;

    StuckCars.Init();
    StuckCars.m_aStuckCars[0].CarHandle = 100;
    StuckCars.m_aStuckCars[1].CarHandle = 200;

    StuckCars.RemoveCarFromCheck(100);

    EXPECT_EQ(StuckCars.m_aStuckCars[0].CarHandle, -1); // removed
    EXPECT_EQ(StuckCars.m_aStuckCars[1].CarHandle, 200); // untouched

    StuckCars = backup;
}

// --- Differential tests ---
// These compare reversed vs original code using the actual game state.

GAME_DIFF_TEST(CStuckCarCheck, IsCarInStuckCarArray) {
    // Test with current game state — check each slot's handle
    for (int i = 0; i < STUCK_CAR_CHECK_SIZE; i++) {
        int32 handle = StuckCars.m_aStuckCars[i].CarHandle;
        if (handle == -1) continue;

        bool rev = StuckCars.IsCarInStuckCarArray(handle);
        bool orig;
        { HookDisableGuard guard("Global/CStuckCarCheck/IsCarInStuckCarArray");
          orig = StuckCars.IsCarInStuckCarArray(handle); }
        EXPECT_EQ(rev, orig);
    }
    // Also test a handle that shouldn't be there
    {
        bool rev = StuckCars.IsCarInStuckCarArray(0x7FFFFFFF);
        bool orig;
        { HookDisableGuard guard("Global/CStuckCarCheck/IsCarInStuckCarArray");
          orig = StuckCars.IsCarInStuckCarArray(0x7FFFFFFF); }
        EXPECT_EQ(rev, orig);
    }
}

GAME_DIFF_TEST(CStuckCarCheck, HasCarBeenStuckForAWhile) {
    for (int i = 0; i < STUCK_CAR_CHECK_SIZE; i++) {
        int32 handle = StuckCars.m_aStuckCars[i].CarHandle;
        if (handle == -1) continue;

        bool rev = StuckCars.HasCarBeenStuckForAWhile(handle);
        bool orig;
        { HookDisableGuard guard("Global/CStuckCarCheck/HasCarBeenStuckForAWhile");
          orig = StuckCars.HasCarBeenStuckForAWhile(handle); }
        EXPECT_EQ(rev, orig);
    }
    {
        bool rev = StuckCars.HasCarBeenStuckForAWhile(0x7FFFFFFF);
        bool orig;
        { HookDisableGuard guard("Global/CStuckCarCheck/HasCarBeenStuckForAWhile");
          orig = StuckCars.HasCarBeenStuckForAWhile(0x7FFFFFFF); }
        EXPECT_EQ(rev, orig);
    }
}

GAME_DIFF_TEST(CStuckCarCheck, ClearStuckFlagForCar) {
    // Test with a synthetic setup — save/restore state
    CStuckCarCheck backup = StuckCars;

    StuckCars.Init();
    StuckCars.m_aStuckCars[3].CarHandle = 99999;
    StuckCars.m_aStuckCars[3].CarStuck = true;

    // Reversed version
    StuckCars.ClearStuckFlagForCar(99999);
    bool revResult = StuckCars.m_aStuckCars[3].CarStuck;

    // Reset and run original
    StuckCars.m_aStuckCars[3].CarStuck = true;
    { HookDisableGuard guard("Global/CStuckCarCheck/ClearStuckFlagForCar");
      StuckCars.ClearStuckFlagForCar(99999); }
    bool origResult = StuckCars.m_aStuckCars[3].CarStuck;

    EXPECT_EQ(revResult, origResult);

    StuckCars = backup;
}

GAME_DIFF_TEST(CStuckCarCheck, RemoveCarFromCheck) {
    CStuckCarCheck backup = StuckCars;

    StuckCars.Init();
    StuckCars.m_aStuckCars[5].CarHandle = 88888;

    // Reversed version
    StuckCars.RemoveCarFromCheck(88888);
    int32 revHandle = StuckCars.m_aStuckCars[5].CarHandle;

    // Reset and run original
    StuckCars.m_aStuckCars[5].CarHandle = 88888;
    { HookDisableGuard guard("Global/CStuckCarCheck/RemoveCarFromCheck");
      StuckCars.RemoveCarFromCheck(88888); }
    int32 origHandle = StuckCars.m_aStuckCars[5].CarHandle;

    EXPECT_EQ(revHandle, origHandle);

    StuckCars = backup;
}
