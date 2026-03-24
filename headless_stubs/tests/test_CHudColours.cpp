// test_CHudColours.cpp — Differential tests for CHudColours.
// Hook paths: Global/CHudColours/<fn>
//
// Tests HUD colour lookups across the full colour index range.
// GetRGB/GetRGBA return CRGBA (4 bytes) — fits in register, no sret issue.

#include "StdInc.h"
#include "TestFramework.h"
#include "HudColours.h"

// --- GetIntColour across full range ---

GAME_DIFF_TEST(CHudColours, GetIntColour_AllIndices) {
    for (int32 i = 0; i < 20; i++) {
        uint32 orig, rev;
        { HookDisableGuard guard("Global/CHudColours/GetIntColour");
          orig = HudColour.GetIntColour(static_cast<eHudColours>(i)); }
        rev = HudColour.GetIntColour(static_cast<eHudColours>(i));
        EXPECT_EQ(orig, rev);
    }
}

// --- GetRGB across full range ---

GAME_DIFF_TEST(CHudColours, GetRGB_AllIndices) {
    for (int32 i = 0; i < 20; i++) {
        CRGBA orig, rev;
        { HookDisableGuard guard("Global/CHudColours/GetRGB");
          orig = HudColour.GetRGB(static_cast<eHudColours>(i)); }
        rev = HudColour.GetRGB(static_cast<eHudColours>(i));
        EXPECT_EQ(orig.r, rev.r);
        EXPECT_EQ(orig.g, rev.g);
        EXPECT_EQ(orig.b, rev.b);
    }
}

// --- GetRGBA with different alpha values ---

GAME_DIFF_TEST(CHudColours, GetRGBA_WithAlpha) {
    uint8 alphas[] = { 0, 64, 128, 192, 255 };
    for (int32 i = 0; i < 20; i++) {
        for (uint8 alpha : alphas) {
            CRGBA orig, rev;
            { HookDisableGuard guard("Global/CHudColours/GetRGBA");
              orig = HudColour.GetRGBA(static_cast<eHudColours>(i), alpha); }
            rev = HudColour.GetRGBA(static_cast<eHudColours>(i), alpha);
            EXPECT_EQ(orig.r, rev.r);
            EXPECT_EQ(orig.g, rev.g);
            EXPECT_EQ(orig.b, rev.b);
            EXPECT_EQ(orig.a, rev.a);
        }
    }
}
