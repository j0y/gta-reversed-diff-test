// test_CPlayerPed.cpp — Differential tests for CPlayerPed member functions.
// Hook paths: Entity/Ped/CPlayerPed/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "Entity/Ped/PlayerPed.h"
#include "World.h"

static CPlayerPed* GetPlayerPed_() {
    return FindPlayerPed(0);
}

GAME_DIFF_TEST(CPlayerPed, GetWantedLevel) {
    auto* ped = GetPlayerPed_();
    if (!ped) return;
    int32 orig, rev;
    { HookDisableGuard guard("Entity/Ped/CPlayerPed/GetWantedLevel"); orig = ped->GetWantedLevel(); }
    rev = ped->GetWantedLevel();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CPlayerPed, IsHidden) {
    auto* ped = GetPlayerPed_();
    if (!ped) return;
    bool orig, rev;
    { HookDisableGuard guard("Entity/Ped/CPlayerPed/IsHidden"); orig = ped->IsHidden(); }
    rev = ped->IsHidden();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CPlayerPed, CanPlayerStartMission) {
    auto* ped = GetPlayerPed_();
    if (!ped) return;
    bool orig, rev;
    { HookDisableGuard guard("Entity/Ped/CPlayerPed/CanPlayerStartMission"); orig = ped->CanPlayerStartMission(); }
    rev = ped->CanPlayerStartMission();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CPlayerPed, DoesPlayerWantNewWeapon) {
    auto* ped = GetPlayerPed_();
    if (!ped) return;
    // Test with a few weapon types and whether player wants them
    eWeaponType weapons[] = { WEAPON_PISTOL, WEAPON_SHOTGUN, WEAPON_AK47 };
    for (auto wt : weapons) {
        bool orig, rev;
        { HookDisableGuard guard("Entity/Ped/CPlayerPed/DoesPlayerWantNewWeapon");
          orig = ped->DoesPlayerWantNewWeapon(wt, false); }
        rev = ped->DoesPlayerWantNewWeapon(wt, false);
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(CPlayerPed, GetWeaponRadiusOnScreen) {
    auto* ped = GetPlayerPed_();
    if (!ped) return;
    float orig, rev;
    { HookDisableGuard guard("Entity/Ped/CPlayerPed/GetWeaponRadiusOnScreen"); orig = ped->GetWeaponRadiusOnScreen(); }
    rev = ped->GetWeaponRadiusOnScreen();
    EXPECT_NEAR(orig, rev, 1e-5f);
}

GAME_DIFF_TEST(CPlayerPed, GetPlayerInfoForThisPlayerPed) {
    auto* ped = GetPlayerPed_();
    if (!ped) return;
    CPlayerInfo* orig;
    CPlayerInfo* rev;
    { HookDisableGuard guard("Entity/Ped/CPlayerPed/GetPlayerInfoForThisPlayerPed");
      orig = ped->GetPlayerInfoForThisPlayerPed(); }
    rev = ped->GetPlayerInfoForThisPlayerPed();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CPlayerPed, PedCanBeTargettedVehicleWise) {
    auto* ped = FindPlayerPed(0);
    if (!ped) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No player ped"); return; }
    bool orig, rev;
    { HookDisableGuard guard("Entity/Ped/CPlayerPed/PedCanBeTargettedVehicleWise");
      orig = CPlayerPed::PedCanBeTargettedVehicleWise(ped); }
    rev = CPlayerPed::PedCanBeTargettedVehicleWise(ped);
    EXPECT_EQ(orig, rev);
}
