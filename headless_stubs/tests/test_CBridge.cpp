// test_CBridge.cpp — Differential tests for CBridge.
// Hook paths: Global/CBridge/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "Bridge.h"

GAME_DIFF_TEST(CBridge, ShouldLightsBeFlashing) {
    bool orig, rev;
    { HookDisableGuard guard("Global/CBridge/ShouldLightsBeFlashing"); orig = CBridge::ShouldLightsBeFlashing(); }
    rev = CBridge::ShouldLightsBeFlashing();
    EXPECT_EQ(orig, rev);
}
