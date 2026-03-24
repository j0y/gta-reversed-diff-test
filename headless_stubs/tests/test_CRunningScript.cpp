// test_CRunningScript.cpp — Differential tests for CRunningScript.
// Hook paths: Scripts/CRunningScript/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "Scripts/RunningScript.h"
#include "Entity/Ped/Ped.h"

GAME_DIFF_TEST(CRunningScript, IsPedDead_Player) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    auto* script = CTheScripts::pActiveScripts;
    if (!script) return;
    bool orig, rev;
    { HookDisableGuard guard("Scripts/CRunningScript/IsPedDead");
      orig = script->IsPedDead(player); }
    rev = script->IsPedDead(player);
    EXPECT_EQ(orig, rev);
}
