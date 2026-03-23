// test_CPickup.cpp — Deep differential tests for CPickup (single pickup).
// Hook paths: Global/CPickup/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "Pickup.h"

// --- FindStringForTextIndex ---

GAME_DIFF_TEST(CPickup, FindStringForTextIndex_Various) {
    for (int32 i = 0; i < 20; i++) {
        const char* orig;
        const char* rev;
        { HookDisableGuard guard("Global/CPickup/FindStringForTextIndex");
          orig = CPickup::FindStringForTextIndex(static_cast<ePickupPropertyText>(i)); }
        rev = CPickup::FindStringForTextIndex(static_cast<ePickupPropertyText>(i));
        EXPECT_EQ((uintptr_t)orig, (uintptr_t)rev);
    }
}

// --- FindTextIndexForString ---

GAME_DIFF_TEST(CPickup, FindTextIndexForString_KnownStrings) {
    const char* strings[] = { "HEALTH", "ARMOUR", "BRIBE", "KILLFRENZY", "INVALID_XYZ" };
    for (auto* str : strings) {
        int32 orig, rev;
        { HookDisableGuard guard("Global/CPickup/FindTextIndexForString");
          orig = (int32)CPickup::FindTextIndexForString(str); }
        rev = (int32)CPickup::FindTextIndexForString(str);
        EXPECT_EQ(orig, rev);
    }
}
