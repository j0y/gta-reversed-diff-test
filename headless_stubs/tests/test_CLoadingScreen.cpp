// test_CLoadingScreen.cpp — Differential tests for CLoadingScreen.
// Hook paths: Global/CLoadingScreen/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "LoadingScreen.h"

// --- IsActive ---

GAME_DIFF_TEST(CLoadingScreen, IsActive) {
    bool orig, rev;
    { HookDisableGuard guard("Global/CLoadingScreen/IsActive");
      orig = CLoadingScreen::IsActive(); }
    rev = CLoadingScreen::IsActive();
    EXPECT_EQ(orig, rev);
}

// --- GetClockTime ---

GAME_DIFF_TEST(CLoadingScreen, GetClockTime_Default) {
    float orig, rev;
    { HookDisableGuard guard("Global/CLoadingScreen/GetClockTime");
      orig = CLoadingScreen::GetClockTime(true); }
    rev = CLoadingScreen::GetClockTime(true);
    EXPECT_NEAR(orig, rev, 1e-3f);
}

GAME_DIFF_TEST(CLoadingScreen, GetClockTime_WithPause) {
    float orig, rev;
    { HookDisableGuard guard("Global/CLoadingScreen/GetClockTime");
      orig = CLoadingScreen::GetClockTime(false); }
    rev = CLoadingScreen::GetClockTime(false);
    EXPECT_NEAR(orig, rev, 1e-3f);
}
