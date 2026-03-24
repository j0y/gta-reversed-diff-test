// test_CIplStore.cpp — Differential tests for CIplStore.
// Hook paths: Global/CIplStore/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "IplStore.h"

// --- FindIplSlot ---

GAME_DIFF_TEST(CIplStore, FindIplSlot_KnownNames) {
    const char* names[] = { "LAe", "LAe2", "LAn", "LAn2", "LAs", "LAs2", "LAw", "LAw2" };
    for (auto* name : names) {
        int32 orig, rev;
        { HookDisableGuard guard("Global/CIplStore/FindIplSlot");
          orig = CIplStore::FindIplSlot(name); }
        rev = CIplStore::FindIplSlot(name);
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(CIplStore, FindIplSlot_Invalid) {
    int32 orig, rev;
    { HookDisableGuard guard("Global/CIplStore/FindIplSlot");
      orig = CIplStore::FindIplSlot("nonexistent_ipl"); }
    rev = CIplStore::FindIplSlot("nonexistent_ipl");
    EXPECT_EQ(orig, rev);
}

// --- GetBoundingBox ---

GAME_DIFF_TEST(CIplStore, GetBoundingBox_FirstSlots) {
    for (int32 slot = 0; slot < 10; slot++) {
        CRect* orig;
        CRect* rev;
        { HookDisableGuard guard("Global/CIplStore/GetBoundingBox");
          orig = CIplStore::GetBoundingBox(slot); }
        rev = CIplStore::GetBoundingBox(slot);
        EXPECT_EQ(orig, rev);
    }
}

// --- GetIplName ---

GAME_DIFF_TEST(CIplStore, GetIplName_FirstSlots) {
    for (int32 slot = 0; slot < 10; slot++) {
        const char* orig;
        const char* rev;
        { HookDisableGuard guard("Global/CIplStore/GetIplName");
          orig = CIplStore::GetIplName(slot); }
        rev = CIplStore::GetIplName(slot);
        // Both should point to same string or both null
        if (orig && rev) {
            EXPECT_TRUE(strcmp(orig, rev) == 0);
        } else {
            EXPECT_EQ(orig, rev);
        }
    }
}
