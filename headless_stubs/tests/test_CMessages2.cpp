// test_CMessages2.cpp — Deep differential tests for CMessages string functions.
// Hook paths: Global/CMessages/<fn>
//
// CMessages handles GxtChar (wide char) strings for game text display.
// String functions are pure and easy to diff-test.

#include "StdInc.h"
#include "TestFramework.h"
#include "Messages.h"

// --- GetStringLength ---

GAME_DIFF_TEST(CMessages, GetStringLength2_Empty) {
    GxtChar empty[] = { 0 };
    uint32 orig, rev;
    { HookDisableGuard guard("Global/CMessages/GetStringLength");
      orig = CMessages::GetStringLength(empty); }
    rev = CMessages::GetStringLength(empty);
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CMessages, GetStringLength2_Short) {
    GxtChar str[] = { 'H', 'E', 'L', 'L', 'O', 0 };
    uint32 orig, rev;
    { HookDisableGuard guard("Global/CMessages/GetStringLength");
      orig = CMessages::GetStringLength(str); }
    rev = CMessages::GetStringLength(str);
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CMessages, GetStringLength_Long) {
    GxtChar str[256];
    for (int i = 0; i < 255; i++) str[i] = 'A' + (i % 26);
    str[255] = 0;
    uint32 orig, rev;
    { HookDisableGuard guard("Global/CMessages/GetStringLength");
      orig = CMessages::GetStringLength(str); }
    rev = CMessages::GetStringLength(str);
    EXPECT_EQ(orig, rev);
}

// --- StringCompare ---

GAME_DIFF_TEST(CMessages, StringCompare2_Equal) {
    GxtChar a[] = { 'T', 'E', 'S', 'T', 0 };
    GxtChar b[] = { 'T', 'E', 'S', 'T', 0 };
    bool orig, rev;
    { HookDisableGuard guard("Global/CMessages/StringCompare");
      orig = CMessages::StringCompare(a, b, 4); }
    rev = CMessages::StringCompare(a, b, 4);
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CMessages, StringCompare2_NotEqual) {
    GxtChar a[] = { 'A', 'B', 'C', 0 };
    GxtChar b[] = { 'A', 'B', 'D', 0 };
    bool orig, rev;
    { HookDisableGuard guard("Global/CMessages/StringCompare");
      orig = CMessages::StringCompare(a, b, 3); }
    rev = CMessages::StringCompare(a, b, 3);
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CMessages, StringCompare_DifferentLengths) {
    GxtChar a[] = { 'A', 'B', 0 };
    GxtChar b[] = { 'A', 'B', 'C', 0 };
    // Compare only first 2 chars
    bool orig, rev;
    { HookDisableGuard guard("Global/CMessages/StringCompare");
      orig = CMessages::StringCompare(a, b, 2); }
    rev = CMessages::StringCompare(a, b, 2);
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CMessages, StringCompare_ZeroLength) {
    GxtChar a[] = { 'A', 0 };
    GxtChar b[] = { 'B', 0 };
    bool orig, rev;
    { HookDisableGuard guard("Global/CMessages/StringCompare");
      orig = CMessages::StringCompare(a, b, 0); }
    rev = CMessages::StringCompare(a, b, 0);
    EXPECT_EQ(orig, rev);
}

// --- StringCopy ---

GAME_DIFF_TEST(CMessages, StringCopy_Short) {
    GxtChar src[] = { 'H', 'I', 0 };
    GxtChar origDst[16] = {};
    GxtChar revDst[16] = {};
    { HookDisableGuard guard("Global/CMessages/StringCopy");
      CMessages::StringCopy(origDst, src, 16); }
    CMessages::StringCopy(revDst, src, 16);
    // Compare the copied strings
    for (int i = 0; i < 3; i++) {
        EXPECT_EQ(origDst[i], revDst[i]);
    }
}

GAME_DIFF_TEST(CMessages, StringCopy_Truncate) {
    GxtChar src[] = { 'A', 'B', 'C', 'D', 'E', 0 };
    GxtChar origDst[4] = {};
    GxtChar revDst[4] = {};
    { HookDisableGuard guard("Global/CMessages/StringCopy");
      CMessages::StringCopy(origDst, src, 3); }
    CMessages::StringCopy(revDst, src, 3);
    for (int i = 0; i < 3; i++) {
        EXPECT_EQ(origDst[i], revDst[i]);
    }
}
