// test_CGarages4.cpp — Extended differential tests for CGarages.
// Hook paths: Global/CGarages/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "Garages.h"
#include "ScenarioHelpers.h"

// --- IsPointWithinHideOutGarage extended ---

GAME_DIFF_TEST(CGarages4, IsPointWithinHideOutGarage_AllCities) {
    CVector positions[] = {
        CVector(2495.0f, -1665.0f, 13.5f),  // GroveStreet
        CVector(1481.0f, -1764.0f, 18.8f),  // Downtown LS
        CVector(-2170.0f, 250.0f, 35.0f),   // San Fierro
        CVector(2027.0f, 1008.0f, 20.0f),   // Las Venturas
        CVector(0.0f, 0.0f, 0.0f),          // world origin
        CVector(2500.0f, -1660.0f, 13.5f),  // near CJ house
    };
    for (auto& pos : positions) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CGarages/IsPointWithinHideOutGarage"); orig = CGarages::IsPointWithinHideOutGarage(pos); }
        rev = CGarages::IsPointWithinHideOutGarage(pos);
        EXPECT_EQ(orig, rev);
    }
}

// --- IsModelIndexADoor extended ---

GAME_DIFF_TEST(CGarages4, IsModelIndexADoor_Extended) {
    // Test full range of common model IDs that might be garage doors
    for (int32 m = 1400; m <= 1500; m++) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CGarages/IsModelIndexADoor"); orig = CGarages::IsModelIndexADoor(m); }
        rev = CGarages::IsModelIndexADoor(m);
        EXPECT_EQ(orig, rev);
    }
}
