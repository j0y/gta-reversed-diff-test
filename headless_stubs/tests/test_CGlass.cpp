// test_CGlass.cpp — Differential tests for CGlass.
// Hook paths: Global/CGlass/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "Glass.h"

GAME_DIFF_TEST(CGlass, HasGlassBeenShatteredAtCoors) {
    CVector positions[] = {
        { 2488.0f, -1666.0f, 13.0f },   // LS downtown
        { 0.0f, 0.0f, 0.0f },            // origin
        { -2000.0f, 500.0f, 10.0f },     // SF area
        { 1000.0f, -1000.0f, 20.0f },    // mid-map
    };
    for (auto& pos : positions) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CGlass/HasGlassBeenShatteredAtCoors");
          orig = CGlass::HasGlassBeenShatteredAtCoors(pos); }
        rev = CGlass::HasGlassBeenShatteredAtCoors(pos);
        EXPECT_EQ(orig, rev);
    }
}
