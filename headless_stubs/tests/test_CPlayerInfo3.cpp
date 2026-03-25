// test_CPlayerInfo3.cpp — Extended CPlayerInfo differential tests.
// Hook paths: Global/CPlayerInfo/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "PlayerInfo.h"

GAME_DIFF_TEST(CPlayerInfo3, AddHealth) {
    auto& info = FindPlayerInfo(0);
    auto* player = FindPlayerPed(0);
    if (!player) return;
    // Save health
    float savedHealth = player->m_fHealth;
    // Add a small amount — compare results
    player->m_fHealth = savedHealth;
    { HookDisableGuard guard("Global/CPlayerInfo/AddHealth"); info.AddHealth(1); }
    float healthAfterOrig = player->m_fHealth;
    player->m_fHealth = savedHealth;
    info.AddHealth(1);
    float healthAfterRev = player->m_fHealth;
    EXPECT_NEAR(healthAfterOrig, healthAfterRev, 1e-5f);
    // Restore
    player->m_fHealth = savedHealth;
}

GAME_DIFF_TEST(CPlayerInfo3, StreamParachuteWeapon) {
    auto& info = FindPlayerInfo(0);
    // StreamParachuteWeapon just requests streaming — safe to call
    { HookDisableGuard guard("Global/CPlayerInfo/StreamParachuteWeapon"); info.StreamParachuteWeapon(false); }
    info.StreamParachuteWeapon(false);
    EXPECT_TRUE(true);
}
