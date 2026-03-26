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

// --- CanAccept ---

GAME_DIFF_TEST(IKChainManager, CanAccept_Player) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    float priorities[] = { 0.0f, 0.5f, 1.0f, 2.0f };
    for (float p : priorities) {
        bool orig, rev;
        { HookDisableGuard guard("Global/IKChainManager_c/CanAccept");
          orig = g_ikChainMan.CanAccept(player, p); }
        rev = g_ikChainMan.CanAccept(player, p);
        EXPECT_EQ(orig, rev);
    }
}

// --- IsArmPointing ---

GAME_DIFF_TEST(IKChainManager, IsArmPointing_Player) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    for (int arm = 0; arm < 2; arm++) {
        bool orig, rev;
        { HookDisableGuard guard("Global/IKChainManager_c/IsArmPointing");
          orig = IKChainManager_c::IsArmPointing((eIKArm)arm, player); }
        rev = IKChainManager_c::IsArmPointing((eIKArm)arm, player);
        EXPECT_EQ(orig, rev);
    }
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
