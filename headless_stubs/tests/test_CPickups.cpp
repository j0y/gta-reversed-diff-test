// test_CPickups.cpp — Differential tests for CPickups.
// Hook paths: Global/CPickups/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "Pickups.h"

GAME_DIFF_TEST(CPickups, ModelForWeapon) {
    // Test weapon types 0-45 (common weapon IDs)
    for (int w = 0; w < 46; w++) {
        int32 orig, rev;
        { HookDisableGuard guard("Global/CPickups/ModelForWeapon");
          orig = CPickups::ModelForWeapon(static_cast<eWeaponType>(w)); }
        rev = CPickups::ModelForWeapon(static_cast<eWeaponType>(w));
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(CPickups, GetActualPickupIndex) {
    // Use small valid pickup references — only low indices to avoid OOB
    tPickupReference refs[] = { tPickupReference(0), tPickupReference(1), tPickupReference(2) };
    for (auto ref : refs) {
        int32 orig, rev;
        { HookDisableGuard guard("Global/CPickups/GetActualPickupIndex");
          orig = CPickups::GetActualPickupIndex(ref); }
        rev = CPickups::GetActualPickupIndex(ref);
        EXPECT_EQ(orig, rev);
    }
}

// GetUniquePickupIndex crashes in original code with some indices — needs investigation
// Likely accesses uninitialized pickup array slots

GAME_DIFF_TEST(CPickups, IsPickUpPickedUp) {
    int refs[] = { 0, 1, 2, 5, 10, 100, 500 };
    for (int ref : refs) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CPickups/IsPickUpPickedUp");
          orig = CPickups::IsPickUpPickedUp(ref); }
        rev = CPickups::IsPickUpPickedUp(ref);
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(CPickups, PlayerCanPickUpThisWeaponTypeAtThisMoment) {
    // Common weapon types
    int weapons[] = { 0, 1, 2, 3, 5, 6, 8, 9, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34 };
    for (int w : weapons) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CPickups/PlayerCanPickUpThisWeaponTypeAtThisMoment");
          orig = CPickups::PlayerCanPickUpThisWeaponTypeAtThisMoment(static_cast<eWeaponType>(w)); }
        rev = CPickups::PlayerCanPickUpThisWeaponTypeAtThisMoment(static_cast<eWeaponType>(w));
        EXPECT_EQ(orig, rev);
    }
}
