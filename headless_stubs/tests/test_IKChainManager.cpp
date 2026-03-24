// test_IKChainManager.cpp — Differential tests for IKChainManager_c.
// Hook paths: Global/IKChainManager_c/<fn>
//
// IK chain manager queries on player ped.

#include "StdInc.h"
#include "TestFramework.h"
#include "IKChainManager_c.h"
#include "Entity/Ped/Ped.h"

// --- IsLooking ---

GAME_DIFF_TEST(IKChainManager, IsLooking_Player) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    bool orig, rev;
    { HookDisableGuard guard("Global/IKChainManager_c/IsLooking");
      orig = g_ikChainMan.IsLooking(player); }
    rev = g_ikChainMan.IsLooking(player);
    EXPECT_EQ(orig, rev);
}

// --- CanAcceptLookAt ---

GAME_DIFF_TEST(IKChainManager, CanAcceptLookAt_Player) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    bool orig, rev;
    { HookDisableGuard guard("Global/IKChainManager_c/CanAcceptLookAt");
      orig = g_ikChainMan.CanAcceptLookAt(player); }
    rev = g_ikChainMan.CanAcceptLookAt(player);
    EXPECT_EQ(orig, rev);
}

// --- GetLookAtEntity ---

GAME_DIFF_TEST(IKChainManager, GetLookAtEntity_Player) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    CEntity* orig;
    CEntity* rev;
    { HookDisableGuard guard("Global/IKChainManager_c/GetLookAtEntity");
      orig = g_ikChainMan.GetLookAtEntity(player); }
    rev = g_ikChainMan.GetLookAtEntity(player);
    EXPECT_EQ(orig, rev);
}
