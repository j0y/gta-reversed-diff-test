// test_CGroupEventHandler.cpp — Differential tests for CGroupEventHandler.
// Hook paths: Events/CGroupEventHandler/<fn>
// Tests group AI response logic.

#include "StdInc.h"
#include "TestFramework.h"
#include "Events/GroupEventHandler.h"
#include "Entity/Ped/Ped.h"
#include "PedGroup.h"
#include "ScenarioHelpers.h"

// --- IsKillTaskAppropriate ---

GAME_DIFF_TEST(CGroupEventHandler, IsKillTaskAppropriate) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    // Test with player's group (usually group 0)
    auto& group = CPedGroups::GetGroup(0);
    bool orig, rev;
    { HookDisableGuard guard("Events/CGroupEventHandler/IsKillTaskAppropriate");
      orig = CGroupEventHandler::IsKillTaskAppropriate(&group, player); }
    rev = CGroupEventHandler::IsKillTaskAppropriate(&group, player);
    EXPECT_EQ(orig, rev);
}
