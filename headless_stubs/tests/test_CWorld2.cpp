// test_CWorld2.cpp — Deep differential tests for CWorld ground/roof Z queries.
// Hook paths: Global/CWorld/<fn>
//
// Ground Z and roof Z lookups are used everywhere — camera, ped placement,
// vehicle physics. Bugs here cause floating/sinking entities.

#include "StdInc.h"
#include "TestFramework.h"
#include "World.h"

// --- FindGroundZForCoord ---

GAME_DIFF_TEST(CWorld, FindGroundZForCoord_GroveStreet) {
    float orig, rev;
    { HookDisableGuard guard("Global/CWorld/FindGroundZForCoord");
      orig = CWorld::FindGroundZForCoord(2495.0f, -1665.0f); }
    rev = CWorld::FindGroundZForCoord(2495.0f, -1665.0f);
    EXPECT_NEAR(orig, rev, 1e-3f);
}

GAME_DIFF_TEST(CWorld, FindGroundZForCoord_Downtown) {
    float orig, rev;
    { HookDisableGuard guard("Global/CWorld/FindGroundZForCoord");
      orig = CWorld::FindGroundZForCoord(1481.0f, -1764.0f); }
    rev = CWorld::FindGroundZForCoord(1481.0f, -1764.0f);
    EXPECT_NEAR(orig, rev, 1e-3f);
}

GAME_DIFF_TEST(CWorld, FindGroundZForCoord_MultiplePositions) {
    struct Pos { float x, y; };
    Pos positions[] = {
        {2495.0f, -1665.0f},   // Grove Street
        {0.0f, 0.0f},          // World origin
        {1000.0f, -1000.0f},   // Random LS
        {-2170.0f, 250.0f},    // San Fierro
        {2027.0f, 1008.0f},    // Las Venturas
    };
    for (auto& p : positions) {
        float orig, rev;
        { HookDisableGuard guard("Global/CWorld/FindGroundZForCoord");
          orig = CWorld::FindGroundZForCoord(p.x, p.y); }
        rev = CWorld::FindGroundZForCoord(p.x, p.y);
        EXPECT_NEAR(orig, rev, 1e-3f);
    }
}

// --- FindGroundZFor3DCoord ---

GAME_DIFF_TEST(CWorld, FindGroundZFor3DCoord_GroveStreet) {
    float orig, rev;
    { HookDisableGuard guard("Global/CWorld/FindGroundZFor3DCoord");
      orig = CWorld::FindGroundZFor3DCoord(CVector(2495.0f, -1665.0f, 100.0f)); }
    rev = CWorld::FindGroundZFor3DCoord(CVector(2495.0f, -1665.0f, 100.0f));
    EXPECT_NEAR(orig, rev, 1e-3f);
}

GAME_DIFF_TEST(CWorld, FindGroundZFor3DCoord_Various) {
    struct Pos { float x, y, z; };
    Pos positions[] = {
        {2495.0f, -1665.0f, 100.0f},
        {0.0f, 0.0f, 50.0f},
        {1000.0f, -1000.0f, 200.0f},
    };
    for (auto& p : positions) {
        float orig, rev;
        { HookDisableGuard guard("Global/CWorld/FindGroundZFor3DCoord");
          orig = CWorld::FindGroundZFor3DCoord(CVector(p.x, p.y, p.z)); }
        rev = CWorld::FindGroundZFor3DCoord(CVector(p.x, p.y, p.z));
        EXPECT_NEAR(orig, rev, 1e-3f);
    }
}

// --- FindLowestZForCoord ---

GAME_DIFF_TEST(CWorld, FindLowestZForCoord_GroveStreet) {
    float orig, rev;
    { HookDisableGuard guard("Global/CWorld/FindLowestZForCoord");
      orig = CWorld::FindLowestZForCoord(2495.0f, -1665.0f); }
    rev = CWorld::FindLowestZForCoord(2495.0f, -1665.0f);
    EXPECT_NEAR(orig, rev, 1e-3f);
}

// --- FindRoofZFor3DCoord ---

GAME_DIFF_TEST(CWorld, FindRoofZFor3DCoord_GroveStreet) {
    float orig, rev;
    { HookDisableGuard guard("Global/CWorld/FindRoofZFor3DCoord");
      orig = CWorld::FindRoofZFor3DCoord(2495.0f, -1665.0f, 13.5f, nullptr); }
    rev = CWorld::FindRoofZFor3DCoord(2495.0f, -1665.0f, 13.5f, nullptr);
    EXPECT_NEAR(orig, rev, 1e-3f);
}
