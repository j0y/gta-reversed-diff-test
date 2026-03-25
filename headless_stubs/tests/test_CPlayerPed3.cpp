// test_CPlayerPed3.cpp — Extended CPlayerPed differential tests.
// Hook paths: Entity/Ped/CPlayerPed/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "Entity/Ped/PlayerPed.h"
#include "Entity/Ped/Ped.h"
#include "Pad.h"

GAME_DIFF_TEST(CPlayerPed3, DoesPlayerWantNewWeapon) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    bool orig, rev;
    { HookDisableGuard guard("Entity/Ped/CPlayerPed/DoesPlayerWantNewWeapon"); orig = player->DoesPlayerWantNewWeapon(WEAPON_PISTOL, true); }
    rev = player->DoesPlayerWantNewWeapon(WEAPON_PISTOL, true);
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CPlayerPed3, DoesPlayerWantNewWeapon_Shotgun) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    bool orig, rev;
    { HookDisableGuard guard("Entity/Ped/CPlayerPed/DoesPlayerWantNewWeapon"); orig = player->DoesPlayerWantNewWeapon(WEAPON_SHOTGUN, true); }
    rev = player->DoesPlayerWantNewWeapon(WEAPON_SHOTGUN, true);
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CPlayerPed3, GetPadFromPlayer) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    CPad* orig;
    CPad* rev;
    { HookDisableGuard guard("Entity/Ped/CPlayerPed/GetPadFromPlayer"); orig = player->GetPadFromPlayer(); }
    rev = player->GetPadFromPlayer();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CPlayerPed3, GetPlayerInfoForThisPlayerPed) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    CPlayerInfo* orig;
    CPlayerInfo* rev;
    { HookDisableGuard guard("Entity/Ped/CPlayerPed/GetPlayerInfoForThisPlayerPed"); orig = player->GetPlayerInfoForThisPlayerPed(); }
    rev = player->GetPlayerInfoForThisPlayerPed();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CPlayerPed3, PedCanBeTargettedVehicleWise) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    bool orig, rev;
    { HookDisableGuard guard("Entity/Ped/CPlayerPed/PedCanBeTargettedVehicleWise"); orig = player->PedCanBeTargettedVehicleWise(player); }
    rev = player->PedCanBeTargettedVehicleWise(player);
    EXPECT_EQ(orig, rev);
}

// DoesTargetHaveToBeBroken(CEntity*, CWeapon*) — needs a valid weapon pointer.
// Use the player's active weapon.
GAME_DIFF_TEST(CPlayerPed3, DoesTargetHaveToBeBroken) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    CWeapon& weapon = player->GetActiveWeapon();
    bool orig, rev;
    { HookDisableGuard guard("Entity/Ped/CPlayerPed/DoesTargetHaveToBeBroken");
      orig = player->DoesTargetHaveToBeBroken((CEntity*)player, &weapon); }
    rev = player->DoesTargetHaveToBeBroken((CEntity*)player, &weapon);
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CPlayerPed3, PlayerHasJustAttackedSomeone) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    bool orig, rev;
    { HookDisableGuard guard("Entity/Ped/CPlayerPed/PlayerHasJustAttackedSomeone"); orig = player->PlayerHasJustAttackedSomeone(); }
    rev = player->PlayerHasJustAttackedSomeone();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CPlayerPed3, GetWeaponRadiusOnScreen) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    float orig, rev;
    { HookDisableGuard guard("Entity/Ped/CPlayerPed/GetWeaponRadiusOnScreen"); orig = player->GetWeaponRadiusOnScreen(); }
    rev = player->GetWeaponRadiusOnScreen();
    EXPECT_NEAR(orig, rev, 1e-5f);
}

GAME_DIFF_TEST(CPlayerPed3, FindTargetPriority) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    // FindTargetPriority(CEntity*) — use player as entity
    float orig, rev;
    { HookDisableGuard guard("Entity/Ped/CPlayerPed/FindTargetPriority"); orig = player->FindTargetPriority((CEntity*)player); }
    rev = player->FindTargetPriority((CEntity*)player);
    EXPECT_NEAR(orig, rev, 1e-5f);
}
