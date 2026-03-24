// test_CPlayerPed2.cpp — Extended differential tests for CPlayerPed.
// Hook paths: Entity/Ped/CPlayerPed/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "Entity/Ped/PlayerPed.h"
#include "Entity/Ped/Ped.h"

GAME_DIFF_TEST(CPlayerPed2, CanPlayerStartMission) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    bool orig, rev;
    { HookDisableGuard guard("Entity/Ped/CPlayerPed/CanPlayerStartMission");
      orig = player->CanPlayerStartMission(); }
    rev = player->CanPlayerStartMission();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CPlayerPed2, IsHidden) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    bool orig, rev;
    { HookDisableGuard guard("Entity/Ped/CPlayerPed/IsHidden");
      orig = player->IsHidden(); }
    rev = player->IsHidden();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CPlayerPed2, GetWantedLevel) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    int32 orig, rev;
    { HookDisableGuard guard("Entity/Ped/CPlayerPed/GetWantedLevel");
      orig = player->GetWantedLevel(); }
    rev = player->GetWantedLevel();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CPlayerPed2, GetPadFromPlayer) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    CPad* orig;
    CPad* rev;
    { HookDisableGuard guard("Entity/Ped/CPlayerPed/GetPadFromPlayer");
      orig = player->GetPadFromPlayer(); }
    rev = player->GetPadFromPlayer();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CPlayerPed2, GetPlayerInfoForThisPlayerPed) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    CPlayerInfo* orig;
    CPlayerInfo* rev;
    { HookDisableGuard guard("Entity/Ped/CPlayerPed/GetPlayerInfoForThisPlayerPed");
      orig = player->GetPlayerInfoForThisPlayerPed(); }
    rev = player->GetPlayerInfoForThisPlayerPed();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CPlayerPed2, DoesPlayerWantNewWeapon) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    // Test with a few weapon types
    eWeaponType weapons[] = { WEAPON_PISTOL, WEAPON_SHOTGUN, WEAPON_AK47, WEAPON_UNARMED };
    for (auto wt : weapons) {
        bool orig, rev;
        { HookDisableGuard guard("Entity/Ped/CPlayerPed/DoesPlayerWantNewWeapon");
          orig = player->DoesPlayerWantNewWeapon(wt, false); }
        rev = player->DoesPlayerWantNewWeapon(wt, false);
        EXPECT_EQ(orig, rev);
    }
}

// CanIKReachThisTarget — original code crashes with null weapon param. Skipped.
