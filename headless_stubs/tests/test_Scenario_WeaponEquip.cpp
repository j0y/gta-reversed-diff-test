// test_Scenario_WeaponEquip.cpp — Scenario: ped given weapons, queries tested.
//
// Spawns a civilian ped, gives it several weapons via GiveWeapon,
// switches active weapon, then differentially tests weapon slot queries,
// type checks, ammo queries, and ped weapon state.

#include "StdInc.h"
#include "TestFramework.h"
#include "ScenarioHelpers.h"
#include "Weapon.h"
#include "WeaponInfo.h"

static Scenario::TestPed* s_weapPed = nullptr;
static bool s_weapSetup = false;

static CPed* GetArmedPed() {
    if (!s_weapSetup) {
        s_weapSetup = true;
        int32 model = Scenario::FindLoadedPedModel();
        if (model < 0) return nullptr;
        s_weapPed = new Scenario::TestPed(PED_TYPE_CIVMALE, model,
            FindPlayerCoors() + CVector(50.0f, 0.0f, 0.0f));
        if (s_weapPed && *s_weapPed) {
            auto* ped = s_weapPed->AsPed();
            // Give several weapons across different slots
            ped->GiveWeapon(WEAPON_PISTOL, 100, true);
            ped->GiveWeapon(WEAPON_SHOTGUN, 50, true);
            ped->GiveWeapon(WEAPON_M4, 200, true);
            ped->GiveWeapon(WEAPON_KNIFE, 1, true);
            ped->GiveWeapon(WEAPON_GRENADE, 5, true);
        }
    }
    return (s_weapPed && *s_weapPed) ? s_weapPed->AsPed() : nullptr;
}

#define WEAP_GUARD() if (!GetArmedPed()) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No armed ped"); return; }

// --- Differential: GetWeaponSlot for each weapon type ---

GAME_DIFF_TEST(Scenario_WeaponEquip, GetWeaponSlot) {
    WEAP_GUARD()
    auto* ped = GetArmedPed();
    eWeaponType types[] = {
        WEAPON_UNARMED, WEAPON_PISTOL, WEAPON_SHOTGUN, WEAPON_M4,
        WEAPON_KNIFE, WEAPON_GRENADE, WEAPON_MINIGUN, WEAPON_CAMERA
    };
    for (auto wt : types) {
        int32 rev = ped->GetWeaponSlot(wt);
        int32 orig;
        { HookDisableGuard guard("Entity/Ped/CPed/GetWeaponSlot");
          orig = ped->GetWeaponSlot(wt); }
        EXPECT_EQ(rev, orig);
    }
}

// --- Differential: DoWeHaveWeaponAvailable ---

GAME_DIFF_TEST(Scenario_WeaponEquip, DoWeHaveWeaponAvailable) {
    WEAP_GUARD()
    auto* ped = GetArmedPed();
    eWeaponType types[] = {
        WEAPON_UNARMED, WEAPON_PISTOL, WEAPON_SHOTGUN, WEAPON_M4,
        WEAPON_KNIFE, WEAPON_GRENADE,
        WEAPON_MINIGUN, WEAPON_ROCKET, WEAPON_FLAMETHROWER // not given
    };
    for (auto wt : types) {
        bool rev = ped->DoWeHaveWeaponAvailable(wt);
        bool orig;
        { HookDisableGuard guard("Entity/Ped/CPed/DoWeHaveWeaponAvailable");
          orig = ped->DoWeHaveWeaponAvailable(wt); }
        EXPECT_EQ(rev, orig);
    }
}

// --- Differential: SetCurrentWeapon then check active weapon ---

GAME_DIFF_TEST(Scenario_WeaponEquip, SetCurrentWeapon_Slot) {
    WEAP_GUARD()
    auto* ped = GetArmedPed();
    auto savedSlot = ped->m_nActiveWeaponSlot;

    // Get pistol slot
    int32 pistolSlot = ped->GetWeaponSlot(WEAPON_PISTOL);
    if (pistolSlot < 0) return;

    // Reversed
    ped->SetCurrentWeapon(pistolSlot);
    auto revSlot = ped->m_nActiveWeaponSlot;
    auto revType = ped->GetActiveWeapon().m_Type;

    // Original
    ped->m_nActiveWeaponSlot = savedSlot;
    { HookDisableGuard guard("Entity/Ped/CPed/SetCurrentWeapon-Slot");
      ped->SetCurrentWeapon(pistolSlot); }
    EXPECT_EQ(revSlot, ped->m_nActiveWeaponSlot);
    EXPECT_EQ((int)revType, (int)ped->GetActiveWeapon().m_Type);

    ped->m_nActiveWeaponSlot = savedSlot;
}

GAME_DIFF_TEST(Scenario_WeaponEquip, SetCurrentWeapon_Type) {
    WEAP_GUARD()
    auto* ped = GetArmedPed();
    auto savedSlot = ped->m_nActiveWeaponSlot;

    // Reversed
    ped->SetCurrentWeapon(WEAPON_M4);
    auto revSlot = ped->m_nActiveWeaponSlot;
    auto revType = ped->GetActiveWeapon().m_Type;

    // Original
    ped->m_nActiveWeaponSlot = savedSlot;
    { HookDisableGuard guard("Entity/Ped/CPed/SetCurrentWeapon-WepType");
      ped->SetCurrentWeapon(WEAPON_M4); }
    EXPECT_EQ(revSlot, ped->m_nActiveWeaponSlot);
    EXPECT_EQ((int)revType, (int)ped->GetActiveWeapon().m_Type);

    ped->m_nActiveWeaponSlot = savedSlot;
}

// --- Differential: CWeapon type checks on armed ped ---

GAME_DIFF_TEST(Scenario_WeaponEquip, WeaponTypeChecks) {
    WEAP_GUARD()
    auto* ped = GetArmedPed();

    // Test each weapon slot the ped has
    for (int slot = 0; slot < 13; slot++) {
        auto& wep = ped->m_aWeapons[slot];
        if (wep.m_Type == WEAPON_UNARMED && slot != 0) continue;

        bool revMelee = wep.IsTypeMelee();
        bool origMelee;
        { HookDisableGuard guard("Global/CWeapon/IsTypeMelee");
          origMelee = wep.IsTypeMelee(); }
        EXPECT_EQ(revMelee, origMelee);

        bool rev2H = wep.IsType2Handed();
        bool orig2H;
        { HookDisableGuard guard("Global/CWeapon/IsType2Handed");
          orig2H = wep.IsType2Handed(); }
        EXPECT_EQ(rev2H, orig2H);

        bool revProj = wep.IsTypeProjectile();
        bool origProj;
        { HookDisableGuard guard("Global/CWeapon/IsTypeProjectile");
          origProj = wep.IsTypeProjectile(); }
        EXPECT_EQ(revProj, origProj);

        bool revAmmo = wep.HasWeaponAmmoToBeUsed();
        bool origAmmo;
        { HookDisableGuard guard("Global/CWeapon/HasWeaponAmmoToBeUsed");
          origAmmo = wep.HasWeaponAmmoToBeUsed(); }
        EXPECT_EQ(revAmmo, origAmmo);
    }
}

// --- Differential: ClearWeapon ---

GAME_DIFF_TEST(Scenario_WeaponEquip, ClearWeapon) {
    WEAP_GUARD()
    auto* ped = GetArmedPed();
    int32 shotgunSlot = ped->GetWeaponSlot(WEAPON_SHOTGUN);
    if (shotgunSlot < 0) return;

    // Save raw bytes (CWeapon has deleted copy ctor)
    char savedWep[sizeof(CWeapon)];
    memcpy(savedWep, &ped->m_aWeapons[shotgunSlot], sizeof(CWeapon));

    // Reversed
    ped->ClearWeapon(WEAPON_SHOTGUN);
    auto revType = ped->m_aWeapons[shotgunSlot].m_Type;
    auto revAmmo = ped->m_aWeapons[shotgunSlot].m_TotalAmmo;

    // Restore and do original
    memcpy(&ped->m_aWeapons[shotgunSlot], savedWep, sizeof(CWeapon));
    { HookDisableGuard guard("Entity/Ped/CPed/ClearWeapon");
      ped->ClearWeapon(WEAPON_SHOTGUN); }
    EXPECT_EQ((int)revType, (int)ped->m_aWeapons[shotgunSlot].m_Type);
    EXPECT_EQ(revAmmo, ped->m_aWeapons[shotgunSlot].m_TotalAmmo);

    // Restore for other tests
    memcpy(&ped->m_aWeapons[shotgunSlot], savedWep, sizeof(CWeapon));
}

// --- Behavior tests ---

GAME_TEST(Scenario_WeaponEquip, HasExpectedWeapons) {
    WEAP_GUARD()
    auto* ped = GetArmedPed();

    EXPECT_TRUE(ped->DoWeHaveWeaponAvailable(WEAPON_PISTOL));
    EXPECT_TRUE(ped->DoWeHaveWeaponAvailable(WEAPON_SHOTGUN));
    EXPECT_TRUE(ped->DoWeHaveWeaponAvailable(WEAPON_M4));
    EXPECT_TRUE(ped->DoWeHaveWeaponAvailable(WEAPON_KNIFE));
    EXPECT_TRUE(ped->DoWeHaveWeaponAvailable(WEAPON_GRENADE));
    EXPECT_FALSE(ped->DoWeHaveWeaponAvailable(WEAPON_MINIGUN));
}

GAME_TEST(Scenario_WeaponEquip, WeaponAmmo) {
    WEAP_GUARD()
    auto* ped = GetArmedPed();

    int32 pistolSlot = ped->GetWeaponSlot(WEAPON_PISTOL);
    EXPECT_TRUE(pistolSlot >= 0);
    if (pistolSlot >= 0) {
        EXPECT_TRUE(ped->m_aWeapons[pistolSlot].m_TotalAmmo > 0);
    }

    int32 m4Slot = ped->GetWeaponSlot(WEAPON_M4);
    EXPECT_TRUE(m4Slot >= 0);
    if (m4Slot >= 0) {
        EXPECT_TRUE(ped->m_aWeapons[m4Slot].m_TotalAmmo > 0);
    }
}

GAME_TEST(Scenario_WeaponEquip, KnifeIsMelee) {
    WEAP_GUARD()
    auto* ped = GetArmedPed();
    int32 knifeSlot = ped->GetWeaponSlot(WEAPON_KNIFE);
    if (knifeSlot >= 0) {
        EXPECT_TRUE(ped->m_aWeapons[knifeSlot].IsTypeMelee());
        EXPECT_FALSE(ped->m_aWeapons[knifeSlot].IsTypeProjectile());
    }
}

GAME_TEST(Scenario_WeaponEquip, GrenadeIsProjectile) {
    WEAP_GUARD()
    auto* ped = GetArmedPed();
    int32 grenadeSlot = ped->GetWeaponSlot(WEAPON_GRENADE);
    if (grenadeSlot >= 0) {
        EXPECT_TRUE(ped->m_aWeapons[grenadeSlot].IsTypeProjectile());
        EXPECT_FALSE(ped->m_aWeapons[grenadeSlot].IsTypeMelee());
    }
}
