// test_CMenuManager.cpp — CMenuManager differential tests.
// Hook paths: Global/CMenuManager/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "MenuManager.h"

// --- StretchX ---

GAME_DIFF_TEST(CMenuManager, StretchX_100) {
    float orig, rev;
    { HookDisableGuard guard("Global/CMenuManager/StretchX");
      orig = FrontEndMenuManager.StretchX(100.0f); }
    rev = FrontEndMenuManager.StretchX(100.0f);
    EXPECT_NEAR(orig, rev, 1e-3f);
}

GAME_DIFF_TEST(CMenuManager, StretchX_200) {
    float orig, rev;
    { HookDisableGuard guard("Global/CMenuManager/StretchX");
      orig = FrontEndMenuManager.StretchX(200.0f); }
    rev = FrontEndMenuManager.StretchX(200.0f);
    EXPECT_NEAR(orig, rev, 1e-3f);
}

GAME_DIFF_TEST(CMenuManager, StretchX_640) {
    float orig, rev;
    { HookDisableGuard guard("Global/CMenuManager/StretchX");
      orig = FrontEndMenuManager.StretchX(640.0f); }
    rev = FrontEndMenuManager.StretchX(640.0f);
    EXPECT_NEAR(orig, rev, 1e-3f);
}

// --- StretchY ---

GAME_DIFF_TEST(CMenuManager, StretchY_100) {
    float orig, rev;
    { HookDisableGuard guard("Global/CMenuManager/StretchY");
      orig = FrontEndMenuManager.StretchY(100.0f); }
    rev = FrontEndMenuManager.StretchY(100.0f);
    EXPECT_NEAR(orig, rev, 1e-3f);
}

GAME_DIFF_TEST(CMenuManager, StretchY_200) {
    float orig, rev;
    { HookDisableGuard guard("Global/CMenuManager/StretchY");
      orig = FrontEndMenuManager.StretchY(200.0f); }
    rev = FrontEndMenuManager.StretchY(200.0f);
    EXPECT_NEAR(orig, rev, 1e-3f);
}

GAME_DIFF_TEST(CMenuManager, StretchY_448) {
    float orig, rev;
    { HookDisableGuard guard("Global/CMenuManager/StretchY");
      orig = FrontEndMenuManager.StretchY(448.0f); }
    rev = FrontEndMenuManager.StretchY(448.0f);
    EXPECT_NEAR(orig, rev, 1e-3f);
}

// --- GetNumberOfMenuOptions ---

GAME_DIFF_TEST(CMenuManager, GetNumberOfMenuOptions) {
    uint32 orig, rev;
    { HookDisableGuard guard("Global/CMenuManager/GetNumberOfMenuOptions");
      orig = FrontEndMenuManager.GetNumberOfMenuOptions(); }
    rev = FrontEndMenuManager.GetNumberOfMenuOptions();
    EXPECT_EQ(orig, rev);
}
