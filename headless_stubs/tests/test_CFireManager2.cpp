// test_CFireManager2.cpp — Extended differential tests for CFireManager.
// Hook paths: Global/CFireManager/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "FireManager.h"

// --- FindNearestFire ---

GAME_DIFF_TEST(CFireManager2, FindNearestFire_MultiplePositions) {
    CVector positions[] = {
        {2488.f, -1666.f, 13.f},
        {0.f, 0.f, 0.f},
        {-2170.f, 250.f, 30.f},
    };
    for (auto& pos : positions) {
        CFire* orig;
        CFire* rev;
        { HookDisableGuard guard("Global/CFireManager/FindNearestFire");
          orig = gFireManager.FindNearestFire(pos); }
        rev = gFireManager.FindNearestFire(pos);
        EXPECT_EQ(orig, rev);
    }
}

// --- IsScriptFireExtinguished ---

GAME_DIFF_TEST(CFireManager2, IsScriptFireExtinguished_InvalidId) {
    bool orig, rev;
    { HookDisableGuard guard("Global/CFireManager/IsScriptFireExtinguished");
      orig = gFireManager.IsScriptFireExtinguished(0); }
    rev = gFireManager.IsScriptFireExtinguished(0);
    EXPECT_EQ(orig, rev);
}

// --- ClearAllScriptFireFlags ---

GAME_DIFF_TEST(CFireManager2, ClearAllScriptFireFlags) {
    { HookDisableGuard guard("Global/CFireManager/ClearAllScriptFireFlags");
      gFireManager.ClearAllScriptFireFlags(); }
    gFireManager.ClearAllScriptFireFlags();
    EXPECT_TRUE(true);
}
