// test_CPickup2.cpp — Extended CPickup differential tests.
// Hook paths: Global/CPickup/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "Pickup.h"
#include "Pickups.h"

// FindTextIndexForString — converts text label to pickup text index
GAME_DIFF_TEST(CPickup2, FindTextIndexForString_KnownLabels) {
    const char* labels[] = { "COLT45", "SHOTGN", "AK47", "M4", "SNIPER", "ROCKET", "FLTHR" };
    for (const char* label : labels) {
        ePickupPropertyText orig, rev;
        { HookDisableGuard guard("Global/CPickup/FindTextIndexForString");
          orig = CPickup::FindTextIndexForString(label); }
        rev = CPickup::FindTextIndexForString(label);
        EXPECT_EQ((int32)orig, (int32)rev);
    }
}

GAME_DIFF_TEST(CPickup2, FindTextIndexForString_Unknown) {
    ePickupPropertyText orig, rev;
    { HookDisableGuard guard("Global/CPickup/FindTextIndexForString");
      orig = CPickup::FindTextIndexForString("INVALID_LABEL"); }
    rev = CPickup::FindTextIndexForString("INVALID_LABEL");
    EXPECT_EQ((int32)orig, (int32)rev);
}

// PickUpShouldBeInvisible — test on pickup array entries
GAME_DIFF_TEST(CPickup2, PickUpShouldBeInvisible_First10) {
    for (int32 i = 0; i < 10; i++) {
        auto& pickup = CPickups::aPickUps[i];
        bool orig, rev;
        { HookDisableGuard guard("Global/CPickup/PickUpShouldBeInvisible");
          orig = pickup.PickUpShouldBeInvisible(); }
        rev = pickup.PickUpShouldBeInvisible();
        EXPECT_EQ(orig, rev);
    }
}
