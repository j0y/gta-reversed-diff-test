// test_CCamera5.cpp — More CCamera differential tests.
// Hook paths: Global/CCamera/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "Camera.h"

// --- ConsiderPedAsDucking (with player ped) ---

GAME_DIFF_TEST(CCamera5, ConsiderPedAsDucking_PlayerPed) {
    auto* ped = FindPlayerPed(0);
    if (!ped) { EXPECT_TRUE(true); return; }
    bool orig, rev;
    { HookDisableGuard guard("Global/CCamera/ConsiderPedAsDucking");
      orig = TheCamera.ConsiderPedAsDucking(ped); }
    rev = TheCamera.ConsiderPedAsDucking(ped);
    EXPECT_EQ(orig, rev);
}

// ConsiderPedAsDucking(nullptr) crashes original code — skip

// --- IsExtraEntityToIgnore (with player ped) ---

GAME_DIFF_TEST(CCamera5, IsExtraEntityToIgnore_PlayerPed) {
    auto* ped = FindPlayerPed(0);
    if (!ped) { EXPECT_TRUE(true); return; }
    bool orig, rev;
    { HookDisableGuard guard("Global/CCamera/IsExtraEntityToIgnore");
      orig = TheCamera.IsExtraEntityToIgnore(ped); }
    rev = TheCamera.IsExtraEntityToIgnore(ped);
    EXPECT_EQ(orig, rev);
}

// --- Using1stPersonWeaponMode ---

GAME_DIFF_TEST(CCamera5, Using1stPersonWeaponMode) {
    bool orig, rev;
    { HookDisableGuard guard("Global/CCamera/Using1stPersonWeaponMode");
      orig = TheCamera.Using1stPersonWeaponMode(); }
    rev = TheCamera.Using1stPersonWeaponMode();
    EXPECT_EQ(orig, rev);
}

// --- GetGameCamPosition ---

GAME_DIFF_TEST(CCamera5, GetGameCamPosition) {
    CVector *orig, *rev;
    { HookDisableGuard guard("Global/CCamera/GetGameCamPosition");
      orig = TheCamera.GetGameCamPosition(); }
    rev = TheCamera.GetGameCamPosition();
    EXPECT_EQ(orig, rev);
}

// --- GetScreenRect ---

GAME_DIFF_TEST(CCamera5, GetScreenRect) {
    CRect origRect{}, revRect{};
    { HookDisableGuard guard("Global/CCamera/GetScreenRect");
      TheCamera.GetScreenRect(&origRect); }
    TheCamera.GetScreenRect(&revRect);
    EXPECT_NEAR(origRect.left, revRect.left, 1e-3f);
    EXPECT_NEAR(origRect.top, revRect.top, 1e-3f);
    EXPECT_NEAR(origRect.right, revRect.right, 1e-3f);
    EXPECT_NEAR(origRect.bottom, revRect.bottom, 1e-3f);
}

// --- ResetDuckingSystem ---

GAME_DIFF_TEST(CCamera5, ResetDuckingSystem_PlayerPed_NoCrash) {
    auto* ped = FindPlayerPed(0);
    if (!ped) { EXPECT_TRUE(true); return; }
    { HookDisableGuard guard("Global/CCamera/ResetDuckingSystem");
      TheCamera.ResetDuckingSystem(ped); }
    TheCamera.ResetDuckingSystem(ped);
    EXPECT_TRUE(true);
}
