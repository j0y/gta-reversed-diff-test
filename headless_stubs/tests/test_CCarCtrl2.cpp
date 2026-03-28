// test_CCarCtrl2.cpp — Extended differential tests for CCarCtrl.
// Hook paths: Global/CCarCtrl/<fn>
//
// Traffic AI controller. Tests query functions, vehicle interest list,
// steer angle calculation, and parking detection.

#include "StdInc.h"
#include "TestFramework.h"
#include "ScenarioHelpers.h"
#include "CarCtrl.h"


// --- Differential tests ---

GAME_DIFF_TEST(CCarCtrl2, IsAnyoneParking) {
    bool rev = CCarCtrl::IsAnyoneParking();
    bool orig;
    { HookDisableGuard guard("Global/CCarCtrl/IsAnyoneParking");
      orig = CCarCtrl::IsAnyoneParking(); }
    EXPECT_EQ(rev, orig);
}

GAME_DIFF_TEST(CCarCtrl2, IsThisVehicleInteresting_Spawned) {
    int32 vehModel = Scenario::FindLoadedVehicleModel();
    if (vehModel < 0) return;
    Scenario::TestVehicle veh(vehModel, CVector(2420.0f, -1665.0f, 13.5f));
    if (!veh) return;

    // Freshly spawned vehicle should not be interesting
    bool rev = CCarCtrl::IsThisVehicleInteresting(veh.AsVehicle());
    bool orig;
    { HookDisableGuard guard("Global/CCarCtrl/IsThisVehicleInteresting");
      orig = CCarCtrl::IsThisVehicleInteresting(veh.AsVehicle()); }
    EXPECT_EQ(rev, orig);
    EXPECT_FALSE(rev);
}

GAME_DIFF_TEST(CCarCtrl2, FindMaxSteerAngle_Stationary) {
    int32 vehModel = Scenario::FindLoadedVehicleModel();
    if (vehModel < 0) return;
    Scenario::TestVehicle veh(vehModel, CVector(2415.0f, -1665.0f, 13.5f));
    if (!veh) return;

    // Stationary vehicle — speed magnitude = 0, should get max steer (0.7)
    veh->GetMoveSpeed() = CVector(0, 0, 0);
    float rev = CCarCtrl::FindMaxSteerAngle(veh.AsVehicle());
    float orig;
    { HookDisableGuard guard("Global/CCarCtrl/FindMaxSteerAngle");
      orig = CCarCtrl::FindMaxSteerAngle(veh.AsVehicle()); }
    EXPECT_NEAR(rev, orig, 1e-5f);
}

GAME_DIFF_TEST(CCarCtrl2, FindMaxSteerAngle_Moving) {
    int32 vehModel = Scenario::FindLoadedVehicleModel();
    if (vehModel < 0) return;
    Scenario::TestVehicle veh(vehModel, CVector(2410.0f, -1665.0f, 13.5f));
    if (!veh) return;

    // Test at various speeds
    CVector speeds[] = {
        {0.1f, 0, 0}, {0.3f, 0, 0}, {0.5f, 0, 0}, {0.7f, 0, 0}, {1.0f, 0, 0}
    };
    for (auto& spd : speeds) {
        veh->GetMoveSpeed() = spd;
        float rev = CCarCtrl::FindMaxSteerAngle(veh.AsVehicle());
        float orig;
        { HookDisableGuard guard("Global/CCarCtrl/FindMaxSteerAngle");
          orig = CCarCtrl::FindMaxSteerAngle(veh.AsVehicle()); }
        EXPECT_NEAR(rev, orig, 1e-5f);
    }
    veh->GetMoveSpeed() = CVector(0, 0, 0);
}

GAME_DIFF_TEST(CCarCtrl2, ChooseCarModelToLoad) {
    // Test with a few car group IDs.
    // Must seed both exe and DLL CRTs — the original ChooseCarModelToLoad at
    // 0x421900 inlines rand() calls to the exe's CRT at 0x821B1E.
    for (int32 group = 0; group < 5; group++) {
        SeedBothRng(42 + group);
        int32 rev = CCarCtrl::ChooseCarModelToLoad(group);
        SeedBothRng(42 + group);
        int32 orig;
        { HookDisableGuard guard("Global/CCarCtrl/ChooseCarModelToLoad");
          orig = CCarCtrl::ChooseCarModelToLoad(group); }
        EXPECT_EQ(rev, orig);
    }
}

GAME_DIFF_TEST(CCarCtrl2, ChoosePoliceCarModel_Extended) {
    // Test all city IDs (0=countryside, 1=LS, 2=SF, 3=LV)
    for (uint32 city = 0; city <= 3; city++) {
        int32 rev = CCarCtrl::ChoosePoliceCarModel(city);
        int32 orig;
        { HookDisableGuard guard("Global/CCarCtrl/ChoosePoliceCarModel");
          orig = CCarCtrl::ChoosePoliceCarModel(city); }
        EXPECT_EQ(rev, orig);
    }
}

GAME_DIFF_TEST(CCarCtrl2, ChooseGangCarModel_AllGangs) {
    for (int gang = 0; gang < 10; gang++) {
        eModelID rev = CCarCtrl::ChooseGangCarModel((eGangID)gang);
        eModelID orig;
        { HookDisableGuard guard("Global/CCarCtrl/ChooseGangCarModel");
          orig = CCarCtrl::ChooseGangCarModel((eGangID)gang); }
        EXPECT_EQ((int)rev, (int)orig);
    }
}

// --- Behavior tests ---

GAME_TEST(CCarCtrl2, TrafficCounters_NonNegative) {
    EXPECT_TRUE(CCarCtrl::NumRandomCars >= 0);
    EXPECT_TRUE(CCarCtrl::NumLawEnforcerCars >= 0);
    EXPECT_TRUE(CCarCtrl::NumMissionCars >= 0);
    EXPECT_TRUE(CCarCtrl::NumParkedCars >= 0);
}

GAME_TEST(CCarCtrl2, CarDensityMultiplier_Positive) {
    EXPECT_TRUE(CCarCtrl::CarDensityMultiplier > 0.0f);
}

GAME_TEST(CCarCtrl2, FindMaxSteerAngle_ClampRange) {
    // Formula: clamp(0.9 - speed, 0.2, 0.7)
    // At speed=0 → 0.7, at speed=0.7 → 0.2, at speed>=0.9 → 0.2
    int32 vehModel = Scenario::FindLoadedVehicleModel();
    if (vehModel < 0) return;
    Scenario::TestVehicle veh(vehModel, CVector(2405.0f, -1665.0f, 13.5f));
    if (!veh) return;

    veh->GetMoveSpeed() = CVector(0, 0, 0);
    float atZero = CCarCtrl::FindMaxSteerAngle(veh.AsVehicle());
    EXPECT_NEAR(atZero, 0.7f, 0.01f);

    veh->GetMoveSpeed() = CVector(2.0f, 0, 0);
    float atFast = CCarCtrl::FindMaxSteerAngle(veh.AsVehicle());
    EXPECT_NEAR(atFast, 0.2f, 0.01f);

    veh->GetMoveSpeed() = CVector(0, 0, 0);
}

GAME_TEST(CCarCtrl2, PoliceCarModels_Valid) {
    // All police car models should be valid model IDs (>= 400)
    for (uint32 city = 0; city <= 3; city++) {
        int32 model = CCarCtrl::ChoosePoliceCarModel(city);
        EXPECT_TRUE(model >= 400);
    }
}
