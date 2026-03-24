// test_CCamera2.cpp — Extended differential tests for CCamera.
// Hook paths: Global/CCamera/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "Camera.h"

// --- GetFading ---

GAME_DIFF_TEST(CCamera2, GetFading) {
    bool orig, rev;
    { HookDisableGuard guard("Global/CCamera/GetFading");
      orig = TheCamera.GetFading(); }
    rev = TheCamera.GetFading();
    EXPECT_EQ(orig, rev);
}

// --- GetFadingDirection ---

GAME_DIFF_TEST(CCamera2, GetFadingDirection) {
    int32 orig, rev;
    { HookDisableGuard guard("Global/CCamera/GetFadingDirection");
      orig = TheCamera.GetFadingDirection(); }
    rev = TheCamera.GetFadingDirection();
    EXPECT_EQ(orig, rev);
}

// --- GetScreenFadeStatus ---

GAME_DIFF_TEST(CCamera2, GetScreenFadeStatus) {
    int32 orig, rev;
    { HookDisableGuard guard("Global/CCamera/GetScreenFadeStatus");
      orig = (int32)TheCamera.GetScreenFadeStatus(); }
    rev = (int32)TheCamera.GetScreenFadeStatus();
    EXPECT_EQ(orig, rev);
}

// --- GetLookDirection ---

GAME_DIFF_TEST(CCamera2, GetLookDirection) {
    int32 orig, rev;
    { HookDisableGuard guard("Global/CCamera/GetLookDirection");
      orig = TheCamera.GetLookDirection(); }
    rev = TheCamera.GetLookDirection();
    EXPECT_EQ(orig, rev);
}

// --- GetLookingForwardFirstPerson ---

GAME_DIFF_TEST(CCamera2, GetLookingForwardFirstPerson) {
    bool orig, rev;
    { HookDisableGuard guard("Global/CCamera/GetLookingForwardFirstPerson");
      orig = TheCamera.GetLookingForwardFirstPerson(); }
    rev = TheCamera.GetLookingForwardFirstPerson();
    EXPECT_EQ(orig, rev);
}

// --- GetLookingLRBFirstPerson ---

GAME_DIFF_TEST(CCamera2, GetLookingLRBFirstPerson) {
    bool orig, rev;
    { HookDisableGuard guard("Global/CCamera/GetLookingLRBFirstPerson");
      orig = TheCamera.GetLookingLRBFirstPerson(); }
    rev = TheCamera.GetLookingLRBFirstPerson();
    EXPECT_EQ(orig, rev);
}

// --- Get_Just_Switched_Status ---

GAME_DIFF_TEST(CCamera2, Get_Just_Switched_Status) {
    bool orig, rev;
    { HookDisableGuard guard("Global/CCamera/Get_Just_Switched_Status");
      orig = TheCamera.Get_Just_Switched_Status(); }
    rev = TheCamera.Get_Just_Switched_Status();
    EXPECT_EQ(orig, rev);
}

// --- FindCamFOV ---

GAME_DIFF_TEST(CCamera2, FindCamFOV) {
    float orig, rev;
    { HookDisableGuard guard("Global/CCamera/FindCamFOV");
      orig = TheCamera.FindCamFOV(); }
    rev = TheCamera.FindCamFOV();
    EXPECT_NEAR(orig, rev, 1e-3f);
}

// CalculateGroundHeight takes eGroundHeightType enum — skipped (need exact enum values)

// --- IsExtraEntityToIgnore ---

GAME_DIFF_TEST(CCamera2, IsExtraEntityToIgnore_Null) {
    bool orig, rev;
    { HookDisableGuard guard("Global/CCamera/IsExtraEntityToIgnore");
      orig = TheCamera.IsExtraEntityToIgnore(nullptr); }
    rev = TheCamera.IsExtraEntityToIgnore(nullptr);
    EXPECT_EQ(orig, rev);
}
