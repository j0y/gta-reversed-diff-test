// test_CTheScripts4.cpp — Extended differential tests for CTheScripts.
// Hook paths: Scripts/CTheScripts/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "Scripts/TheScripts.h"
#include "Entity/Ped/Ped.h"
#include "Entity/Vehicle/Vehicle.h"
#include "ScenarioHelpers.h"

GAME_DIFF_TEST(CTheScripts4, IsPlayerOnAMission) {
    bool orig, rev;
    { HookDisableGuard guard("Scripts/CTheScripts/IsPlayerOnAMission");
      orig = CTheScripts::IsPlayerOnAMission(); }
    rev = CTheScripts::IsPlayerOnAMission();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CTheScripts4, IsPedStopped_Player) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    bool orig, rev;
    { HookDisableGuard guard("Scripts/CTheScripts/IsPedStopped");
      orig = CTheScripts::IsPedStopped(player); }
    rev = CTheScripts::IsPedStopped(player);
    EXPECT_EQ(orig, rev);
}

static Scenario::TestVehicle* s_ts4Veh = nullptr;
static bool s_triedTs4 = false;

static CVehicle* GetTs4Veh() {
    if (!s_triedTs4) {
        s_triedTs4 = true;
        int32 model = Scenario::FindLoadedVehicleModel();
        if (model >= 0)
            s_ts4Veh = new Scenario::TestVehicle(model, CVector(2380.f, -1650.f, 13.5f));
    }
    return s_ts4Veh ? s_ts4Veh->AsVehicle() : nullptr;
}

GAME_DIFF_TEST(CTheScripts4, IsVehicleStopped_SpawnedVeh) {
    auto* veh = GetTs4Veh();
    if (!veh) return;
    bool orig, rev;
    { HookDisableGuard guard("Scripts/CTheScripts/IsVehicleStopped");
      orig = CTheScripts::IsVehicleStopped(veh); }
    rev = CTheScripts::IsVehicleStopped(veh);
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CTheScripts4, HasCarModelBeenSuppressed_Range) {
    eModelID models[] = { MODEL_LANDSTAL, MODEL_BRAVURA, MODEL_BUFFALO, MODEL_LINERUN, MODEL_COPCARLA };
    for (auto m : models) {
        bool orig, rev;
        { HookDisableGuard guard("Scripts/CTheScripts/HasCarModelBeenSuppressed");
          orig = CTheScripts::HasCarModelBeenSuppressed(m); }
        rev = CTheScripts::HasCarModelBeenSuppressed(m);
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(CTheScripts4, HasVehicleModelBeenBlockedByScript_Range) {
    eModelID models[] = { MODEL_LANDSTAL, MODEL_BRAVURA, MODEL_BUFFALO, MODEL_COPCARLA };
    for (auto m : models) {
        bool orig, rev;
        { HookDisableGuard guard("Scripts/CTheScripts/HasVehicleModelBeenBlockedByScript");
          orig = CTheScripts::HasVehicleModelBeenBlockedByScript(m); }
        rev = CTheScripts::HasVehicleModelBeenBlockedByScript(m);
        EXPECT_EQ(orig, rev);
    }
}
