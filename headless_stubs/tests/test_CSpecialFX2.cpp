// test_CSpecialFX2.cpp — Behavior and differential tests for CSpecialFX.
// Hook paths: Global/CSpecialFX/<fn>  (RH_ScopedCategoryGlobal)

#include "StdInc.h"
#include "TestFramework.h"

// --- ReplayStarted: resets internal state ---

GAME_DIFF_TEST(CSpecialFX, ReplayStarted) {
    {
        HookDisableGuard guard("Global/CSpecialFX/ReplayStarted");
        CSpecialFX::ReplayStarted();
    }
    auto origVideoCam = CSpecialFX::bVideoCam;
    auto origLiftCam  = CSpecialFX::bLiftCam;

    CSpecialFX::ReplayStarted();
    auto revVideoCam = CSpecialFX::bVideoCam;
    auto revLiftCam  = CSpecialFX::bLiftCam;

    EXPECT_EQ(origVideoCam, revVideoCam);
    EXPECT_EQ(origLiftCam, revLiftCam);
}

// --- Init: initializes textures and internal state, behavior test ---

GAME_TEST(CSpecialFX, Init_NoCrash) {
    CSpecialFX::Init();
    EXPECT_TRUE(true);
}

// --- Static member defaults after ReplayStarted ---

GAME_TEST(CSpecialFX, ReplayStarted_ClearsFlags) {
    // Set flags to non-default
    CSpecialFX::bVideoCam = true;
    CSpecialFX::bLiftCam  = true;

    CSpecialFX::ReplayStarted();

    EXPECT_EQ(CSpecialFX::bVideoCam, false);
    EXPECT_EQ(CSpecialFX::bLiftCam, false);
}

// --- Shutdown: safe to call, behavior test ---

GAME_TEST(CSpecialFX, Shutdown_NoCrash) {
    CSpecialFX::Shutdown();
    EXPECT_TRUE(true);
    // Re-init so other tests aren't affected
    CSpecialFX::Init();
}

// --- Update: requires render context, behavior test ---

GAME_TEST(CSpecialFX, Update_NoCrash) {
    CSpecialFX::Update();
    EXPECT_TRUE(true);
}
