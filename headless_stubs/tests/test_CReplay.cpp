// test_CReplay.cpp — Differential tests for CReplay.
// Hook paths: Global/CReplay/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "Replay.h"

GAME_DIFF_TEST(CReplay, ShouldStandardCameraBeProcessed) {
    bool orig, rev;
    { HookDisableGuard guard("Global/CReplay/ShouldStandardCameraBeProcessed"); orig = CReplay::ShouldStandardCameraBeProcessed(); }
    rev = CReplay::ShouldStandardCameraBeProcessed();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CReplay, NumberFramesAvailableToPlay) {
    uint32 orig, rev;
    { HookDisableGuard guard("Global/CReplay/NumberFramesAvailableToPlay"); orig = CReplay::NumberFramesAvailableToPlay(); }
    rev = CReplay::NumberFramesAvailableToPlay();
    EXPECT_EQ(orig, rev);
}
