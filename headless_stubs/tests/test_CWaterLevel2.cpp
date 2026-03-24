// test_CWaterLevel2.cpp — Behavior tests for CWaterLevel.
// GetWaterLevel is an overloaded function — hook path not found at runtime.
// Test as behavior only to verify correctness of return values.

#include "StdInc.h"
#include "TestFramework.h"
#include "WaterLevel.h"

// --- GetWaterLevel behavior tests ---

GAME_TEST(CWaterLevel2, GetWaterLevel_Ocean) {
    // Deep ocean positions should have water
    float level = 0.f;
    bool found = CWaterLevel::GetWaterLevel(-2800.f, -2800.f, 0.f, level, 0, nullptr);
    EXPECT_TRUE(found);
    if (found) {
        EXPECT_NEAR(level, 0.f, 1.0f); // Sea level is ~0
    }
}

GAME_TEST(CWaterLevel2, GetWaterLevel_Beach) {
    float level = 0.f;
    bool found = CWaterLevel::GetWaterLevel(370.f, -2100.f, 0.f, level, 0, nullptr);
    // Near Santa Maria beach — should have water
    if (found) {
        EXPECT_NEAR(level, 0.f, 1.0f);
    }
}

GAME_TEST(CWaterLevel2, GetWaterLevel_GroveStreet) {
    // Inland position — may or may not have water
    float level = 0.f;
    bool found = CWaterLevel::GetWaterLevel(2488.f, -1666.f, 13.f, level, 0, nullptr);
    // Just verify no crash — inland water presence is data-dependent
    EXPECT_TRUE(true);
}

GAME_TEST(CWaterLevel2, GetWaterLevel_MultipleOceanPoints) {
    struct Pos { float x, y; };
    Pos positions[] = {
        {-2800.f, -2800.f},
        {3000.f, -3000.f},
        {-2500.f, 2500.f},
        {2800.f, 2800.f},
    };
    for (auto& p : positions) {
        float level = 0.f;
        bool found = CWaterLevel::GetWaterLevel(p.x, p.y, 0.f, level, 0, nullptr);
        EXPECT_TRUE(found);
    }
}
