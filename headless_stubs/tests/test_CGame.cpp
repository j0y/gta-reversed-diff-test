// test_CGame.cpp — Differential tests for CGame.
// Hook paths: Global/CGame/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "Game.h"

GAME_DIFF_TEST(CGame, CanSeeOutSideFromCurrArea) {
    bool orig, rev;
    { HookDisableGuard guard("Global/CGame/CanSeeOutSideFromCurrArea");
      orig = CGame::CanSeeOutSideFromCurrArea(); }
    rev = CGame::CanSeeOutSideFromCurrArea();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CGame, CanSeeWaterFromCurrArea) {
    bool orig, rev;
    { HookDisableGuard guard("Global/CGame/CanSeeWaterFromCurrArea");
      orig = CGame::CanSeeWaterFromCurrArea(); }
    rev = CGame::CanSeeWaterFromCurrArea();
    EXPECT_EQ(orig, rev);
}
