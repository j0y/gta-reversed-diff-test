// test_CPlayerInfo2.cpp — Extended differential tests for CPlayerInfo.
// Hook paths: Global/CPlayerInfo/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "PlayerInfo.h"
#include "World.h"

// --- IsRestartingAfterMissionFailed ---

GAME_DIFF_TEST(CPlayerInfo2, IsRestartingAfterMissionFailed2) {
    auto& info = FindPlayerInfo(0);
    bool orig, rev;
    { HookDisableGuard guard("Global/CPlayerInfo/IsRestartingAfterMissionFailed"); orig = info.IsRestartingAfterMissionFailed(); }
    rev = info.IsRestartingAfterMissionFailed();
    EXPECT_EQ(orig, rev);
}

// --- IsRestartingAfterArrest ---

GAME_DIFF_TEST(CPlayerInfo2, IsRestartingAfterArrest2) {
    auto& info = FindPlayerInfo(0);
    bool orig, rev;
    { HookDisableGuard guard("Global/CPlayerInfo/IsRestartingAfterArrest"); orig = info.IsRestartingAfterArrest(); }
    rev = info.IsRestartingAfterArrest();
    EXPECT_EQ(orig, rev);
}

// --- IsRestartingAfterDeath ---

GAME_DIFF_TEST(CPlayerInfo2, IsRestartingAfterDeath2) {
    auto& info = FindPlayerInfo(0);
    bool orig, rev;
    { HookDisableGuard guard("Global/CPlayerInfo/IsRestartingAfterDeath"); orig = info.IsRestartingAfterDeath(); }
    rev = info.IsRestartingAfterDeath();
    EXPECT_EQ(orig, rev);
}

// --- IsPlayerInRemoteMode ---

GAME_DIFF_TEST(CPlayerInfo2, IsPlayerInRemoteMode2) {
    auto& info = FindPlayerInfo(0);
    bool orig, rev;
    { HookDisableGuard guard("Global/CPlayerInfo/IsPlayerInRemoteMode"); orig = info.IsPlayerInRemoteMode(); }
    rev = info.IsPlayerInRemoteMode();
    EXPECT_EQ(orig, rev);
}

// PlayerFailedCriticalMission returns void — not a query function
