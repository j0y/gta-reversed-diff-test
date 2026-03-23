// test_CFont.cpp — Deep differential tests for CFont.
// Hook paths: Global/CFont/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "Font.h"

// --- GetCharacterSize ---

GAME_DIFF_TEST(CFont, GetCharacterSize_Letters) {
    for (uint8 ch = 'A'; ch <= 'Z'; ch++) {
        float orig, rev;
        { HookDisableGuard guard("Global/CFont/GetCharacterSize"); orig = CFont::GetCharacterSize(ch); }
        rev = CFont::GetCharacterSize(ch);
        EXPECT_NEAR(orig, rev, 1e-5f);
    }
}

GAME_DIFF_TEST(CFont, GetCharacterSize_Digits) {
    for (uint8 ch = '0'; ch <= '9'; ch++) {
        float orig, rev;
        { HookDisableGuard guard("Global/CFont/GetCharacterSize"); orig = CFont::GetCharacterSize(ch); }
        rev = CFont::GetCharacterSize(ch);
        EXPECT_NEAR(orig, rev, 1e-5f);
    }
}

GAME_DIFF_TEST(CFont, GetCharacterSize_Special) {
    uint8 chars[] = { ' ', '.', ',', '!', '?', ':', '-', '(', ')', 0 };
    for (int i = 0; chars[i] != 0; i++) {
        float orig, rev;
        { HookDisableGuard guard("Global/CFont/GetCharacterSize"); orig = CFont::GetCharacterSize(chars[i]); }
        rev = CFont::GetCharacterSize(chars[i]);
        EXPECT_NEAR(orig, rev, 1e-5f);
    }
}
