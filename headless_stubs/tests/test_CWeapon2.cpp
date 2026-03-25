// test_CWeapon2.cpp — Differential tests for CWeapon (uncovered methods).
// Hook paths: Global/CWeapon/<fn>
// Covers: CanBeUsedFor2Player (static + method), TargetWeaponRangeMultiplier

#include "StdInc.h"
#include "TestFramework.h"
#include "Weapon.h"
#include "WeaponInfo.h"
#include "Entity/Ped/Ped.h"
#include "World.h"

static CWeapon* GetPlayerWeapon2(int slot) {
    auto* ped = FindPlayerPed(0);
    if (!ped) return nullptr;
    return &ped->m_aWeapons[slot];
}

// --- CanBeUsedFor2Player (static version, takes eWeaponType) ---

GAME_DIFF_TEST(CWeapon2, CanBeUsedFor2Player_Static) {
    eWeaponType weapons[] = {
        WEAPON_UNARMED, WEAPON_PISTOL, WEAPON_PISTOL_SILENCED, WEAPON_DESERT_EAGLE,
        WEAPON_SHOTGUN, WEAPON_SAWNOFF_SHOTGUN, WEAPON_SPAS12_SHOTGUN,
        WEAPON_MICRO_UZI, WEAPON_MP5, WEAPON_AK47, WEAPON_M4, WEAPON_TEC9,
        WEAPON_COUNTRYRIFLE, WEAPON_SNIPERRIFLE,
        WEAPON_RLAUNCHER, WEAPON_RLAUNCHER_HS, WEAPON_FLAMETHROWER, WEAPON_MINIGUN,
        WEAPON_GRENADE, WEAPON_TEARGAS, WEAPON_MOLOTOV,
        WEAPON_KNIFE, WEAPON_BASEBALLBAT, WEAPON_CHAINSAW,
        WEAPON_SPRAYCAN, WEAPON_EXTINGUISHER, WEAPON_CAMERA,
        WEAPON_PARACHUTE,
    };
    for (auto wt : weapons) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CWeapon/CanBeUsedFor2Player-Static");
          orig = CWeapon::CanBeUsedFor2Player(wt); }
        rev = CWeapon::CanBeUsedFor2Player(wt);
        EXPECT_EQ(orig, rev);
    }
}

// --- CanBeUsedFor2Player (instance method) ---

GAME_DIFF_TEST(CWeapon2, CanBeUsedFor2Player_Method) {
    for (int slot = 0; slot < 13; slot++) {
        auto* wep = GetPlayerWeapon2(slot);
        if (!wep) continue;
        bool orig, rev;
        { HookDisableGuard guard("Global/CWeapon/CanBeUsedFor2Player-Method");
          orig = wep->CanBeUsedFor2Player(); }
        rev = wep->CanBeUsedFor2Player();
        EXPECT_EQ(orig, rev);
    }
}

// --- TargetWeaponRangeMultiplier (static, takes target + weaponOwner) ---

GAME_DIFF_TEST(CWeapon2, TargetWeaponRangeMultiplier_Player) {
    // Use player ped as both target and owner — safe entities that exist in state 9
    auto* player = FindPlayerPed(0);
    if (!player) return;
    float orig, rev;
    { HookDisableGuard guard("Global/CWeapon/TargetWeaponRangeMultiplier");
      orig = CWeapon::TargetWeaponRangeMultiplier(player, player); }
    rev = CWeapon::TargetWeaponRangeMultiplier(player, player);
    EXPECT_NEAR(orig, rev, 1e-5f);
}

GAME_DIFF_TEST(CWeapon2, TargetWeaponRangeMultiplier_Nullptr) {
    // Test with null target — should handle gracefully
    auto* player = FindPlayerPed(0);
    if (!player) return;
    float orig, rev;
    { HookDisableGuard guard("Global/CWeapon/TargetWeaponRangeMultiplier");
      orig = CWeapon::TargetWeaponRangeMultiplier(nullptr, player); }
    rev = CWeapon::TargetWeaponRangeMultiplier(nullptr, player);
    EXPECT_NEAR(orig, rev, 1e-5f);
}
