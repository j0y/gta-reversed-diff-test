// test_CWorld3.cpp — Extended differential tests for CWorld.
// Hook paths: Global/CWorld/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "World.h"
#include "Entity/Ped/Ped.h"

// --- GetIsLineOfSightClear ---

GAME_DIFF_TEST(CWorld, GetIsLineOfSightClear_NearPlayer) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    CVector from = player->GetPosition();
    CVector to = from + CVector(10.0f, 0.0f, 0.0f);
    bool orig, rev;
    { HookDisableGuard guard("Global/CWorld/GetIsLineOfSightClear");
      orig = CWorld::GetIsLineOfSightClear(from, to, true, true, false, true, false); }
    rev = CWorld::GetIsLineOfSightClear(from, to, true, true, false, true, false);
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CWorld, GetIsLineOfSightClear_LongRange) {
    CVector from(2495.0f, -1665.0f, 20.0f);
    CVector to(2495.0f, -1665.0f + 100.0f, 20.0f);
    bool orig, rev;
    { HookDisableGuard guard("Global/CWorld/GetIsLineOfSightClear");
      orig = CWorld::GetIsLineOfSightClear(from, to, true, true, false, true, false); }
    rev = CWorld::GetIsLineOfSightClear(from, to, true, true, false, true, false);
    EXPECT_EQ(orig, rev);
}

// --- FindGroundZForCoord with more positions ---

GAME_DIFF_TEST(CWorld, FindGroundZForCoord_EdgeCases) {
    struct Pos { float x, y; };
    Pos positions[] = {
        {-3000.0f, -3000.0f},  // map corner
        {3000.0f, 3000.0f},    // opposite corner
        {82.0f, -1021.0f},     // near train crossing
        {-2170.0f, 250.0f},    // San Fierro
    };
    for (auto& p : positions) {
        float orig, rev;
        { HookDisableGuard guard("Global/CWorld/FindGroundZForCoord");
          orig = CWorld::FindGroundZForCoord(p.x, p.y); }
        rev = CWorld::FindGroundZForCoord(p.x, p.y);
        EXPECT_NEAR(orig, rev, 1e-3f);
    }
}

// --- FindRoofZFor3DCoord at various heights ---

GAME_DIFF_TEST(CWorld, FindRoofZFor3DCoord_Various) {
    struct Pos { float x, y, z; };
    Pos positions[] = {
        {2495.0f, -1665.0f, 13.5f},
        {2495.0f, -1665.0f, 50.0f},
        {0.0f, 0.0f, 10.0f},
    };
    for (auto& p : positions) {
        float orig, rev;
        { HookDisableGuard guard("Global/CWorld/FindRoofZFor3DCoord");
          orig = CWorld::FindRoofZFor3DCoord(p.x, p.y, p.z, nullptr); }
        rev = CWorld::FindRoofZFor3DCoord(p.x, p.y, p.z, nullptr);
        EXPECT_NEAR(orig, rev, 1e-3f);
    }
}
