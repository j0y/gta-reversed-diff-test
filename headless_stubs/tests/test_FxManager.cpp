// test_FxManager.cpp — Differential tests for FxManager_c.
// Hook paths: Fx/FxManager_c/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "Fx/FxManager.h"

// --- FindFxSystemBP ---

GAME_DIFF_TEST(FxManager, FindFxSystemBP_Blood) {
    FxSystemBP_c* orig;
    FxSystemBP_c* rev;
    { HookDisableGuard guard("Fx/FxManager_c/FindFxSystemBP");
      orig = g_fxMan.FindFxSystemBP("blood_heli"); }
    rev = g_fxMan.FindFxSystemBP("blood_heli");
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(FxManager, FindFxSystemBP_Gunflash) {
    FxSystemBP_c* orig;
    FxSystemBP_c* rev;
    { HookDisableGuard guard("Fx/FxManager_c/FindFxSystemBP");
      orig = g_fxMan.FindFxSystemBP("gunflash"); }
    rev = g_fxMan.FindFxSystemBP("gunflash");
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(FxManager, FindFxSystemBP_Unknown) {
    FxSystemBP_c* orig;
    FxSystemBP_c* rev;
    { HookDisableGuard guard("Fx/FxManager_c/FindFxSystemBP");
      orig = g_fxMan.FindFxSystemBP("nonexistent_fx_name"); }
    rev = g_fxMan.FindFxSystemBP("nonexistent_fx_name");
    EXPECT_EQ(orig, rev);
}

// --- GetFrustumInfo ---

GAME_DIFF_TEST(FxManager, GetFrustumInfo) {
    FxFrustumInfo_c* orig;
    FxFrustumInfo_c* rev;
    { HookDisableGuard guard("Fx/FxManager_c/GetFrustumInfo");
      orig = g_fxMan.GetFrustumInfo(); }
    rev = g_fxMan.GetFrustumInfo();
    EXPECT_EQ(orig, rev);
}
