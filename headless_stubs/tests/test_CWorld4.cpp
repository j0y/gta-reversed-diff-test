// test_CWorld4.cpp — Extended differential tests for CWorld.
// Hook paths: Global/CWorld/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "World.h"
#include "Entity/Ped/Ped.h"
#include "ScenarioHelpers.h"

// --- FindNearestObjectOfType ---

GAME_DIFF_TEST(CWorld4, FindNearestObjectOfType_Vehicle596) {
    // Look for model 596 (police car) near Grove Street
    CVector pos(2495.0f, -1665.0f, 13.5f);
    CEntity* orig;
    CEntity* rev;
    { HookDisableGuard guard("Global/CWorld/FindNearestObjectOfType");
      orig = CWorld::FindNearestObjectOfType(596, pos, 100.0f, false, false, true, false, false, false); }
    rev = CWorld::FindNearestObjectOfType(596, pos, 100.0f, false, false, true, false, false, false);
    EXPECT_EQ((uintptr_t)orig, (uintptr_t)rev);
}

GAME_DIFF_TEST(CWorld4, FindNearestObjectOfType_NoResult) {
    // Look for a model that doesn't exist nearby
    CVector pos(2495.0f, -1665.0f, 13.5f);
    CEntity* orig;
    CEntity* rev;
    { HookDisableGuard guard("Global/CWorld/FindNearestObjectOfType");
      orig = CWorld::FindNearestObjectOfType(0, pos, 5.0f, false, false, false, false, false, false); }
    rev = CWorld::FindNearestObjectOfType(0, pos, 5.0f, false, false, false, false, false, false);
    EXPECT_EQ((uintptr_t)orig, (uintptr_t)rev);
}

// --- FindGroundZForCoord extended city coverage ---

GAME_DIFF_TEST(CWorld4, FindGroundZForCoord_AllCities) {
    struct Pos { float x, y; const char* name; };
    Pos positions[] = {
        {2495.0f, -1665.0f, "GroveStreet"},
        {1481.0f, -1764.0f, "DowntownLS"},
        {-2170.0f, 250.0f, "SanFierro"},
        {2027.0f, 1008.0f, "LasVenturas"},
        {-789.0f, -2600.0f, "Ocean"},
        {400.0f, -1500.0f, "Unity"},
        {-300.0f, 1500.0f, "Countryside"},
    };
    for (auto& p : positions) {
        float orig, rev;
        { HookDisableGuard guard("Global/CWorld/FindGroundZForCoord");
          orig = CWorld::FindGroundZForCoord(p.x, p.y); }
        rev = CWorld::FindGroundZForCoord(p.x, p.y);
        EXPECT_NEAR(orig, rev, 1e-3f);
    }
}

// --- FindLowestZForCoord extended ---

GAME_DIFF_TEST(CWorld4, FindLowestZForCoord_Various) {
    struct Pos { float x, y; };
    Pos positions[] = {
        {2495.0f, -1665.0f}, {0.0f, 0.0f}, {-2170.0f, 250.0f},
        {2027.0f, 1008.0f}, {-789.0f, -2600.0f},
    };
    for (auto& p : positions) {
        float orig, rev;
        { HookDisableGuard guard("Global/CWorld/FindLowestZForCoord");
          orig = CWorld::FindLowestZForCoord(p.x, p.y); }
        rev = CWorld::FindLowestZForCoord(p.x, p.y);
        EXPECT_NEAR(orig, rev, 1e-3f);
    }
}
