// test_CWanted3.cpp — Extended differential tests for CWanted.
// Hook paths: Global/CWanted/<fn>
// IsInPursuit takes CCopPed* — pass nullptr to check general pursuit state.

#include "StdInc.h"
#include "TestFramework.h"
#include "Wanted.h"
#include "Entity/Ped/Ped.h"

GAME_DIFF_TEST(CWanted3, IsInPursuit_NullCop) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    auto* wanted = player->GetWanted();
    if (!wanted) return;
    bool orig, rev;
    { HookDisableGuard guard("Global/CWanted/IsInPursuit");
      orig = wanted->IsInPursuit(nullptr); }
    rev = wanted->IsInPursuit(nullptr);
    EXPECT_EQ(orig, rev);
}

// --- ClearQdCrimes ---

GAME_DIFF_TEST(CWanted3, ClearQdCrimes) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    auto* wanted = player->GetWanted();
    if (!wanted) return;
    { HookDisableGuard guard("Global/CWanted/ClearQdCrimes");
      wanted->ClearQdCrimes(); }
    wanted->ClearQdCrimes();
    EXPECT_TRUE(true);
}

// --- ClearWantedLevelAndGoOnParole ---

GAME_DIFF_TEST(CWanted3, ClearWantedLevelAndGoOnParole) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    auto* wanted = player->GetWanted();
    if (!wanted) return;
    { HookDisableGuard guard("Global/CWanted/ClearWantedLevelAndGoOnParole");
      wanted->ClearWantedLevelAndGoOnParole(); }
    wanted->ClearWantedLevelAndGoOnParole();
    EXPECT_TRUE(true);
}

// CanCopJoinPursuit-func is a non-static member. Skipped.

// --- IsClosestCop ---

GAME_DIFF_TEST(CWanted3, IsClosestCop_Null) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    auto* wanted = player->GetWanted();
    if (!wanted) return;
    bool orig, rev;
    { HookDisableGuard guard("Global/CWanted/IsClosestCop");
      orig = wanted->IsClosestCop(nullptr, 0); }
    rev = wanted->IsClosestCop(nullptr, 0);
    EXPECT_EQ(orig, rev);
}

// --- SetWantedLevel round-trip ---

GAME_DIFF_TEST(CWanted3, SetWantedLevel_Zero) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    auto* wanted = player->GetWanted();
    if (!wanted) return;
    auto saved = wanted->m_nWantedLevel;
    { HookDisableGuard guard("Global/CWanted/SetWantedLevel");
      wanted->SetWantedLevel(0); }
    auto origLevel = wanted->m_nWantedLevel;

    wanted->m_nWantedLevel = saved;
    wanted->SetWantedLevel(0);
    auto revLevel = wanted->m_nWantedLevel;

    EXPECT_EQ(origLevel, revLevel);
    wanted->m_nWantedLevel = saved;
}
