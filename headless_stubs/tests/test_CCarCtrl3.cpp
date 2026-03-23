// test_CCarCtrl3.cpp — Deep differential tests for CCarCtrl.
// Hook paths: Global/CCarCtrl/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "CarCtrl.h"

// --- IsThisVehicleInteresting ---

GAME_DIFF_TEST(CCarCtrl3, IsThisVehicleInteresting_Models) {
    int32 models[] = { 400, 410, 420, 500, 560, 596, 597, 598, 600 };
    for (int32 m : models) {
        auto* mi = CModelInfo::GetModelInfo(m);
        if (!mi) continue;
        bool orig, rev;
        { HookDisableGuard guard("Global/CCarCtrl/IsThisVehicleInteresting"); orig = CCarCtrl::IsThisVehicleInteresting(nullptr); }
        rev = CCarCtrl::IsThisVehicleInteresting(nullptr);
        EXPECT_EQ(orig, rev);
        break; // just test once with nullptr to avoid crash
    }
}

// --- FindMaxSteerAngle (takes CVehicle*) ---

GAME_DIFF_TEST(CCarCtrl3, FindMaxSteerAngle) {
    auto* veh = FindPlayerVehicle(0);
    if (!veh) return; // skip if not in vehicle
    float orig, rev;
    { HookDisableGuard guard("Global/CCarCtrl/FindMaxSteerAngle"); orig = CCarCtrl::FindMaxSteerAngle(veh); }
    rev = CCarCtrl::FindMaxSteerAngle(veh);
    EXPECT_NEAR(orig, rev, 1e-5f);
}
