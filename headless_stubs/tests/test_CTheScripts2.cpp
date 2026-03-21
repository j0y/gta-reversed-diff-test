// test_CTheScripts2.cpp — Additional differential tests for CTheScripts.
// Hook paths: Scripts/CTheScripts/<fn>
// Extends test_CTheScripts.cpp with ped/vehicle stopped queries.
//
// IsPedStopped and IsVehicleStopped are used extensively by mission scripts
// to gate progression — bugs here break mission triggers.

#include "StdInc.h"
#include "TestFramework.h"
#include "ScenarioHelpers.h"
#include "Scripts/TheScripts.h"

// --- IsPedStopped ---

GAME_DIFF_TEST(CTheScripts2, IsPedStopped_Player) {
    auto* ped = FindPlayerPed(0);
    if (!ped) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No player ped"); return; }
    bool orig, rev;
    { HookDisableGuard guard("Scripts/CTheScripts/IsPedStopped"); orig = CTheScripts::IsPedStopped(ped); }
    rev = CTheScripts::IsPedStopped(ped);
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CTheScripts2, IsPedStopped_SpawnedPed) {
    int32 model = Scenario::FindLoadedPedModel();
    if (model < 0) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No loaded ped model"); return; }
    Scenario::TestPed testPed(PED_TYPE_CIVMALE, model, CVector(2496.0f, -1670.0f, 13.5f));
    if (!testPed) { GetTestContext().RecordFailure(__FILE__, __LINE__, "Failed to spawn ped"); return; }

    bool orig, rev;
    { HookDisableGuard guard("Scripts/CTheScripts/IsPedStopped"); orig = CTheScripts::IsPedStopped(testPed.AsPed()); }
    rev = CTheScripts::IsPedStopped(testPed.AsPed());
    EXPECT_EQ(orig, rev);
}

// --- IsVehicleStopped ---

GAME_DIFF_TEST(CTheScripts2, IsVehicleStopped_SpawnedVehicle) {
    int32 model = Scenario::FindLoadedVehicleModel();
    if (model < 0) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No loaded vehicle model"); return; }
    Scenario::TestVehicle veh(model, CVector(2506.0f, -1665.0f, 13.5f));
    if (!veh) { GetTestContext().RecordFailure(__FILE__, __LINE__, "Failed to spawn vehicle"); return; }

    bool orig, rev;
    { HookDisableGuard guard("Scripts/CTheScripts/IsVehicleStopped"); orig = CTheScripts::IsVehicleStopped(veh.AsVehicle()); }
    rev = CTheScripts::IsVehicleStopped(veh.AsVehicle());
    EXPECT_EQ(orig, rev);
}
