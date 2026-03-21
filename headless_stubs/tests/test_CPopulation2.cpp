// test_CPopulation2.cpp — Extended differential tests for CPopulation.
// Hook paths: Global/CPopulation/<fn>
//
// Ped/vehicle density, multipliers, occupation selection, distance queries.

#include "StdInc.h"
#include "TestFramework.h"
#include "ScenarioHelpers.h"
#include "Population.h"

// --- Differential tests ---

GAME_DIFF_TEST(CPopulation2, FindPedMultiplierMotorway) {
    float rev = CPopulation::FindPedMultiplierMotorway();
    float orig;
    { HookDisableGuard guard("Global/CPopulation/FindPedMultiplierMotorway");
      orig = CPopulation::FindPedMultiplierMotorway(); }
    EXPECT_NEAR(rev, orig, 1e-5f);
}

GAME_DIFF_TEST(CPopulation2, FindCarMultiplierMotorway) {
    float rev = CPopulation::FindCarMultiplierMotorway();
    float orig;
    { HookDisableGuard guard("Global/CPopulation/FindCarMultiplierMotorway");
      orig = CPopulation::FindCarMultiplierMotorway(); }
    EXPECT_NEAR(rev, orig, 1e-5f);
}

GAME_DIFF_TEST(CPopulation2, FindNumberOfPedsWeCanPlaceOnBenches) {
    int32 rev = CPopulation::FindNumberOfPedsWeCanPlaceOnBenches();
    int32 orig;
    { HookDisableGuard guard("Global/CPopulation/FindNumberOfPedsWeCanPlaceOnBenches");
      orig = CPopulation::FindNumberOfPedsWeCanPlaceOnBenches(); }
    EXPECT_EQ(rev, orig);
}

GAME_DIFF_TEST(CPopulation2, FindDistanceToNearestPedOfType) {
    CVector playerPos = FindPlayerCoors();
    ePedType types[] = { PED_TYPE_CIVMALE, PED_TYPE_CIVFEMALE, PED_TYPE_COP, PED_TYPE_GANG1 };
    for (auto type : types) {
        float rev = CPopulation::FindDistanceToNearestPedOfType(type, playerPos);
        float orig;
        { HookDisableGuard guard("Global/CPopulation/FindDistanceToNearestPedOfType");
          orig = CPopulation::FindDistanceToNearestPedOfType(type, playerPos); }
        EXPECT_NEAR(rev, orig, 1e-3f);
    }
}

GAME_DIFF_TEST(CPopulation2, ChooseCivilianOccupationForVehicle) {
    int32 vehModel = Scenario::FindLoadedVehicleModel();
    if (vehModel < 0) return;
    Scenario::TestVehicle veh(vehModel, CVector(2400.0f, -1665.0f, 13.5f));
    if (!veh) return;

    // Test male and female
    for (bool mustBeMale : { true, false }) {
        srand(42);
        eModelID rev = CPopulation::ChooseCivilianOccupationForVehicle(mustBeMale, veh.AsVehicle());
        srand(42);
        eModelID orig;
        { HookDisableGuard guard("Global/CPopulation/ChooseCivilianOccupationForVehicle");
          orig = CPopulation::ChooseCivilianOccupationForVehicle(mustBeMale, veh.AsVehicle()); }
        EXPECT_EQ((int)rev, (int)orig);
    }
}

GAME_DIFF_TEST(CPopulation2, PickGangCar) {
    for (int gang = 0; gang < 10; gang++) {
        eModelID rev = CPopulation::PickGangCar((eGangID)gang);
        eModelID orig;
        { HookDisableGuard guard("Global/CPopulation/PickGangCar");
          orig = CPopulation::PickGangCar((eGangID)gang); }
        EXPECT_EQ((int)rev, (int)orig);
    }
}

GAME_DIFF_TEST(CPopulation2, IsMale_LoadedModels) {
    // Only test models that are loaded (avoid null CModelInfo crash)
    for (int m = 0; m < 300; m++) {
        if (!CStreaming::IsModelLoaded(m)) continue;
        bool rev = CPopulation::IsMale((eModelID)m);
        bool orig;
        { HookDisableGuard guard("Global/CPopulation/IsMale");
          orig = CPopulation::IsMale((eModelID)m); }
        EXPECT_EQ(rev, orig);
    }
}

GAME_DIFF_TEST(CPopulation2, IsFemale_LoadedModels) {
    for (int m = 0; m < 300; m++) {
        if (!CStreaming::IsModelLoaded(m)) continue;
        bool rev = CPopulation::IsFemale((eModelID)m);
        bool orig;
        { HookDisableGuard guard("Global/CPopulation/IsFemale");
          orig = CPopulation::IsFemale((eModelID)m); }
        EXPECT_EQ(rev, orig);
    }
}

GAME_DIFF_TEST(CPopulation2, DoesCarGroupHaveModelId_Extended) {
    // Test more groups and models
    for (int group = 0; group < 20; group++) {
        for (int model = 400; model <= 611; model += 20) {
            bool rev = CPopulation::DoesCarGroupHaveModelId(group, model);
            bool orig;
            { HookDisableGuard guard("Global/CPopulation/DoesCarGroupHaveModelId");
              orig = CPopulation::DoesCarGroupHaveModelId(group, model); }
            EXPECT_EQ(rev, orig);
        }
    }
}

// --- Behavior tests ---

GAME_TEST(CPopulation2, MultipliersPositive) {
    EXPECT_TRUE(CPopulation::FindPedMultiplierMotorway() >= 0.0f);
    EXPECT_TRUE(CPopulation::FindCarMultiplierMotorway() >= 0.0f);
    EXPECT_TRUE(CPopulation::PedCreationDistMultiplier() > 0.0f);
}

GAME_TEST(CPopulation2, GenderConsistency) {
    // A loaded model can't be both male and female
    for (int m = 0; m < 300; m++) {
        if (!CStreaming::IsModelLoaded(m)) continue;
        bool male = CPopulation::IsMale((eModelID)m);
        bool female = CPopulation::IsFemale((eModelID)m);
        EXPECT_FALSE(male && female);
    }
}
