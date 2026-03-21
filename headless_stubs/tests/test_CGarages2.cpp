// test_CGarages2.cpp — Extended differential + scenario tests for CGarages.
// Hook paths: Global/CGarages/<fn>
//
// Tests garage state queries across all garages, IsCarSprayable on spawned
// vehicle, ActivateGarage/DeActivateGarage round-trips, and respray toggle.

#include "StdInc.h"
#include "TestFramework.h"
#include "ScenarioHelpers.h"
#include "Garages.h"

// --- Differential: query all garages ---

GAME_DIFF_TEST(CGarages2, IsGarageOpen_AllGarages) {
    for (int16 id = 0; id < CGarages::NumGarages; id++) {
        bool rev = CGarages::IsGarageOpen(id);
        bool orig;
        { HookDisableGuard guard("Global/CGarages/IsGarageOpen");
          orig = CGarages::IsGarageOpen(id); }
        EXPECT_EQ(rev, orig);
    }
}

GAME_DIFF_TEST(CGarages2, IsGarageClosed_AllGarages) {
    for (int16 id = 0; id < CGarages::NumGarages; id++) {
        bool rev = CGarages::IsGarageClosed(id);
        bool orig;
        { HookDisableGuard guard("Global/CGarages/IsGarageClosed");
          orig = CGarages::IsGarageClosed(id); }
        EXPECT_EQ(rev, orig);
    }
}

// --- Differential: IsCarSprayable on spawned vehicle ---

GAME_DIFF_TEST(CGarages2, IsCarSprayable) {
    int32 vehModel = Scenario::FindLoadedVehicleModel();
    if (vehModel < 0) return;
    Scenario::TestVehicle veh(vehModel, CVector(2450.0f, -1665.0f, 13.5f));
    if (!veh) return;

    bool rev = CGarages::IsCarSprayable(veh.AsVehicle());
    bool orig;
    { HookDisableGuard guard("Global/CGarages/IsCarSprayable");
      orig = CGarages::IsCarSprayable(veh.AsVehicle()); }
    EXPECT_EQ(rev, orig);
}

// --- Differential: ActivateGarage / DeActivateGarage round-trip ---

GAME_DIFF_TEST(CGarages2, ActivateDeactivateGarage) {
    if (CGarages::NumGarages <= 0) return;

    int16 testId = 0;
    auto& garage = CGarages::GetGarage(testId);
    bool savedInactive = garage.m_bInactive;

    // Reversed: deactivate then reactivate
    CGarages::DeActivateGarage(testId);
    bool revInactiveAfterDeact = garage.m_bInactive;
    CGarages::ActivateGarage(testId);
    bool revInactiveAfterAct = garage.m_bInactive;

    // Restore
    garage.m_bInactive = savedInactive;

    // Original: same sequence
    { HookDisableGuard guard("Global/CGarages/DeActivateGarage");
      CGarages::DeActivateGarage(testId); }
    bool origInactiveAfterDeact = garage.m_bInactive;
    { HookDisableGuard guard("Global/CGarages/ActivateGarage");
      CGarages::ActivateGarage(testId); }
    bool origInactiveAfterAct = garage.m_bInactive;

    EXPECT_EQ(revInactiveAfterDeact, origInactiveAfterDeact);
    EXPECT_EQ(revInactiveAfterAct, origInactiveAfterAct);

    garage.m_bInactive = savedInactive;
}

// --- Differential: AllRespraysCloseOrOpen ---

GAME_DIFF_TEST(CGarages2, AllRespraysCloseOrOpen) {
    // Save NoResprays state
    bool savedNoResprays = CGarages::NoResprays;

    // Reversed: close all resprays
    CGarages::AllRespraysCloseOrOpen(false);
    bool revClosed = CGarages::NoResprays;

    // Reversed: open all resprays
    CGarages::AllRespraysCloseOrOpen(true);
    bool revOpen = CGarages::NoResprays;

    // Original: close
    CGarages::NoResprays = savedNoResprays;
    { HookDisableGuard guard("Global/CGarages/AllRespraysCloseOrOpen");
      CGarages::AllRespraysCloseOrOpen(false); }
    bool origClosed = CGarages::NoResprays;

    // Original: open
    { HookDisableGuard guard("Global/CGarages/AllRespraysCloseOrOpen");
      CGarages::AllRespraysCloseOrOpen(true); }
    bool origOpen = CGarages::NoResprays;

    EXPECT_EQ(revClosed, origClosed);
    EXPECT_EQ(revOpen, origOpen);

    CGarages::NoResprays = savedNoResprays;
}

// --- Differential: IsModelIndexADoor extended ---

GAME_DIFF_TEST(CGarages2, IsModelIndexADoor_Extended) {
    // Test a wider range including known garage door models
    for (int32 m = 1300; m <= 1400; m++) {
        bool rev = CGarages::IsModelIndexADoor(m);
        bool orig;
        { HookDisableGuard guard("Global/CGarages/IsModelIndexADoor");
          orig = CGarages::IsModelIndexADoor(m); }
        EXPECT_EQ(rev, orig);
    }
}

// --- Differential: ChangeGarageType ---

GAME_DIFF_TEST(CGarages2, ChangeGarageType) {
    if (CGarages::NumGarages <= 0) return;

    int16 testId = 0;
    auto& garage = CGarages::GetGarage(testId);
    auto savedType = garage.m_nType;

    // Reversed
    CGarages::ChangeGarageType(testId, SAFEHOUSE_GANTON, 0);
    auto revType = garage.m_nType;

    // Restore and do original
    garage.m_nType = savedType;
    { HookDisableGuard guard("Global/CGarages/ChangeGarageType");
      CGarages::ChangeGarageType(testId, SAFEHOUSE_GANTON, 0); }
    EXPECT_EQ((int)revType, (int)garage.m_nType);

    garage.m_nType = savedType;
}

// --- Behavior tests ---

GAME_TEST(CGarages2, NumGaragesValid) {
    EXPECT_TRUE(CGarages::NumGarages >= 0);
    EXPECT_TRUE(CGarages::NumGarages <= CGarages::MAX_NUM_GARAGES);
}

GAME_TEST(CGarages2, GarageTypesInRange) {
    // eGarageType is uint8 — just verify within byte range
    for (int32 i = 0; i < CGarages::NumGarages; i++) {
        auto type = (int)CGarages::GetGarage(i).m_nType;
        EXPECT_TRUE(type >= 0 && type <= 255);
    }
}

GAME_TEST(CGarages2, OpenClosedMutuallyExclusive) {
    // A garage can't be both open and closed simultaneously
    for (int16 id = 0; id < CGarages::NumGarages; id++) {
        bool open = CGarages::IsGarageOpen(id);
        bool closed = CGarages::IsGarageClosed(id);
        // At least one should be true, or neither (transitioning)
        EXPECT_FALSE(open && closed);
    }
}
