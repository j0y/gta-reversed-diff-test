// test_CPathFind3.cpp — Extended differential tests for CPathFind.
// Hook paths: Global/CPathFind/<fn>
//
// Tests pathfinding queries at various world positions.
// FindNodeClosestToCoorsFavourDirection takes (CVector, ePathType, CVector2D).

#include "StdInc.h"
#include "TestFramework.h"
#include "PathFind.h"

// --- FindNodeClosestToCoorsFavourDirection ---

GAME_DIFF_TEST(CPathFind3, FindNodeClosestFavourDir_MultiplePositions) {
    struct TestCase { CVector pos; CVector2D dir; };
    TestCase cases[] = {
        { {2488.f, -1666.f, 13.f}, {0.f, 1.f} },        // Grove Street, north
        { {2488.f, -1666.f, 13.f}, {1.f, 0.f} },        // Grove Street, east
        { {2488.f, -1666.f, 13.f}, {0.f, -1.f} },       // Grove Street, south
        { {-2170.f, 250.f, 30.f}, {0.f, 1.f} },         // San Fierro, north
        { {2027.f, 1008.f, 20.f}, {0.f, 1.f} },         // Las Venturas, north
        { {0.f, 0.f, 0.f}, {1.f, 0.f} },                // Origin, east
    };
    for (auto& tc : cases) {
        CNodeAddress orig, rev;
        { HookDisableGuard guard("Global/CPathFind/FindNodeClosestToCoorsFavourDirection");
          orig = ThePaths.FindNodeClosestToCoorsFavourDirection(tc.pos, PATH_TYPE_PED, tc.dir); }
        rev = ThePaths.FindNodeClosestToCoorsFavourDirection(tc.pos, PATH_TYPE_PED, tc.dir);
        EXPECT_EQ(orig.IsValid(), rev.IsValid());
        if (orig.IsValid() && rev.IsValid()) {
            EXPECT_EQ(orig.m_wAreaId, rev.m_wAreaId);
            EXPECT_EQ(orig.m_wNodeId, rev.m_wNodeId);
        }
    }
}

GAME_DIFF_TEST(CPathFind3, FindNodeClosestFavourDir_VehiclePaths) {
    CVector pos(2488.f, -1666.f, 13.f);
    CVector2D dir(1.f, 0.f);
    CNodeAddress orig, rev;
    { HookDisableGuard guard("Global/CPathFind/FindNodeClosestToCoorsFavourDirection");
      orig = ThePaths.FindNodeClosestToCoorsFavourDirection(pos, PATH_TYPE_VEH, dir); }
    rev = ThePaths.FindNodeClosestToCoorsFavourDirection(pos, PATH_TYPE_VEH, dir);
    EXPECT_EQ(orig.IsValid(), rev.IsValid());
    if (orig.IsValid() && rev.IsValid()) {
        EXPECT_EQ(orig.m_wAreaId, rev.m_wAreaId);
        EXPECT_EQ(orig.m_wNodeId, rev.m_wNodeId);
    }
}

// --- FindXRegionForCoors extended ---

GAME_DIFF_TEST(CPathFind3, FindXRegionForCoors_Boundaries) {
    float coords[] = { -3000.f, -2999.f, -1500.f, -1.f, 0.f, 1.f, 1500.f, 2999.f, 3000.f };
    for (float x : coords) {
        size_t orig, rev;
        { HookDisableGuard guard("Global/CPathFind/FindXRegionForCoors");
          orig = ThePaths.FindXRegionForCoors(x); }
        rev = ThePaths.FindXRegionForCoors(x);
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(CPathFind3, FindYRegionForCoors_Boundaries) {
    float coords[] = { -3000.f, -2999.f, -1500.f, -1.f, 0.f, 1.f, 1500.f, 2999.f, 3000.f };
    for (float y : coords) {
        size_t orig, rev;
        { HookDisableGuard guard("Global/CPathFind/FindYRegionForCoors");
          orig = ThePaths.FindYRegionForCoors(y); }
        rev = ThePaths.FindYRegionForCoors(y);
        EXPECT_EQ(orig, rev);
    }
}
