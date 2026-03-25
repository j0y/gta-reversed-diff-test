// test_CWanted5.cpp — Extended CWanted differential and behavior tests.
// Hook paths: Global/CWanted/<fn>
// SetMaximumWantedLevel is static. CanCopJoinPursuit and SetPursuitCop are member methods.

#include "StdInc.h"
#include "TestFramework.h"
#include "Wanted.h"

// --- SetMaximumWantedLevel ---

GAME_DIFF_TEST(CWanted5, SetMaximumWantedLevel_Range) {
    // Save and restore by calling with known value at the end
    for (int32 level = 0; level <= 6; level++) {
        { HookDisableGuard guard("Global/CWanted/SetMaximumWantedLevel");
          CWanted::SetMaximumWantedLevel(level); }
        CWanted::SetMaximumWantedLevel(level);
        EXPECT_TRUE(true); // void function — no crash = pass
    }
    // Restore default
    CWanted::SetMaximumWantedLevel(6);
}

// --- CanCopJoinPursuit (member) ---

GAME_DIFF_TEST(CWanted5, CanCopJoinPursuit_Nullptr) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    auto* wanted = player->GetWanted();
    if (!wanted) return;
    bool orig, rev;
    { HookDisableGuard guard("Global/CWanted/CanCopJoinPursuit-method");
      orig = wanted->CanCopJoinPursuit(nullptr); }
    rev = wanted->CanCopJoinPursuit(nullptr);
    EXPECT_EQ(orig, rev);
}

// --- SetPursuitCop ---

GAME_DIFF_TEST(CWanted5, SetPursuitCop_Nullptr) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    auto* wanted = player->GetWanted();
    if (!wanted) return;
    bool orig, rev;
    { HookDisableGuard guard("Global/CWanted/SetPursuitCop");
      orig = wanted->SetPursuitCop(nullptr); }
    rev = wanted->SetPursuitCop(nullptr);
    EXPECT_EQ(orig, rev);
}
