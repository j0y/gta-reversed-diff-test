// test_CGame2.cpp — Deep differential tests for CGame.
// Hook paths: Global/CGame/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "Game.h"

GAME_DIFF_TEST(CGame2, CanSeeOutSideFromCurrArea) {
    bool orig, rev;
    { HookDisableGuard guard("Global/CGame/CanSeeOutSideFromCurrArea"); orig = CGame::CanSeeOutSideFromCurrArea(); }
    rev = CGame::CanSeeOutSideFromCurrArea();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CGame2, CanSeeWaterFromCurrArea) {
    bool orig, rev;
    { HookDisableGuard guard("Global/CGame/CanSeeWaterFromCurrArea"); orig = CGame::CanSeeWaterFromCurrArea(); }
    rev = CGame::CanSeeWaterFromCurrArea();
    EXPECT_EQ(orig, rev);
}
