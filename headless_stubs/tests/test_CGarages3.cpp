// test_CGarages3.cpp — Deep differential tests for CGarages query functions.
// Hook paths: Global/CGarages/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "Garages.h"

// --- IsModelIndexADoor ---

GAME_DIFF_TEST(CGarages3, IsModelIndexADoor_Various) {
    int32 models[] = { 0, 1, 100, 400, 500, 1000, 1500, 2000, 3000, 5000 };
    for (int32 m : models) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CGarages/IsModelIndexADoor"); orig = CGarages::IsModelIndexADoor(m); }
        rev = CGarages::IsModelIndexADoor(m);
        EXPECT_EQ(orig, rev);
    }
}

// --- IsPointWithinHideOutGarage ---

GAME_DIFF_TEST(CGarages3, IsPointWithinHideOutGarage_GroveStreet) {
    CVector pos(2495.0f, -1665.0f, 13.5f);
    bool orig, rev;
    { HookDisableGuard guard("Global/CGarages/IsPointWithinHideOutGarage"); orig = CGarages::IsPointWithinHideOutGarage(pos); }
    rev = CGarages::IsPointWithinHideOutGarage(pos);
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CGarages3, IsPointWithinHideOutGarage_Random) {
    CVector positions[] = {
        {0.0f, 0.0f, 0.0f}, {1000.0f, -1000.0f, 20.0f}, {-2000.0f, 500.0f, 30.0f},
    };
    for (auto& pos : positions) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CGarages/IsPointWithinHideOutGarage"); orig = CGarages::IsPointWithinHideOutGarage(pos); }
        rev = CGarages::IsPointWithinHideOutGarage(pos);
        EXPECT_EQ(orig, rev);
    }
}

// --- IsGarageOpen / IsGarageClosed ---

GAME_DIFF_TEST(CGarages3, IsGarageOpen_Various) {
    int16 indices[] = { 0, 1, 5, 10, 20 };
    for (int16 i : indices) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CGarages/IsGarageOpen"); orig = CGarages::IsGarageOpen(i); }
        rev = CGarages::IsGarageOpen(i);
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(CGarages3, IsGarageClosed_Various) {
    int16 indices[] = { 0, 1, 5, 10, 20 };
    for (int16 i : indices) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CGarages/IsGarageClosed"); orig = CGarages::IsGarageClosed(i); }
        rev = CGarages::IsGarageClosed(i);
        EXPECT_EQ(orig, rev);
    }
}

// --- IsCarSprayable ---

// IsCarSprayable(nullptr) crashes in original code — needs valid vehicle
GAME_DIFF_TEST(CGarages3, IsCarSprayable) {
    auto* veh = FindPlayerVehicle(0);
    if (!veh) return; // skip if player not in vehicle
    bool orig, rev;
    { HookDisableGuard guard("Global/CGarages/IsCarSprayable"); orig = CGarages::IsCarSprayable(veh); }
    rev = CGarages::IsCarSprayable(veh);
    EXPECT_EQ(orig, rev);
}

// --- GetGarageNumberByName ---

GAME_DIFF_TEST(CGarages3, GetGarageNumberByName_Various) {
    const char* names[] = { "MODLAST", "BURGL1", "HANGAR", "SPRAY1", "INVALID_XYZ" };
    for (auto* name : names) {
        int16 orig, rev;
        { HookDisableGuard guard("Global/CGarages/GetGarageNumberByName"); orig = CGarages::GetGarageNumberByName(name); }
        rev = CGarages::GetGarageNumberByName(name);
        EXPECT_EQ(orig, rev);
    }
}
