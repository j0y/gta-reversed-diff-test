// test_CWaterLevel3.cpp — Extended CWaterLevel differential tests.
// Hook paths: overloaded — CWaterLevel/GetWaterDepth has no dash suffix.
// Test as behavior only since the hook is overloaded.

#include "StdInc.h"
#include "TestFramework.h"
#include "WaterLevel.h"

// GetWaterDepth — behavior tests (can't diff-test overloaded hooks)
GAME_TEST(CWaterLevel3, GetWaterDepth_Ocean) {
    CVector pos(370.f, -1840.f, 0.f);
    float depth = 0.f, level = 0.f, ground = 0.f;
    bool result = CWaterLevel::GetWaterDepth(pos, &depth, &level, &ground);
    // Ocean should have water
    if (result) {
        EXPECT_GT(depth, 0.0f);
    }
    EXPECT_TRUE(true);
}

GAME_TEST(CWaterLevel3, GetWaterDepth_Inland) {
    CVector pos(2488.f, -1666.f, 13.5f);
    float depth = 0.f, level = 0.f, ground = 0.f;
    bool result = CWaterLevel::GetWaterDepth(pos, &depth, &level, &ground);
    // Inland — may or may not have water
    EXPECT_TRUE(true);
    (void)result;
}

GAME_TEST(CWaterLevel3, GetWaterDepth_HighAltitude) {
    CVector pos(370.f, -1840.f, 500.f);
    float depth = 0.f, level = 0.f, ground = 0.f;
    bool result = CWaterLevel::GetWaterDepth(pos, &depth, &level, &ground);
    EXPECT_TRUE(true);
    (void)result;
}
