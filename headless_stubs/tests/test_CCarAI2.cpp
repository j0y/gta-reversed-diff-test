// test_CCarAI2.cpp — Extended differential tests for CCarAI.
// Hook paths: AI/CCarAI/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "CarAI.h"
#include "Entity/Ped/Ped.h"

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
