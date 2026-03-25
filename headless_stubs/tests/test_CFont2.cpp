// test_CFont2.cpp — Differential tests for CFont.
// Hook paths: Global/CFont/<fn>
// Tests font metric queries — GetStringWidth is a pure measurement function.

#include "StdInc.h"
#include "TestFramework.h"
#include "Font.h"

// GetStringWidth measures the pixel width of a GXT string.
// It depends on font state (scale, style) so we set known state first.

GAME_DIFF_TEST(CFont2, GetStringWidth_EmptyString) {
    // Setup font state
    CFont::SetFontStyle(FONT_SUBTITLES);
    CFont::SetScale(1.0f, 1.0f);

    // Empty GXT string (single null terminator)
    GxtChar empty[2] = { 0, 0 };
    float orig, rev;
    { HookDisableGuard guard("Global/CFont/GetStringWidth"); orig = CFont::GetStringWidth(empty, true, false); }
    rev = CFont::GetStringWidth(empty, true, false);
    EXPECT_NEAR(orig, rev, 1e-3f);
}

GAME_DIFF_TEST(CFont2, GetStringWidth_SimpleText) {
    CFont::SetFontStyle(FONT_SUBTITLES);
    CFont::SetScale(1.0f, 1.0f);

    // "AB" in GXT encoding (ASCII letters map directly)
    GxtChar text[] = { 'A', 'B', 0 };
    float orig, rev;
    { HookDisableGuard guard("Global/CFont/GetStringWidth"); orig = CFont::GetStringWidth(text, true, false); }
    rev = CFont::GetStringWidth(text, true, false);
    EXPECT_NEAR(orig, rev, 1e-3f);
}

GAME_DIFF_TEST(CFont2, GetStringWidth_DifferentScales) {
    CFont::SetFontStyle(FONT_MENU);
    GxtChar text[] = { 'T', 'e', 's', 't', 0 };

    float scales[] = { 0.5f, 1.0f, 2.0f };
    for (float s : scales) {
        CFont::SetScale(s, s);
        float orig, rev;
        { HookDisableGuard guard("Global/CFont/GetStringWidth"); orig = CFont::GetStringWidth(text, true, false); }
        rev = CFont::GetStringWidth(text, true, false);
        EXPECT_NEAR(orig, rev, 1e-3f);
    }
}

GAME_DIFF_TEST(CFont2, GetStringWidth_AllFontStyles) {
    GxtChar text[] = { 'X', 'Y', 'Z', 0 };
    CFont::SetScale(1.0f, 1.0f);

    for (int style = 0; style < 3; style++) {
        CFont::SetFontStyle((eFontStyle)style);
        float orig, rev;
        { HookDisableGuard guard("Global/CFont/GetStringWidth"); orig = CFont::GetStringWidth(text, true, false); }
        rev = CFont::GetStringWidth(text, true, false);
        EXPECT_NEAR(orig, rev, 1e-3f);
    }
}
