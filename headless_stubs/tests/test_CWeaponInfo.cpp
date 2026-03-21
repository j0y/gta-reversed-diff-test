// test_CWeaponInfo.cpp — Differential tests for CWeaponInfo.
// Hook paths: Global/CWeaponInfo/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "WeaponInfo.h"

// GetSkillStatIndex crashes in original code for some weapon types — needs investigation.
// Likely original code has no bounds check and some eWeaponType values cause OOB access.

GAME_DIFF_TEST(CWeaponInfo, FindWeaponType) {
    const char* names[] = {
        "UNARMED", "PISTOL", "SHOTGUN", "AK47", "M4", "SNIPERRIFLE",
        "GRENADE", "KNIFE", "BAT", "INVALID_WEAPON_NAME"
    };
    for (auto name : names) {
        int orig, rev;
        { HookDisableGuard guard("Global/CWeaponInfo/FindWeaponType");
          orig = (int)CWeaponInfo::FindWeaponType(name); }
        rev = (int)CWeaponInfo::FindWeaponType(name);
        EXPECT_EQ(orig, rev);
    }
}

// GetWeaponInfo is overloaded — hook name differs. Skipped.
