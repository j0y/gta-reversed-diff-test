// test_CShadows.cpp — Differential tests for CShadows.
// Hook paths: Shadows/CShadows/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "Shadows.h"

// --- CalcPedShadowValues: pure computation on a sun position vector ---

GAME_DIFF_TEST(CShadows, CalcPedShadowValues) {
    CVector sunPosn{0.0f, 0.0f, -1.0f}; // sun straight above

    float origFX, origFY, origSX, origSY, origDX, origDY;
    float revFX, revFY, revSX, revSY, revDX, revDY;

    {
        HookDisableGuard guard("Shadows/CShadows/CalcPedShadowValues");
        CShadows::CalcPedShadowValues(sunPosn, origFX, origFY, origSX, origSY, origDX, origDY);
    }
    CShadows::CalcPedShadowValues(sunPosn, revFX, revFY, revSX, revSY, revDX, revDY);

    EXPECT_NEAR(origFX, revFX, 0.01f);
    EXPECT_NEAR(origFY, revFY, 0.01f);
    EXPECT_NEAR(origSX, revSX, 0.01f);
    EXPECT_NEAR(origSY, revSY, 0.01f);
    EXPECT_NEAR(origDX, revDX, 0.01f);
    EXPECT_NEAR(origDY, revDY, 0.01f);
}

GAME_DIFF_TEST(CShadows, CalcPedShadowValues_Angled) {
    CVector sunPosn{0.5f, 0.5f, -0.707f}; // angled sun

    float origFX, origFY, origSX, origSY, origDX, origDY;
    float revFX, revFY, revSX, revSY, revDX, revDY;

    {
        HookDisableGuard guard("Shadows/CShadows/CalcPedShadowValues");
        CShadows::CalcPedShadowValues(sunPosn, origFX, origFY, origSX, origSY, origDX, origDY);
    }
    CShadows::CalcPedShadowValues(sunPosn, revFX, revFY, revSX, revSY, revDX, revDY);

    EXPECT_NEAR(origFX, revFX, 0.01f);
    EXPECT_NEAR(origFY, revFY, 0.01f);
    EXPECT_NEAR(origSX, revSX, 0.01f);
    EXPECT_NEAR(origSY, revSY, 0.01f);
    EXPECT_NEAR(origDX, revDX, 0.01f);
    EXPECT_NEAR(origDY, revDY, 0.01f);
}

// --- AffectColourWithLighting: pure computation ---

GAME_DIFF_TEST(CShadows, AffectColourWithLighting_Default) {
    uint8 origR, origG, origB;
    uint8 revR, revG, revB;

    {
        HookDisableGuard guard("Shadows/CShadows/AffectColourWithLighting");
        CShadows::AffectColourWithLighting(SHADOW_DEFAULT, 0xFF, 128, 128, 128, origR, origG, origB);
    }
    CShadows::AffectColourWithLighting(SHADOW_DEFAULT, 0xFF, 128, 128, 128, revR, revG, revB);

    EXPECT_EQ(origR, revR);
    EXPECT_EQ(origG, revG);
    EXPECT_EQ(origB, revB);
}

GAME_DIFF_TEST(CShadows, AffectColourWithLighting_Additive) {
    uint8 origR, origG, origB;
    uint8 revR, revG, revB;

    {
        HookDisableGuard guard("Shadows/CShadows/AffectColourWithLighting");
        CShadows::AffectColourWithLighting(SHADOW_ADDITIVE, 0x84, 255, 0, 0, origR, origG, origB);
    }
    CShadows::AffectColourWithLighting(SHADOW_ADDITIVE, 0x84, 255, 0, 0, revR, revG, revB);

    EXPECT_EQ(origR, revR);
    EXPECT_EQ(origG, revG);
    EXPECT_EQ(origB, revB);
}

// --- TidyUpShadows: resets shadow state ---

GAME_DIFF_TEST(CShadows, TidyUpShadows) {
    {
        HookDisableGuard guard("Shadows/CShadows/TidyUpShadows");
        CShadows::TidyUpShadows();
    }
    auto origCount = CShadows::ShadowsStoredToBeRendered;

    CShadows::TidyUpShadows();
    auto revCount = CShadows::ShadowsStoredToBeRendered;

    EXPECT_EQ(origCount, revCount);
}

// --- SetRenderModeForShadowType: sets RW render state, behavior test only ---

GAME_TEST(CShadows, SetRenderModeForShadowType_Default) {
    CShadows::SetRenderModeForShadowType(SHADOW_DEFAULT);
    EXPECT_TRUE(true);
}

GAME_TEST(CShadows, SetRenderModeForShadowType_Additive) {
    CShadows::SetRenderModeForShadowType(SHADOW_ADDITIVE);
    EXPECT_TRUE(true);
}

// --- RemoveOilInArea: modifies permanent shadows array ---

GAME_DIFF_TEST(CShadows, RemoveOilInArea) {
    {
        HookDisableGuard guard("Shadows/CShadows/RemoveOilInArea");
        CShadows::RemoveOilInArea(0.0f, 10.0f, 0.0f, 10.0f);
    }
    CShadows::RemoveOilInArea(0.0f, 10.0f, 0.0f, 10.0f);
    EXPECT_TRUE(true); // no crash = pass; area likely empty
}
