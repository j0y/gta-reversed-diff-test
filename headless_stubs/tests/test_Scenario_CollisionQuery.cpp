// test_Scenario_CollisionQuery.cpp — CWorld line-of-sight and collision queries.
// Hook paths: Global/CWorld/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "World.h"
#include "Entity/Ped/Ped.h"

// --- GetIsLineOfSightClear ---

GAME_DIFF_TEST(Scenario_CollisionQuery, LineOfSight_NearPlayer) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    CVector start = player->GetPosition();
    CVector end = start + CVector(10.f, 0.f, 0.f);
    bool orig, rev;
    { HookDisableGuard guard("Global/CWorld/GetIsLineOfSightClear");
      orig = CWorld::GetIsLineOfSightClear(start, end, true, false, false, false, false, false, false); }
    rev = CWorld::GetIsLineOfSightClear(start, end, true, false, false, false, false, false, false);
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(Scenario_CollisionQuery, LineOfSight_Vertical) {
    CVector start(2488.f, -1666.f, 13.f);
    CVector end(2488.f, -1666.f, 100.f);
    bool orig, rev;
    { HookDisableGuard guard("Global/CWorld/GetIsLineOfSightClear");
      orig = CWorld::GetIsLineOfSightClear(start, end, true, true, false, false, false, false, false); }
    rev = CWorld::GetIsLineOfSightClear(start, end, true, true, false, false, false, false, false);
    EXPECT_EQ(orig, rev);
}

// --- TestSphereAgainstWorld at multiple positions ---

GAME_DIFF_TEST(Scenario_CollisionQuery, SphereAgainstWorld_Grid) {
    CVector positions[] = {
        {2488.f, -1666.f, 13.f},
        {2488.f, -1666.f, 20.f},
        {2500.f, -1660.f, 13.f},
        {0.f, 0.f, 50.f},
    };
    for (auto& pos : positions) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CWorld/TestSphereAgainstWorld");
          orig = CWorld::TestSphereAgainstWorld(pos, 2.0f, nullptr, true, true, false, false, false, false); }
        rev = CWorld::TestSphereAgainstWorld(pos, 2.0f, nullptr, true, true, false, false, false, false);
        EXPECT_EQ(orig, rev);
    }
}

// --- FindGroundZForCoord extended ---

GAME_DIFF_TEST(Scenario_CollisionQuery, FindGroundZ_Grid) {
    struct Pos { float x, y; };
    Pos positions[] = {
        {2488.f, -1666.f}, {2500.f, -1660.f}, {2480.f, -1680.f},
        {-2170.f, 250.f}, {2027.f, 1008.f}, {0.f, 0.f},
    };
    for (auto& p : positions) {
        float orig, rev;
        { HookDisableGuard guard("Global/CWorld/FindGroundZForCoord");
          orig = CWorld::FindGroundZForCoord(p.x, p.y); }
        rev = CWorld::FindGroundZForCoord(p.x, p.y);
        EXPECT_NEAR(orig, rev, 1e-3f);
    }
}
