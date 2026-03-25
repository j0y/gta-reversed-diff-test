// test_CCarAI2.cpp — Extended differential tests for CCarAI.
// Hook paths: AI/CCarAI/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "CarAI.h"
#include "Entity/Ped/Ped.h"
#include "ScenarioHelpers.h"

// --- BackToCruisingIfNoWantedLevel ---

GAME_DIFF_TEST(CCarAI2, BackToCruisingIfNoWantedLevel) {
    auto* veh = FindPlayerVehicle(0);
    if (!veh) return; // skip if not in vehicle
    { HookDisableGuard guard("AI/CCarAI/BackToCruisingIfNoWantedLevel");
      CCarAI::BackToCruisingIfNoWantedLevel(veh); }
    CCarAI::BackToCruisingIfNoWantedLevel(veh);
    // Just verify no crash — function modifies vehicle AI state
    EXPECT_TRUE(true);
}

// --- EntitiesGoHeadOn ---

GAME_DIFF_TEST(CCarAI2, EntitiesGoHeadOn_PlayerSelf) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    bool orig, rev;
    { HookDisableGuard guard("AI/CCarAI/EntitiesGoHeadOn"); orig = CCarAI::EntitiesGoHeadOn(player, player); }
    rev = CCarAI::EntitiesGoHeadOn(player, player);
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CCarAI2, EntitiesGoHeadOn_PlayerAndVehicle) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    int32 model = Scenario::FindLoadedVehicleModel();
    if (model < 0) return;
    Scenario::TestVehicle veh(model, CVector(2500.f, -1666.f, 13.5f));
    if (!veh) return;
    bool orig, rev;
    { HookDisableGuard guard("AI/CCarAI/EntitiesGoHeadOn"); orig = CCarAI::EntitiesGoHeadOn(player, veh.AsVehicle()); }
    rev = CCarAI::EntitiesGoHeadOn(player, veh.AsVehicle());
    EXPECT_EQ(orig, rev);
}
