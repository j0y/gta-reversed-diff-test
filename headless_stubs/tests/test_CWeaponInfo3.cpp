// test_CWeaponInfo3.cpp — Differential tests for CWeaponInfo (uncovered methods).
// Hook paths: Global/CWeaponInfo/<fn>
// Covers: GetWeaponInfo (static lookup), GetSkillStatIndex

#include "StdInc.h"
#include "TestFramework.h"
#include "WeaponInfo.h"

// --- GetWeaponInfo (static, eWeaponType + eWeaponSkill) ---
// Hook name: "Global/CWeaponInfo/GetWeaponInfo-original"

GAME_DIFF_TEST(CWeaponInfo3, GetWeaponInfo_STD) {
    // Test all standard weapon types with STD skill
    eWeaponType weapons[] = {
        WEAPON_UNARMED, WEAPON_BRASSKNUCKLE, WEAPON_GOLFCLUB, WEAPON_NIGHTSTICK,
        WEAPON_KNIFE, WEAPON_BASEBALLBAT, WEAPON_SHOVEL, WEAPON_POOL_CUE,
        WEAPON_KATANA, WEAPON_CHAINSAW,
        WEAPON_PISTOL, WEAPON_PISTOL_SILENCED, WEAPON_DESERT_EAGLE,
        WEAPON_SHOTGUN, WEAPON_SAWNOFF_SHOTGUN, WEAPON_SPAS12_SHOTGUN,
        WEAPON_MICRO_UZI, WEAPON_MP5, WEAPON_AK47, WEAPON_M4, WEAPON_TEC9,
        WEAPON_COUNTRYRIFLE, WEAPON_SNIPERRIFLE,
        WEAPON_RLAUNCHER, WEAPON_RLAUNCHER_HS, WEAPON_FLAMETHROWER, WEAPON_MINIGUN,
        WEAPON_GRENADE, WEAPON_TEARGAS, WEAPON_MOLOTOV,
        WEAPON_SPRAYCAN, WEAPON_EXTINGUISHER, WEAPON_CAMERA,
    };
    for (auto wt : weapons) {
        CWeaponInfo* orig;
        CWeaponInfo* rev;
        { HookDisableGuard guard("Global/CWeaponInfo/GetWeaponInfo-original");
          orig = CWeaponInfo::GetWeaponInfo(wt, eWeaponSkill::STD); }
        rev = CWeaponInfo::GetWeaponInfo(wt, eWeaponSkill::STD);
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(CWeaponInfo3, GetWeaponInfo_Skills) {
    // Test weapons that support skill levels (PISTOL through TEC9)
    eWeaponType skillWeapons[] = {
        WEAPON_PISTOL, WEAPON_PISTOL_SILENCED, WEAPON_DESERT_EAGLE,
        WEAPON_SHOTGUN, WEAPON_SAWNOFF_SHOTGUN, WEAPON_SPAS12_SHOTGUN,
        WEAPON_MICRO_UZI, WEAPON_MP5, WEAPON_AK47, WEAPON_M4, WEAPON_TEC9,
    };
    eWeaponSkill skills[] = {
        eWeaponSkill::STD, eWeaponSkill::POOR, eWeaponSkill::PRO, eWeaponSkill::COP,
    };
    for (auto wt : skillWeapons) {
        for (auto sk : skills) {
            CWeaponInfo* orig;
            CWeaponInfo* rev;
            { HookDisableGuard guard("Global/CWeaponInfo/GetWeaponInfo-original");
              orig = CWeaponInfo::GetWeaponInfo(wt, sk); }
            rev = CWeaponInfo::GetWeaponInfo(wt, sk);
            EXPECT_EQ(orig, rev);
        }
    }
}

// --- GetSkillStatIndex ---

GAME_DIFF_TEST(CWeaponInfo3, GetSkillStatIndex) {
    // Weapons that have skill stats (PISTOL through TEC9)
    eWeaponType skillWeapons[] = {
        WEAPON_PISTOL, WEAPON_PISTOL_SILENCED, WEAPON_DESERT_EAGLE,
        WEAPON_SHOTGUN, WEAPON_SAWNOFF_SHOTGUN, WEAPON_SPAS12_SHOTGUN,
        WEAPON_MICRO_UZI, WEAPON_MP5, WEAPON_AK47, WEAPON_M4, WEAPON_TEC9,
    };
    for (auto wt : skillWeapons) {
        int32 orig, rev;
        { HookDisableGuard guard("Global/CWeaponInfo/GetSkillStatIndex");
          orig = (int32)CWeaponInfo::GetSkillStatIndex(wt); }
        rev = (int32)CWeaponInfo::GetSkillStatIndex(wt);
        EXPECT_EQ(orig, rev);
    }
}
