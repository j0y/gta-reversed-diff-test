// test_Common.cpp — Differential tests for Common namespace free functions.
// Hook paths: Common/Common/<fn>
//
// MakeUpperCase, AsciiToGxtChar, IsPointInsideLine — pure string/math.

#include "StdInc.h"
#include "TestFramework.h"
#include "common.h"
#include "GxtChar.h"

// --- MakeUpperCase (two-arg: dest, src) ---

GAME_DIFF_TEST(Common, MakeUpperCase_Copy) {
    const char* inputs[] = {
        "hello", "Hello World", "ALREADY", "mixedCase123",
        "special!@#", "", "a", "abcdefghijklmnopqrstuvwxyz",
    };
    for (const char* input : inputs) {
        char origBuf[64] = {}, revBuf[64] = {};
        { HookDisableGuard guard("Common/Common/MakeUpperCase-");
          MakeUpperCase(origBuf, input); }
        MakeUpperCase(revBuf, input);
        EXPECT_EQ(strcmp(origBuf, revBuf), 0);
    }
}

// --- MakeUpperCase (in-place) ---

GAME_DIFF_TEST(Common, MakeUpperCase_InPlace) {
    const char* inputs[] = { "hello", "Hello World", "ALREADY", "abc123xyz" };
    for (const char* input : inputs) {
        char origBuf[64], revBuf[64];
        strcpy(origBuf, input);
        strcpy(revBuf, input);
        { HookDisableGuard guard("Common/Common/MakeUpperCase-in");
          MakeUpperCase(origBuf); }
        MakeUpperCase(revBuf);
        EXPECT_EQ(strcmp(origBuf, revBuf), 0);
    }
}

// --- AsciiToGxtChar ---

GAME_DIFF_TEST(Common, AsciiToGxtChar) {
    const char* inputs[] = { "Hello", "TEST123", "Special!@#", "", "A" };
    for (const char* input : inputs) {
        GxtChar origBuf[64] = {}, revBuf[64] = {};
        { HookDisableGuard guard("Common/Common/AsciiToGxtChar");
          AsciiToGxtChar(input, origBuf); }
        AsciiToGxtChar(input, revBuf);
        // Compare as raw bytes (GxtChar is uint16)
        int len = (int)strlen(input) + 1;
        for (int i = 0; i < len; i++) {
            EXPECT_EQ(origBuf[i], revBuf[i]);
        }
    }
}

// --- IsPointInsideLine ---

GAME_DIFF_TEST(Common, IsPointInsideLine) {
    struct Case { CVector2D origin, dir, pt; float radius; };
    Case cases[] = {
        // Point on right side of line
        { {0,0}, {1,0}, {0,5}, 0.0f },
        // Point on left side
        { {0,0}, {1,0}, {0,-5}, 0.0f },
        // Point exactly on line
        { {0,0}, {1,0}, {5,0}, 0.0f },
        // With radius
        { {0,0}, {1,0}, {0,3}, 5.0f },
        { {0,0}, {1,0}, {0,3}, 2.0f },
        // Diagonal line
        { {0,0}, {1,1}, {5,0}, 0.0f },
        { {0,0}, {1,1}, {0,5}, 0.0f },
        // Various radii
        { {10,10}, {0,1}, {15,10}, -3.0f },
    };
    for (auto& c : cases) {
        bool orig, rev;
        { HookDisableGuard guard("Common/Common/IsPointInsideLine");
          orig = IsPointInsideLine(c.origin, c.dir, c.pt, c.radius); }
        rev = IsPointInsideLine(c.origin, c.dir, c.pt, c.radius);
        EXPECT_EQ(orig, rev);
    }
}
