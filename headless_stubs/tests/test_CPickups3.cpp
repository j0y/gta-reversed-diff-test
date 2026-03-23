// test_CPickups3.cpp — Extended differential tests for CPickups.
// Hook paths: Global/CPickups/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "Pickups.h"
#include "Entity/Ped/Ped.h"

// --- GivePlayerGoodiesWithPickUpMI ---
// Test with various model IDs — checks what goodies each model gives

GAME_DIFF_TEST(CPickups3, GivePlayerGoodiesWithPickUpMI_Various) {
    int32 models[] = { 0, 1, 100, 330, 331, 333, 334, 350, 370, 400, 1240, 1242, 1247, 1550 };
    for (int32 m : models) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CPickups/GivePlayerGoodiesWithPickUpMI");
          orig = CPickups::GivePlayerGoodiesWithPickUpMI(m, 0); }
        rev = CPickups::GivePlayerGoodiesWithPickUpMI(m, 0);
        EXPECT_EQ(orig, rev);
    }
}

// --- ModelForWeapon extended ---

GAME_DIFF_TEST(CPickups3, ModelForWeapon_FullRange) {
    // Test all weapon types 0-46
    for (int32 wt = 0; wt <= 46; wt++) {
        int32 orig, rev;
        { HookDisableGuard guard("Global/CPickups/ModelForWeapon");
          orig = CPickups::ModelForWeapon(static_cast<eWeaponType>(wt)); }
        rev = CPickups::ModelForWeapon(static_cast<eWeaponType>(wt));
        EXPECT_EQ(orig, rev);
    }
}

// --- WeaponForModel extended ---

// WeaponForModel crashes for some model IDs in original code (OOB) — skip broad range test
