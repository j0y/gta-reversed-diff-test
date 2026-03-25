// test_CGlass3.cpp — Extended CGlass differential tests.
// Hook paths: Global/CGlass/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "Glass.h"

GAME_TEST(CGlass3, Init_NoCrash) {
    CGlass::Init();
    EXPECT_TRUE(true);
}

// Test HasGlassBeenShatteredAtCoors at many positions
GAME_DIFF_TEST(CGlass3, HasGlassBeenShatteredAtCoors_Grid) {
    CVector positions[] = {
        {2488.0f, -1666.0f, 13.5f},  // Grove Street
        {1500.0f, -1500.0f, 20.0f},
        {-2170.0f, 250.0f, 30.0f},   // SF
        {370.0f, -1840.0f, 5.0f},    // Santa Maria Beach
        {1000.0f, 1000.0f, 10.0f},
        {-500.0f, -500.0f, 15.0f},
    };
    for (auto& pos : positions) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CGlass/HasGlassBeenShatteredAtCoors");
          orig = CGlass::HasGlassBeenShatteredAtCoors(pos); }
        rev = CGlass::HasGlassBeenShatteredAtCoors(pos);
        EXPECT_EQ(orig, rev);
    }
}
