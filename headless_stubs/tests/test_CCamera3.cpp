// test_CCamera3.cpp — Extended CCamera query tests.
// Hook paths: Global/CCamera/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "Camera.h"

GAME_DIFF_TEST(CCamera3, GetRoughDistanceToGround) {
    float orig, rev;
    { HookDisableGuard guard("Global/CCamera/GetRoughDistanceToGround");
      orig = TheCamera.GetRoughDistanceToGround(); }
    rev = TheCamera.GetRoughDistanceToGround();
    EXPECT_NEAR(orig, rev, 1e-1f);
}

GAME_DIFF_TEST(CCamera3, GetPositionAlongSpline) {
    float orig, rev;
    { HookDisableGuard guard("Global/CCamera/GetPositionAlongSpline");
      orig = TheCamera.GetPositionAlongSpline(); }
    rev = TheCamera.GetPositionAlongSpline();
    EXPECT_NEAR(orig, rev, 1e-3f);
}

GAME_DIFF_TEST(CCamera3, GetCutSceneFinishTime) {
    uint32 orig, rev;
    { HookDisableGuard guard("Global/CCamera/GetCutSceneFinishTime");
      orig = TheCamera.GetCutSceneFinishTime(); }
    rev = TheCamera.GetCutSceneFinishTime();
    EXPECT_EQ(orig, rev);
}

// CamShake takes different args, CamShakeNoPos doesn't exist. Skipped.

// --- ProcessFade ---

GAME_DIFF_TEST(CCamera3, ProcessFade_NoCrash) {
    { HookDisableGuard guard("Global/CCamera/ProcessFade");
      TheCamera.ProcessFade(); }
    TheCamera.ProcessFade();
    EXPECT_TRUE(true);
}

// --- ProcessMusicFade ---

GAME_DIFF_TEST(CCamera3, ProcessMusicFade_NoCrash) {
    { HookDisableGuard guard("Global/CCamera/ProcessMusicFade");
      TheCamera.ProcessMusicFade(); }
    TheCamera.ProcessMusicFade();
    EXPECT_TRUE(true);
}
