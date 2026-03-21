// test_CWorld.cpp — Differential tests for CWorld.
// Hook paths: Global/CWorld/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "World.h"

GAME_DIFF_TEST(CWorld, FindGroundZForCoord) {
    float coords[][2] = { {0,0}, {2488,-1666}, {-2200,2300}, {500,500}, {-500,-500} };
    for (auto& c : coords) {
        float orig, rev;
        { HookDisableGuard guard("Global/CWorld/FindGroundZForCoord");
          orig = CWorld::FindGroundZForCoord(c[0], c[1]); }
        rev = CWorld::FindGroundZForCoord(c[0], c[1]);
        EXPECT_NEAR(orig, rev, 1e-3f);
    }
}

GAME_DIFF_TEST(CWorld, FindLowestZForCoord) {
    float coords[][2] = { {0,0}, {2488,-1666}, {-2200,2300} };
    for (auto& c : coords) {
        float orig, rev;
        { HookDisableGuard guard("Global/CWorld/FindLowestZForCoord");
          orig = CWorld::FindLowestZForCoord(c[0], c[1]); }
        rev = CWorld::FindLowestZForCoord(c[0], c[1]);
        EXPECT_NEAR(orig, rev, 1e-3f);
    }
}

GAME_DIFF_TEST(CWorld, GetIsLineOfSightClear) {
    CVector origin(2488, -1666, 20);
    CVector targets[] = { {2488, -1666, 100}, {2500, -1650, 20}, {2488, -1666, 13} };
    for (auto& target : targets) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CWorld/GetIsLineOfSightClear");
          orig = CWorld::GetIsLineOfSightClear(origin, target, true, true, true, true); }
        rev = CWorld::GetIsLineOfSightClear(origin, target, true, true, true, true);
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(CWorld, FindGroundZFor3DCoord) {
    // Test at player spawn + other locations
    CVector coords[] = {
        { 2488.6f, -1666.9f, 50.0f },   // above Grove Street
        { 2488.6f, -1666.9f, 13.9f },   // at ground level
        { 0.0f, 0.0f, 100.0f },          // map center
        { 1000.0f, -1000.0f, 50.0f },   // mid-LS
    };
    for (auto& c : coords) {
        bool origResult = false, revResult = false;
        float orig, rev;
        { HookDisableGuard guard("Global/CWorld/FindGroundZFor3DCoord");
          orig = CWorld::FindGroundZFor3DCoord(c, &origResult); }
        rev = CWorld::FindGroundZFor3DCoord(c, &revResult);
        EXPECT_NEAR(orig, rev, 1e-3f);
        EXPECT_EQ(origResult, revResult);
    }
}

GAME_DIFF_TEST(CWorld, FindRoofZFor3DCoord) {
    float coords[][3] = {
        { 2488.6f, -1666.9f, 10.0f },
        { 0.0f, 0.0f, 10.0f },
    };
    for (auto& c : coords) {
        bool origResult = false, revResult = false;
        float orig, rev;
        { HookDisableGuard guard("Global/CWorld/FindRoofZFor3DCoord");
          orig = CWorld::FindRoofZFor3DCoord(c[0], c[1], c[2], &origResult); }
        rev = CWorld::FindRoofZFor3DCoord(c[0], c[1], c[2], &revResult);
        EXPECT_NEAR(orig, rev, 1e-3f);
        EXPECT_EQ(origResult, revResult);
    }
}
