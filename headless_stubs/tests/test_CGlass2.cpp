// test_CGlass2.cpp — Deep differential tests for CGlass.
// Hook paths: Global/CGlass/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "Glass.h"

// --- HasGlassBeenShatteredAtCoors ---

GAME_DIFF_TEST(CGlass2, HasGlassBeenShatteredAtCoors_Various) {
    CVector positions[] = {
        CVector(2495.0f, -1665.0f, 13.5f),
        CVector(0.0f, 0.0f, 10.0f),
        CVector(1000.0f, -1000.0f, 20.0f),
    };
    for (auto& pos : positions) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CGlass/HasGlassBeenShatteredAtCoors"); orig = CGlass::HasGlassBeenShatteredAtCoors(pos); }
        rev = CGlass::HasGlassBeenShatteredAtCoors(pos);
        EXPECT_EQ(orig, rev);
    }
}
