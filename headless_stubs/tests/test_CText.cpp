// test_CText.cpp — CText differential tests.
// Hook path: Text/CText/Get
//
// Tests CText::Get with known GXT keys to verify the reversed
// implementation returns the same string pointers as the original.

#include "StdInc.h"
#include "TestFramework.h"
#include "Text/Text.h"

GAME_DIFF_TEST(CText, Get_FESZ_SA) {
    const GxtChar* orig;
    const GxtChar* rev;
    { HookDisableGuard guard("Text/CText/Get");
      orig = TheText.Get("FESZ_SA"); }
    rev = TheText.Get("FESZ_SA");
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CText, Get_IN_BET) {
    const GxtChar* orig;
    const GxtChar* rev;
    { HookDisableGuard guard("Text/CText/Get");
      orig = TheText.Get("IN_BET"); }
    rev = TheText.Get("IN_BET");
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CText, Get_DUMMY) {
    const GxtChar* orig;
    const GxtChar* rev;
    { HookDisableGuard guard("Text/CText/Get");
      orig = TheText.Get("DUMMY"); }
    rev = TheText.Get("DUMMY");
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CText, Get_MultipleKeys) {
    const char* keys[] = { "FESZ_SA", "IN_BET", "DUMMY", "FEM_ON", "FEM_OFF" };
    for (auto* key : keys) {
        const GxtChar* orig;
        const GxtChar* rev;
        { HookDisableGuard guard("Text/CText/Get");
          orig = TheText.Get(key); }
        rev = TheText.Get(key);
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(CText, Get_InvalidKey) {
    // A key that doesn't exist should return the GXT error string
    const GxtChar* orig;
    const GxtChar* rev;
    { HookDisableGuard guard("Text/CText/Get");
      orig = TheText.Get("ZZZZZZZZ"); }
    rev = TheText.Get("ZZZZZZZZ");
    EXPECT_EQ(orig, rev);
}
