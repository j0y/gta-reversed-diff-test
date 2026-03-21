// test_CPlayerInfo.cpp — Differential tests for CPlayerInfo member functions.
// Hook paths: Global/CPlayerInfo/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "PlayerInfo.h"
#include "World.h"

static CPlayerInfo* GetPlayerInfo() {
    return &FindPlayerInfo(0);
}

#define DIFF_PLAYERINFO_BOOL(FnName) \
    GAME_DIFF_TEST(CPlayerInfo, FnName) { \
        auto* info = GetPlayerInfo(); \
        if (!info) return; \
        bool orig, rev; \
        { HookDisableGuard guard("Global/CPlayerInfo/" #FnName); orig = info->FnName(); } \
        rev = info->FnName(); \
        EXPECT_EQ(orig, rev); \
    }

DIFF_PLAYERINFO_BOOL(IsRestartingAfterMissionFailed)
DIFF_PLAYERINFO_BOOL(IsRestartingAfterArrest)
DIFF_PLAYERINFO_BOOL(IsRestartingAfterDeath)
DIFF_PLAYERINFO_BOOL(IsPlayerInRemoteMode)
