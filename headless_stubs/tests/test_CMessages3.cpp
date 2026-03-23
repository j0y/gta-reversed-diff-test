// test_CMessages3.cpp — Deep differential tests for CMessages string manipulation.
// Hook paths: Global/CMessages/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "Messages.h"

// --- InsertNumberInString ---
// Replaces ~1~ markers in GxtChar strings with numbers.

GAME_DIFF_TEST(CMessages3, InsertNumberInString_OneNumber) {
    GxtChar src[] = { 'S', 'C', 'O', 'R', 'E', ':', ' ', '~', '1', '~', 0 };
    GxtChar origDst[64] = {};
    GxtChar revDst[64] = {};

    { HookDisableGuard guard("Global/CMessages/InsertNumberInString");
      CMessages::InsertNumberInString(src, 42, -1, -1, -1, -1, -1, origDst); }
    CMessages::InsertNumberInString(src, 42, -1, -1, -1, -1, -1, revDst);

    // Compare output strings
    bool match = true;
    for (int i = 0; i < 64; i++) {
        if (origDst[i] != revDst[i]) { match = false; break; }
        if (origDst[i] == 0) break;
    }
    EXPECT_TRUE(match);
}

GAME_DIFF_TEST(CMessages3, InsertNumberInString_MultipleNumbers) {
    GxtChar src[] = { '~', '1', '~', '/', '~', '1', '~', 0 };
    GxtChar origDst[64] = {};
    GxtChar revDst[64] = {};

    { HookDisableGuard guard("Global/CMessages/InsertNumberInString");
      CMessages::InsertNumberInString(src, 100, 200, -1, -1, -1, -1, origDst); }
    CMessages::InsertNumberInString(src, 100, 200, -1, -1, -1, -1, revDst);

    bool match = true;
    for (int i = 0; i < 64; i++) {
        if (origDst[i] != revDst[i]) { match = false; break; }
        if (origDst[i] == 0) break;
    }
    EXPECT_TRUE(match);
}

GAME_DIFF_TEST(CMessages3, InsertNumberInString_NoMarkers) {
    GxtChar src[] = { 'H', 'E', 'L', 'L', 'O', 0 };
    GxtChar origDst[64] = {};
    GxtChar revDst[64] = {};

    { HookDisableGuard guard("Global/CMessages/InsertNumberInString");
      CMessages::InsertNumberInString(src, 42, -1, -1, -1, -1, -1, origDst); }
    CMessages::InsertNumberInString(src, 42, -1, -1, -1, -1, -1, revDst);

    bool match = true;
    for (int i = 0; i < 64; i++) {
        if (origDst[i] != revDst[i]) { match = false; break; }
        if (origDst[i] == 0) break;
    }
    EXPECT_TRUE(match);
}

GAME_DIFF_TEST(CMessages3, InsertNumberInString_LargeNumbers) {
    GxtChar src[] = { '~', '1', '~', 0 };
    int32 numbers[] = { 0, 1, -1, 999999, -999999, 2147483647 };
    for (int32 n : numbers) {
        GxtChar origDst[64] = {};
        GxtChar revDst[64] = {};
        { HookDisableGuard guard("Global/CMessages/InsertNumberInString");
          CMessages::InsertNumberInString(src, n, -1, -1, -1, -1, -1, origDst); }
        CMessages::InsertNumberInString(src, n, -1, -1, -1, -1, -1, revDst);
        bool match = true;
        for (int i = 0; i < 64; i++) {
            if (origDst[i] != revDst[i]) { match = false; break; }
            if (origDst[i] == 0) break;
        }
        EXPECT_TRUE(match);
    }
}

// --- CutString ---

GAME_DIFF_TEST(CMessages3, CutString_Simple) {
    GxtChar str[] = { 'A', 'B', '~', 'n', '~', 'C', 'D', 0 };
    GxtChar* origParts[4] = {};
    GxtChar* revParts[4] = {};

    { HookDisableGuard guard("Global/CMessages/CutString");
      CMessages::CutString(2, str, origParts); }
    CMessages::CutString(2, str, revParts);

    // Both should split at the same positions
    for (int i = 0; i < 2; i++) {
        EXPECT_EQ((uintptr_t)origParts[i], (uintptr_t)revParts[i]);
    }
}
