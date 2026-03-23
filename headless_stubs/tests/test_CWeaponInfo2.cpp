// test_CWeaponInfo2.cpp — Deep differential tests for CWeaponInfo.
// Hook paths: Global/CWeaponInfo/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "WeaponInfo.h"

// --- FindWeaponFireType (takes string name) ---

GAME_DIFF_TEST(CWeaponInfo, FindWeaponFireType_Names) {
    const char* names[] = {
        "MELEE", "INSTANT_HIT", "PROJECTILE", "AREA_EFFECT",
        "CAMERA", "USE", "INVALID_NAME_XYZ"
    };
    for (auto* name : names) {
        int32 orig, rev;
        { HookDisableGuard guard("Global/CWeaponInfo/FindWeaponFireType");
          orig = (int32)CWeaponInfo::FindWeaponFireType(name); }
        rev = (int32)CWeaponInfo::FindWeaponFireType(name);
        EXPECT_EQ(orig, rev);
    }
}

// --- FindWeaponType (existing hook, test more weapon name strings) ---

GAME_DIFF_TEST(CWeaponInfo, FindWeaponType_ExtendedNames) {
    const char* names[] = {
        "UNARMED", "BRASSKNUCKLE", "GOLFCLUB", "NIGHTSTICK", "KNIFE",
        "BASEBALLBAT", "SHOVEL", "POOL_CUE", "KATANA", "CHAINSAW",
        "PISTOL", "PISTOL_SILENCED", "DESERT_EAGLE",
        "SHOTGUN", "SAWNOFF_SHOTGUN", "SPAS12_SHOTGUN",
        "MICRO_UZI", "MP5", "AK47", "M4", "TEC9",
        "COUNTRYRIFLE", "SNIPERRIFLE",
        "RLAUNCHER", "RLAUNCHER_HS", "FTHROWER", "MINIGUN",
        "GRENADE", "TEARGAS", "MOLOTOV",
        "SPRAYCAN", "EXTINGUISHER", "CAMERA",
        "NIGHTVISION", "INFRARED", "PARACHUTE",
        "TOTALLY_INVALID_NAME"
    };
    for (auto* name : names) {
        int32 orig, rev;
        { HookDisableGuard guard("Global/CWeaponInfo/FindWeaponType");
          orig = (int32)CWeaponInfo::FindWeaponType(name); }
        rev = (int32)CWeaponInfo::FindWeaponType(name);
        EXPECT_EQ(orig, rev);
    }
}
