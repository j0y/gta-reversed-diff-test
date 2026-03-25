// test_CPointLights2.cpp — Behavior tests for CPointLights.
// CPointLights has no InjectHooks (all plugin::Call wrappers), so behavior tests only.

#include "StdInc.h"
#include "TestFramework.h"

// --- GetLightMultiplier: returns float based on registered lights ---

GAME_TEST(CPointLights, GetLightMultiplier_NoLights) {
    // With no lights registered, multiplier should be 1.0
    auto savedNum = CPointLights::NumLights;
    CPointLights::NumLights = 0;

    CVector testPoint{2350.0f, -1650.0f, 25.0f};
    float result = CPointLights::GetLightMultiplier(&testPoint);

    CPointLights::NumLights = savedNum;
    EXPECT_NEAR(result, 1.0f, 0.01f);
}

GAME_TEST(CPointLights, GetLightMultiplier_WithLight) {
    auto savedNum = CPointLights::NumLights;

    // Add a point light near our test point
    CVector lightPos{2355.0f, -1650.0f, 25.0f};
    CVector lightDir{0.0f, 0.0f, 0.0f};
    CPointLights::AddLight(PLTYPE_POINTLIGHT, lightPos, lightDir, 20.0f, 1.0f, 1.0f, 1.0f, 0, false, nullptr);

    CVector testPoint{2355.0f, -1650.0f, 25.0f};
    float result = CPointLights::GetLightMultiplier(&testPoint);

    // Should be >= 1.0 since a light is nearby
    EXPECT_TRUE(result >= 1.0f);

    CPointLights::NumLights = savedNum;
}

// --- Init: resets all lights ---

GAME_TEST(CPointLights, Init_NoCrash) {
    CPointLights::Init();
    EXPECT_TRUE(true);
}

// --- RemoveLightsAffectingObject ---

GAME_TEST(CPointLights, RemoveLightsAffectingObject_NoCrash) {
    CPointLights::RemoveLightsAffectingObject();
    EXPECT_TRUE(true);
}

// --- AddLight: adds a light to the array ---

// AddLight is a plugin::Call wrapper (not reversed) — may be a no-op.
GAME_TEST(CPointLights, AddLight_NoCrash) {
    CVector pos{2360.0f, -1650.0f, 25.0f};
    CVector dir{0.0f, 0.0f, -1.0f};
    CPointLights::AddLight(PLTYPE_POINTLIGHT, pos, dir, 10.0f, 1.0f, 0.5f, 0.0f);
    EXPECT_TRUE(true);
}

// --- ProcessVerticalLineUsingCache ---

GAME_TEST(CPointLights, ProcessVerticalLineUsingCache_NoCrash) {
    CVector point{2365.0f, -1650.0f, 25.0f};
    float outZ = 0.0f;
    CPointLights::ProcessVerticalLineUsingCache(point, &outZ);
    EXPECT_TRUE(true);
}

// --- GenerateLightsAffectingObject ---

GAME_TEST(CPointLights, GenerateLightsAffectingObject_NoCrash) {
    CVector point{2370.0f, -1650.0f, 25.0f};
    float totalLighting = 0.0f;
    CPointLights::GenerateLightsAffectingObject(&point, &totalLighting, nullptr);
    EXPECT_TRUE(true);
}
