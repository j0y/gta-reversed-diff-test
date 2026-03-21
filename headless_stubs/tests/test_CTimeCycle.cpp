// test_CTimeCycle.cpp — Differential tests for CTimeCycle getters.
// Hook paths: Global/CTimeCycle/<fn>
// These are pure getters that read from the current time cycle state.

#include "StdInc.h"
#include "TestFramework.h"

#define DIFF_GETTER(Name) \
    GAME_DIFF_TEST(CTimeCycle, Name) { \
        float orig, rev; \
        { HookDisableGuard guard("Global/CTimeCycle/" #Name); orig = CTimeCycle::Name(); } \
        rev = CTimeCycle::Name(); \
        EXPECT_NEAR(orig, rev, 1e-5f); \
    }

DIFF_GETTER(GetAmbientRed)
DIFF_GETTER(GetAmbientGreen)
DIFF_GETTER(GetAmbientBlue)
DIFF_GETTER(GetAmbientRed_Obj)
DIFF_GETTER(GetAmbientGreen_Obj)
DIFF_GETTER(GetAmbientBlue_Obj)
DIFF_GETTER(GetAmbientRed_BeforeBrightness)
DIFF_GETTER(GetAmbientGreen_BeforeBrightness)
DIFF_GETTER(GetAmbientBlue_BeforeBrightness)

#undef DIFF_GETTER

GAME_DIFF_TEST(CTimeCycle, FindFarClipForCoors) {
    CVector positions[] = { {0,0,0}, {1000,1000,50}, {-2000,500,10}, {2488,-1666,13} };
    for (auto& pos : positions) {
        float orig, rev;
        { HookDisableGuard guard("Global/CTimeCycle/FindFarClipForCoors"); orig = CTimeCycle::FindFarClipForCoors(pos); }
        rev = CTimeCycle::FindFarClipForCoors(pos);
        EXPECT_NEAR(orig, rev, 1e-3f);
    }
}
