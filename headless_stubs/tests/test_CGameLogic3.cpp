// test_CGameLogic3.cpp — Extended differential tests for CGameLogic.
// Hook paths: Global/CGameLogic/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "GameLogic.h"
#include "Entity/Ped/Ped.h"

// --- IsPlayerAllowedToGoInThisDirection ---

GAME_DIFF_TEST(CGameLogic3, IsPlayerAllowedToGoInThisDirection_Various) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    CVector directions[] = {
        CVector(1.0f, 0.0f, 0.0f),    // east
        CVector(-1.0f, 0.0f, 0.0f),   // west
        CVector(0.0f, 1.0f, 0.0f),    // north
        CVector(0.0f, -1.0f, 0.0f),   // south
    };
    float limits[] = { 10.0f, 50.0f, 200.0f };
    for (auto& dir : directions) {
        for (float lim : limits) {
            bool orig, rev;
            { HookDisableGuard guard("Global/CGameLogic/IsPlayerAllowedToGoInThisDirection");
              orig = CGameLogic::IsPlayerAllowedToGoInThisDirection(player, dir, lim); }
            rev = CGameLogic::IsPlayerAllowedToGoInThisDirection(player, dir, lim);
            EXPECT_EQ(orig, rev);
        }
    }
}

// --- PassTime ---

GAME_DIFF_TEST(CGameLogic3, PassTime_Zero) {
    // Save clock state
    auto savedH = CClock::ms_nGameClockHours;
    auto savedM = CClock::ms_nGameClockMinutes;

    { HookDisableGuard guard("Global/CGameLogic/PassTime");
      CGameLogic::PassTime(0); }
    auto origH = CClock::ms_nGameClockHours;
    auto origM = CClock::ms_nGameClockMinutes;

    // Restore and test reversed
    CClock::ms_nGameClockHours = savedH;
    CClock::ms_nGameClockMinutes = savedM;
    CGameLogic::PassTime(0);
    auto revH = CClock::ms_nGameClockHours;
    auto revM = CClock::ms_nGameClockMinutes;

    EXPECT_EQ(origH, revH);
    EXPECT_EQ(origM, revM);

    // Restore
    CClock::ms_nGameClockHours = savedH;
    CClock::ms_nGameClockMinutes = savedM;
}
