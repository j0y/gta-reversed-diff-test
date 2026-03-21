// test_CGarages.cpp — Differential tests for CGarages.
// Hook paths: Global/CGarages/<fn>

#include "StdInc.h"
#include "Garages.h"
#include "TestFramework.h"

GAME_DIFF_TEST(CGarages, IsModelIndexADoor) {
    // Test various model IDs — doors vs non-doors
    int32 models[] = { 0, 1, 10, 100, 500, 1000, 1500 };
    for (int32 m : models) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CGarages/IsModelIndexADoor"); orig = CGarages::IsModelIndexADoor(m); }
        rev = CGarages::IsModelIndexADoor(m);
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(CGarages, IsPointWithinHideOutGarage) {
    CVector positions[] = {
        {2505, -1690, 13},  // Near Grove Street safehouse
        {0, 0, 0},          // Origin
        {-2100, 900, 30},   // SF garage area
    };
    for (auto& pos : positions) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CGarages/IsPointWithinHideOutGarage"); orig = CGarages::IsPointWithinHideOutGarage(pos); }
        rev = CGarages::IsPointWithinHideOutGarage(pos);
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(CGarages, IsGarageOpen) {
    for (int16 id = 0; id < 10; id++) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CGarages/IsGarageOpen"); orig = CGarages::IsGarageOpen(id); }
        rev = CGarages::IsGarageOpen(id);
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(CGarages, IsGarageClosed) {
    for (int16 id = 0; id < 10; id++) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CGarages/IsGarageClosed"); orig = CGarages::IsGarageClosed(id); }
        rev = CGarages::IsGarageClosed(id);
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(CGarages, GetGarageNumberByName) {
    const char* names[] = { "YOUGA1", "YOUGA2", "GANTON1", "NONEXISTENT_GARAGE" };
    for (auto name : names) {
        int16 orig, rev;
        { HookDisableGuard guard("Global/CGarages/GetGarageNumberByName"); orig = CGarages::GetGarageNumberByName(name); }
        rev = CGarages::GetGarageNumberByName(name);
        EXPECT_EQ(orig, rev);
    }
}
