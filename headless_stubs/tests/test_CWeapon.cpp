// test_CWeapon.cpp — Differential tests for CWeapon instance methods.
// Hook paths: Global/CWeapon/<fn>
// Uses the player ped's weapon slots — CJ has unarmed weapon at state 9.

#include "StdInc.h"
#include "TestFramework.h"
#include "Weapon.h"
#include "Entity/Ped/Ped.h"
#include "World.h"

// Player has 13 weapon slots. Test the active/equipped ones.
static CWeapon* GetPlayerWeapon(int slot) {
    auto* ped = FindPlayerPed(0);
    if (!ped) return nullptr;
    return &ped->m_aWeapons[slot];
}

GAME_DIFF_TEST(CWeapon, IsTypeMelee) {
    // Test all weapon slots the player has
    for (int slot = 0; slot < 13; slot++) {
        auto* wep = GetPlayerWeapon(slot);
        if (!wep || wep->m_Type == WEAPON_UNARMED) continue;
        bool orig, rev;
        { HookDisableGuard guard("Global/CWeapon/IsTypeMelee"); orig = wep->IsTypeMelee(); }
        rev = wep->IsTypeMelee();
        EXPECT_EQ(orig, rev);
    }
    // Always test the unarmed slot (slot 0)
    auto* unarmed = GetPlayerWeapon(0);
    if (unarmed) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CWeapon/IsTypeMelee"); orig = unarmed->IsTypeMelee(); }
        rev = unarmed->IsTypeMelee();
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(CWeapon, IsType2Handed) {
    for (int slot = 0; slot < 13; slot++) {
        auto* wep = GetPlayerWeapon(slot);
        if (!wep) continue;
        bool orig, rev;
        { HookDisableGuard guard("Global/CWeapon/IsType2Handed"); orig = wep->IsType2Handed(); }
        rev = wep->IsType2Handed();
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(CWeapon, IsTypeProjectile) {
    for (int slot = 0; slot < 13; slot++) {
        auto* wep = GetPlayerWeapon(slot);
        if (!wep) continue;
        bool orig, rev;
        { HookDisableGuard guard("Global/CWeapon/IsTypeProjectile"); orig = wep->IsTypeProjectile(); }
        rev = wep->IsTypeProjectile();
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(CWeapon, HasWeaponAmmoToBeUsed) {
    for (int slot = 0; slot < 13; slot++) {
        auto* wep = GetPlayerWeapon(slot);
        if (!wep) continue;
        bool orig, rev;
        { HookDisableGuard guard("Global/CWeapon/HasWeaponAmmoToBeUsed"); orig = wep->HasWeaponAmmoToBeUsed(); }
        rev = wep->HasWeaponAmmoToBeUsed();
        EXPECT_EQ(orig, rev);
    }
}
