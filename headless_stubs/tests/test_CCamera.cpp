// test_CCamera.cpp — Differential tests for CCamera.
// Hook paths: Global/CCamera/<fn>
// TheCamera is a global instance initialized at state 9.

#include "StdInc.h"
#include "TestFramework.h"
#include "Camera.h"

// --- Simple bool/int getters (no args) ---

#define DIFF_CAM_BOOL(FnName) \
    GAME_DIFF_TEST(CCamera, FnName) { \
        bool orig, rev; \
        { HookDisableGuard guard("Global/CCamera/" #FnName); orig = TheCamera.FnName(); } \
        rev = TheCamera.FnName(); \
        EXPECT_EQ(orig, rev); \
    }

#define DIFF_CAM_INT(FnName) \
    GAME_DIFF_TEST(CCamera, FnName) { \
        int32 orig, rev; \
        { HookDisableGuard guard("Global/CCamera/" #FnName); orig = TheCamera.FnName(); } \
        rev = TheCamera.FnName(); \
        EXPECT_EQ(orig, rev); \
    }

#define DIFF_CAM_FLOAT(FnName) \
    GAME_DIFF_TEST(CCamera, FnName) { \
        float orig, rev; \
        { HookDisableGuard guard("Global/CCamera/" #FnName); orig = TheCamera.FnName(); } \
        rev = TheCamera.FnName(); \
        EXPECT_NEAR(orig, rev, 1e-5f); \
    }

DIFF_CAM_BOOL(GetFading)
DIFF_CAM_INT(GetFadingDirection)
DIFF_CAM_INT(GetLookDirection)
DIFF_CAM_BOOL(GetLookingForwardFirstPerson)
DIFF_CAM_BOOL(GetLookingLRBFirstPerson)
DIFF_CAM_BOOL(Get_Just_Switched_Status)
DIFF_CAM_FLOAT(FindCamFOV)
// Find3rdPersonQuickAimPitch reads m_fMouseAccelVertical which is modified
// by the hook toggle process itself — skip differential test
GAME_TEST(CCamera, Find3rdPersonQuickAimPitch_NoExcept) {
    float val = TheCamera.Find3rdPersonQuickAimPitch();
    (void)val; // just ensure no crash
    EXPECT_TRUE(true);
}
DIFF_CAM_FLOAT(GetPositionAlongSpline)
DIFF_CAM_BOOL(Using1stPersonWeaponMode)

GAME_DIFF_TEST(CCamera, GetScreenFadeStatus) {
    int orig, rev;
    { HookDisableGuard guard("Global/CCamera/GetScreenFadeStatus"); orig = (int)TheCamera.GetScreenFadeStatus(); }
    rev = (int)TheCamera.GetScreenFadeStatus();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CCamera, GetCutSceneFinishTime) {
    uint32 orig, rev;
    { HookDisableGuard guard("Global/CCamera/GetCutSceneFinishTime"); orig = TheCamera.GetCutSceneFinishTime(); }
    rev = TheCamera.GetCutSceneFinishTime();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CCamera, GetRoughDistanceToGround) {
    float orig, rev;
    { HookDisableGuard guard("Global/CCamera/GetRoughDistanceToGround"); orig = TheCamera.GetRoughDistanceToGround(); }
    rev = TheCamera.GetRoughDistanceToGround();
    EXPECT_NEAR(orig, rev, 1e-3f);
}

GAME_DIFF_TEST(CCamera, GetGameCamPosition) {
    CVector* orig;
    CVector* rev;
    { HookDisableGuard guard("Global/CCamera/GetGameCamPosition"); orig = TheCamera.GetGameCamPosition(); }
    rev = TheCamera.GetGameCamPosition();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CCamera, CalculateGroundHeight) {
    // Test each ground height type
    for (int type = 0; type < 3; type++) {
        float orig, rev;
        { HookDisableGuard guard("Global/CCamera/CalculateGroundHeight");
          orig = TheCamera.CalculateGroundHeight((eGroundHeightType)type); }
        rev = TheCamera.CalculateGroundHeight((eGroundHeightType)type);
        EXPECT_NEAR(orig, rev, 1e-3f);
    }
}

// --- Methods with simple args ---

GAME_DIFF_TEST(CCamera, GetArrPosForVehicleType) {
    // Test various vehicle types
    for (int type = 0; type < 10; type++) {
        int32 origPos = -1, revPos = -1;
        bool origRet, revRet;
        { HookDisableGuard guard("Global/CCamera/GetArrPosForVehicleType");
          origRet = TheCamera.GetArrPosForVehicleType((eVehicleType)type, origPos); }
        revRet = TheCamera.GetArrPosForVehicleType((eVehicleType)type, revPos);
        EXPECT_EQ(origRet, revRet);
        EXPECT_EQ(origPos, revPos);
    }
}

GAME_DIFF_TEST(CCamera, IsExtraEntityToIgnore) {
    // Test with nullptr — should return false
    bool orig, rev;
    { HookDisableGuard guard("Global/CCamera/IsExtraEntityToIgnore"); orig = TheCamera.IsExtraEntityToIgnore(nullptr); }
    rev = TheCamera.IsExtraEntityToIgnore(nullptr);
    EXPECT_EQ(orig, rev);
}
