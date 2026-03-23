// test_CPathFind2.cpp — Deep differential tests for CPathFind.
// Hook paths: Global/CPathFind/<fn>
//
// CPathFind methods are instance methods on ThePaths global.

#include "StdInc.h"
#include "TestFramework.h"
#include "PathFind.h"

// --- FindXRegionForCoors ---

GAME_DIFF_TEST(CPathFind, FindXRegionForCoors_Various) {
    float coords[] = { -3000.0f, -1000.0f, 0.0f, 1000.0f, 2000.0f, 2495.0f, 3000.0f };
    for (float x : coords) {
        size_t orig, rev;
        { HookDisableGuard guard("Global/CPathFind/FindXRegionForCoors"); orig = ThePaths.FindXRegionForCoors(x); }
        rev = ThePaths.FindXRegionForCoors(x);
        EXPECT_EQ(orig, rev);
    }
}

// --- FindYRegionForCoors ---

GAME_DIFF_TEST(CPathFind, FindYRegionForCoors_Various) {
    float coords[] = { -3000.0f, -1665.0f, -1000.0f, 0.0f, 1000.0f, 2000.0f, 3000.0f };
    for (float y : coords) {
        size_t orig, rev;
        { HookDisableGuard guard("Global/CPathFind/FindYRegionForCoors"); orig = ThePaths.FindYRegionForCoors(y); }
        rev = ThePaths.FindYRegionForCoors(y);
        EXPECT_EQ(orig, rev);
    }
}

// --- FindXCoorsForRegion ---

GAME_DIFF_TEST(CPathFind, FindXCoorsForRegion_Various) {
    size_t regions[] = { 0, 1, 5, 10, 20, 50 };
    for (size_t r : regions) {
        float orig, rev;
        { HookDisableGuard guard("Global/CPathFind/FindXCoorsForRegion"); orig = ThePaths.FindXCoorsForRegion(r); }
        rev = ThePaths.FindXCoorsForRegion(r);
        EXPECT_NEAR(orig, rev, 1e-3f);
    }
}

// --- FindYCoorsForRegion ---

GAME_DIFF_TEST(CPathFind, FindYCoorsForRegion_Various) {
    size_t regions[] = { 0, 1, 5, 10, 20, 50 };
    for (size_t r : regions) {
        float orig, rev;
        { HookDisableGuard guard("Global/CPathFind/FindYCoorsForRegion"); orig = ThePaths.FindYCoorsForRegion(r); }
        rev = ThePaths.FindYCoorsForRegion(r);
        EXPECT_NEAR(orig, rev, 1e-3f);
    }
}
