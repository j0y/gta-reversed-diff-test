// test_CTagManager2.cpp — Deep differential tests for CTagManager.
// Hook paths: Global/CTagManager/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "TagManager.h"
#include "Core/Rect.h"

// --- GetPercentageTagged ---

GAME_DIFF_TEST(CTagManager, GetPercentageTagged_Diff) {
    float orig, rev;
    { HookDisableGuard guard("Global/CTagManager/GetPercentageTagged");
      orig = CTagManager::GetPercentageTagged(); }
    rev = CTagManager::GetPercentageTagged();
    EXPECT_NEAR(orig, rev, 1e-5f);
}

// --- GetPercentageTaggedInArea ---

GAME_DIFF_TEST(CTagManager, GetPercentageTaggedInArea_GroveStreet) {
    CRect area(2400.0f, -1750.0f, 2550.0f, -1600.0f);
    int32 orig, rev;
    { HookDisableGuard guard("Global/CTagManager/GetPercentageTaggedInArea");
      orig = CTagManager::GetPercentageTaggedInArea(&area); }
    rev = CTagManager::GetPercentageTaggedInArea(&area);
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CTagManager, GetPercentageTaggedInArea_WholeMap) {
    CRect area(-3000.0f, -3000.0f, 3000.0f, 3000.0f);
    int32 orig, rev;
    { HookDisableGuard guard("Global/CTagManager/GetPercentageTaggedInArea");
      orig = CTagManager::GetPercentageTaggedInArea(&area); }
    rev = CTagManager::GetPercentageTaggedInArea(&area);
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CTagManager, GetPercentageTaggedInArea_Empty) {
    CRect area(3000.0f, 3000.0f, 3100.0f, 3100.0f);
    int32 orig, rev;
    { HookDisableGuard guard("Global/CTagManager/GetPercentageTaggedInArea");
      orig = CTagManager::GetPercentageTaggedInArea(&area); }
    rev = CTagManager::GetPercentageTaggedInArea(&area);
    EXPECT_EQ(orig, rev);
}
