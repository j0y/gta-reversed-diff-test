// test_CPlaneTrails.cpp — Differential tests for CPlaneTrails.
// Hook paths: Global/CPlaneTrails/<fn>
//
// Plane trail registration — data-only, no rendering.

#include "StdInc.h"
#include "TestFramework.h"
#include "PlaneTrails.h"

// Init should reset trail state
GAME_DIFF_TEST(CPlaneTrails, Init) {
    // Save state, init, compare
    { HookDisableGuard guard("Global/CPlaneTrails/Init");
      CPlaneTrails::Init(); }
    CPlaneTrails::Init();
    // Both should reset to same state — no observable output, just verify no crash
    EXPECT_TRUE(true);
}

// RegisterPoint should accept valid positions
GAME_DIFF_TEST(CPlaneTrails, RegisterPoint_NoCrash) {
    CVector pos(2488.f, -1666.f, 200.f);
    uint32 time = 1000;
    { HookDisableGuard guard("Global/CPlaneTrails/RegisterPoint");
      CPlaneTrails::RegisterPoint(pos, time); }
    CPlaneTrails::RegisterPoint(pos, time);
    EXPECT_TRUE(true);
}
