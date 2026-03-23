// test_CTimeCycle2.cpp — Deep differential tests for CTimeCycle.
// Hook paths: Global/CTimeCycle/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "TimeCycle.h"

// --- Ambient color getters ---

GAME_DIFF_TEST(CTimeCycle2, GetAmbientRed) {
    int32 orig, rev;
    { HookDisableGuard guard("Global/CTimeCycle/GetAmbientRed"); orig = CTimeCycle::GetAmbientRed(); }
    rev = CTimeCycle::GetAmbientRed();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CTimeCycle2, GetAmbientGreen) {
    int32 orig, rev;
    { HookDisableGuard guard("Global/CTimeCycle/GetAmbientGreen"); orig = CTimeCycle::GetAmbientGreen(); }
    rev = CTimeCycle::GetAmbientGreen();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CTimeCycle2, GetAmbientBlue) {
    int32 orig, rev;
    { HookDisableGuard guard("Global/CTimeCycle/GetAmbientBlue"); orig = CTimeCycle::GetAmbientBlue(); }
    rev = CTimeCycle::GetAmbientBlue();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CTimeCycle2, GetAmbientRed_BeforeBrightness) {
    int32 orig, rev;
    { HookDisableGuard guard("Global/CTimeCycle/GetAmbientRed_BeforeBrightness"); orig = CTimeCycle::GetAmbientRed_BeforeBrightness(); }
    rev = CTimeCycle::GetAmbientRed_BeforeBrightness();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CTimeCycle2, GetAmbientGreen_BeforeBrightness) {
    int32 orig, rev;
    { HookDisableGuard guard("Global/CTimeCycle/GetAmbientGreen_BeforeBrightness"); orig = CTimeCycle::GetAmbientGreen_BeforeBrightness(); }
    rev = CTimeCycle::GetAmbientGreen_BeforeBrightness();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CTimeCycle2, GetAmbientBlue_BeforeBrightness) {
    int32 orig, rev;
    { HookDisableGuard guard("Global/CTimeCycle/GetAmbientBlue_BeforeBrightness"); orig = CTimeCycle::GetAmbientBlue_BeforeBrightness(); }
    rev = CTimeCycle::GetAmbientBlue_BeforeBrightness();
    EXPECT_EQ(orig, rev);
}

// --- FindFarClipForCoors ---

GAME_DIFF_TEST(CTimeCycle2, FindFarClipForCoors) {
    float orig, rev;
    CVector pos(2495.0f, -1665.0f, 13.5f);
    { HookDisableGuard guard("Global/CTimeCycle/FindFarClipForCoors"); orig = CTimeCycle::FindFarClipForCoors(pos); }
    rev = CTimeCycle::FindFarClipForCoors(pos);
    EXPECT_NEAR(orig, rev, 1e-3f);
}

GAME_DIFF_TEST(CTimeCycle2, FindFarClipForCoors_Various) {
    CVector positions[] = {
        CVector(2495.0f, -1665.0f, 13.5f),
        CVector(0.0f, 0.0f, 50.0f),
        CVector(-2170.0f, 250.0f, 30.0f),
        CVector(2027.0f, 1008.0f, 20.0f),
    };
    for (auto& pos : positions) {
        float orig, rev;
        { HookDisableGuard guard("Global/CTimeCycle/FindFarClipForCoors"); orig = CTimeCycle::FindFarClipForCoors(pos); }
        rev = CTimeCycle::FindFarClipForCoors(pos);
        EXPECT_NEAR(orig, rev, 1e-3f);
    }
}
