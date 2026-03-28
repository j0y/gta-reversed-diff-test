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
// GetClockTime is private after upstream refactor (#1237).
// Call via direct function pointer at original address.
using GetClockTimeFn = float(__cdecl*)(bool);
static auto getClockTimeFn = reinterpret_cast<GetClockTimeFn>(0x590280);

GAME_DIFF_TEST(CLoadingScreen, GetClockTime_Default) {
    float orig, rev;
    { HookDisableGuard guard("Global/CLoadingScreen/GetClockTime");
      orig = getClockTimeFn(true); }
    rev = getClockTimeFn(true);
    EXPECT_NEAR(orig, rev, 1e-3f);
}

GAME_DIFF_TEST(CLoadingScreen, GetClockTime_WithPause) {
    float orig, rev;
    { HookDisableGuard guard("Global/CLoadingScreen/GetClockTime");
      orig = getClockTimeFn(false); }
    rev = getClockTimeFn(false);
    EXPECT_NEAR(orig, rev, 1e-3f);
}
