// test_CCutsceneMgr.cpp — Differential tests for CCutsceneMgr.
// Hook paths: Global/CCutsceneMgr/<fn>
// At state 9 no cutscene is active — test default state queries.

#include "StdInc.h"
#include "TestFramework.h"
#include "CutsceneMgr.h"

GAME_DIFF_TEST(CCutsceneMgr, HasCutsceneFinished) {
    bool orig, rev;
    { HookDisableGuard guard("Global/CCutsceneMgr/HasCutsceneFinished");
      orig = CCutsceneMgr::HasCutsceneFinished(); }
    rev = CCutsceneMgr::HasCutsceneFinished();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CCutsceneMgr, IsCutsceneSkipButtonBeingPressed) {
    bool orig, rev;
    { HookDisableGuard guard("Global/CCutsceneMgr/IsCutsceneSkipButtonBeingPressed");
      orig = CCutsceneMgr::IsCutsceneSkipButtonBeingPressed(); }
    rev = CCutsceneMgr::IsCutsceneSkipButtonBeingPressed();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CCutsceneMgr, GetCutsceneTimeInMilleseconds) {
    uint32 orig, rev;
    { HookDisableGuard guard("Global/CCutsceneMgr/GetCutsceneTimeInMilleseconds");
      orig = CCutsceneMgr::GetCutsceneTimeInMilleseconds(); }
    rev = CCutsceneMgr::GetCutsceneTimeInMilleseconds();
    EXPECT_EQ(orig, rev);
}
