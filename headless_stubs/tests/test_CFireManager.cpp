// test_CFireManager.cpp — Differential tests for CFireManager.
// Hook paths: Global/CFireManager/<fn>
// gFireManager is a global instance at 0xB71F80.

#include "StdInc.h"
#include "TestFramework.h"
#include "FireManager.h"

GAME_DIFF_TEST(CFireManager, GetNumOfNonScriptFires) {
    uint32 orig, rev;
    { HookDisableGuard guard("Global/CFireManager/GetNumOfNonScriptFires"); orig = gFireManager.GetNumOfNonScriptFires(); }
    rev = gFireManager.GetNumOfNonScriptFires();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CFireManager, PlentyFiresAvailable) {
    bool orig, rev;
    { HookDisableGuard guard("Global/CFireManager/PlentyFiresAvailable"); orig = gFireManager.PlentyFiresAvailable(); }
    rev = gFireManager.PlentyFiresAvailable();
    if (orig != rev) {
        char buf[64];
        snprintf(buf, sizeof(buf), "PlentyFiresAvailable: orig=%d rev=%d", (int)orig, (int)rev);
        GetTestContext().RecordFailure("test_CFireManager.cpp", __LINE__, buf);
        return;
    }
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CFireManager, GetNumFiresInRange) {
    CVector positions[] = {
        { 2488.0f, -1666.0f, 13.0f },
        { 0.0f, 0.0f, 0.0f },
    };
    float radii[] = { 100.0f, 1000.0f };
    for (auto& pos : positions) {
        for (float r : radii) {
            uint32 orig, rev;
            { HookDisableGuard guard("Global/CFireManager/GetNumFiresInRange");
              orig = gFireManager.GetNumFiresInRange(pos, r * r); }
            rev = gFireManager.GetNumFiresInRange(pos, r * r);
            EXPECT_EQ(orig, rev);
        }
    }
}

GAME_DIFF_TEST(CFireManager, GetNumFiresInArea) {
    // Test around Grove Street area
    float orig, rev;
    { HookDisableGuard guard("Global/CFireManager/GetNumFiresInArea");
      orig = (float)gFireManager.GetNumFiresInArea(2400.f, -1750.f, 0.f, 2550.f, -1600.f, 100.f); }
    rev = (float)gFireManager.GetNumFiresInArea(2400.f, -1750.f, 0.f, 2550.f, -1600.f, 100.f);
    EXPECT_NEAR(orig, rev, 0);
}
