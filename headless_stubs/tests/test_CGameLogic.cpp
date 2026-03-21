// test_CGameLogic.cpp — Differential tests for CGameLogic.
// Hook paths: Global/CGameLogic/<fn>

#include "StdInc.h"
#include "TestFramework.h"

GAME_DIFF_TEST(CGameLogic, IsCoopGameGoingOn) {
    bool orig, rev;
    { HookDisableGuard guard("Global/CGameLogic/IsCoopGameGoingOn"); orig = CGameLogic::IsCoopGameGoingOn(); }
    rev = CGameLogic::IsCoopGameGoingOn();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CGameLogic, FindCityClosestToPoint) {
    CVector2D positions[] = { {0,0}, {2488,-1666}, {-2200,2300}, {400,-400}, {1500,1500} };
    for (auto& pos : positions) {
        eLevelName orig, rev;
        { HookDisableGuard guard("Global/CGameLogic/FindCityClosestToPoint"); orig = CGameLogic::FindCityClosestToPoint(pos); }
        rev = CGameLogic::FindCityClosestToPoint(pos);
        EXPECT_EQ((int)orig, (int)rev);
    }
}

GAME_DIFF_TEST(CGameLogic, SkipCanBeActivated) {
    bool orig, rev;
    { HookDisableGuard guard("Global/CGameLogic/SkipCanBeActivated"); orig = CGameLogic::SkipCanBeActivated(); }
    rev = CGameLogic::SkipCanBeActivated();
    EXPECT_EQ(orig, rev);
}
