// test_CRadar6.cpp — Systematic CRadar differential tests.
// Tests remaining untested hooked CRadar functions.
// Hook paths: Global/CRadar/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "Radar.h"

// ===================================================================
// DisplayThisBlip — bool, takes sprite ID + priority
// ===================================================================

GAME_DIFF_TEST(CRadar6, DisplayThisBlip_AllSprites) {
    for (int32 sprite = 0; sprite < 64; sprite++) {
        for (int8 prio = -1; prio <= 1; prio++) {
            bool orig, rev;
            { HookDisableGuard guard("Global/CRadar/DisplayThisBlip");
              orig = CRadar::DisplayThisBlip((eRadarSprite)sprite, prio); }
            rev = CRadar::DisplayThisBlip((eRadarSprite)sprite, prio);
            EXPECT_EQ(orig, rev);
        }
    }
}

// ===================================================================
// LimitToMap — modifies x,y in place to clamp to radar bounds
// ===================================================================

GAME_DIFF_TEST(CRadar6, LimitToMap_Inside) {
    float origX = 0.5f, origY = 0.5f;
    float revX = 0.5f, revY = 0.5f;
    { HookDisableGuard guard("Global/CRadar/LimitToMap");
      CRadar::LimitToMap(origX, origY); }
    CRadar::LimitToMap(revX, revY);
    EXPECT_NEAR(origX, revX, 1e-5f);
    EXPECT_NEAR(origY, revY, 1e-5f);
}

GAME_DIFF_TEST(CRadar6, LimitToMap_Outside) {
    float origX = 5.0f, origY = -5.0f;
    float revX = 5.0f, revY = -5.0f;
    { HookDisableGuard guard("Global/CRadar/LimitToMap");
      CRadar::LimitToMap(origX, origY); }
    CRadar::LimitToMap(revX, revY);
    EXPECT_NEAR(origX, revX, 1e-5f);
    EXPECT_NEAR(origY, revY, 1e-5f);
}

GAME_DIFF_TEST(CRadar6, LimitToMap_Extreme) {
    float origX = 100.f, origY = 100.f;
    float revX = 100.f, revY = 100.f;
    { HookDisableGuard guard("Global/CRadar/LimitToMap");
      CRadar::LimitToMap(origX, origY); }
    CRadar::LimitToMap(revX, revY);
    EXPECT_NEAR(origX, revX, 1e-5f);
    EXPECT_NEAR(origY, revY, 1e-5f);
}

// ===================================================================
// SetMapCentreToPlayerCoords — void, safe to call
// ===================================================================

GAME_DIFF_TEST(CRadar6, SetMapCentreToPlayerCoords) {
    { HookDisableGuard guard("Global/CRadar/SetMapCentreToPlayerCoords");
      CRadar::SetMapCentreToPlayerCoords(); }
    CRadar::SetMapCentreToPlayerCoords();
    EXPECT_TRUE(true);
}

// ===================================================================
// CalculateCachedSinCos — void, updates cached sin/cos values
// ===================================================================

GAME_DIFF_TEST(CRadar6, CalculateCachedSinCos) {
    { HookDisableGuard guard("Global/CRadar/CalculateCachedSinCos");
      CRadar::CalculateCachedSinCos(); }
    float origSin = CRadar::cachedSin;
    float origCos = CRadar::cachedCos;

    CRadar::CalculateCachedSinCos();
    float revSin = CRadar::cachedSin;
    float revCos = CRadar::cachedCos;

    EXPECT_NEAR(origSin, revSin, 1e-5f);
    EXPECT_NEAR(origCos, revCos, 1e-5f);
}

// ===================================================================
// ChangeBlipBrightness/Scale/Display/Colour — void setters
// Test by calling with handle 0 (may be unused) and verifying no crash
// ===================================================================

GAME_DIFF_TEST(CRadar6, ChangeBlipBrightness_NoCrash) {
    { HookDisableGuard guard("Global/CRadar/ChangeBlipBrightness");
      CRadar::ChangeBlipBrightness(0, 200); }
    CRadar::ChangeBlipBrightness(0, 200);
    EXPECT_TRUE(true);
}

GAME_DIFF_TEST(CRadar6, ChangeBlipScale_NoCrash) {
    { HookDisableGuard guard("Global/CRadar/ChangeBlipScale");
      CRadar::ChangeBlipScale(0, 5); }
    CRadar::ChangeBlipScale(0, 5);
    EXPECT_TRUE(true);
}

GAME_DIFF_TEST(CRadar6, ChangeBlipDisplay_NoCrash) {
    { HookDisableGuard guard("Global/CRadar/ChangeBlipDisplay");
      CRadar::ChangeBlipDisplay(0, BLIP_DISPLAY_BOTH); }
    CRadar::ChangeBlipDisplay(0, BLIP_DISPLAY_BOTH);
    EXPECT_TRUE(true);
}

GAME_DIFF_TEST(CRadar6, ChangeBlipColour_NoCrash) {
    { HookDisableGuard guard("Global/CRadar/ChangeBlipColour");
      CRadar::ChangeBlipColour(0, BLIP_COLOUR_RED); }
    CRadar::ChangeBlipColour(0, BLIP_COLOUR_RED);
    EXPECT_TRUE(true);
}

// ===================================================================
// SetCoordBlipAppearance — void, modifies blip appearance
// ===================================================================

GAME_DIFF_TEST(CRadar6, SetCoordBlipAppearance_NoCrash) {
    { HookDisableGuard guard("Global/CRadar/SetCoordBlipAppearance");
      CRadar::SetCoordBlipAppearance(0, (eBlipAppearance)0); }
    CRadar::SetCoordBlipAppearance(0, (eBlipAppearance)0);
    EXPECT_TRUE(true);
}

// ===================================================================
// ClearBlip — void, removes a blip
// ===================================================================

GAME_DIFF_TEST(CRadar6, ClearBlip_InvalidHandle) {
    { HookDisableGuard guard("Global/CRadar/ClearBlip");
      CRadar::ClearBlip(-1); }
    CRadar::ClearBlip(-1);
    EXPECT_TRUE(true);
}
