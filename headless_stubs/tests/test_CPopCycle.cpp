// test_CPopCycle.cpp — Differential tests for CPopCycle.
// Hook paths: Global/CPopCycle/<fn>

#include "StdInc.h"
#include "TestFramework.h"

GAME_DIFF_TEST(CPopCycle, GetCurrentPercOther_Peds) {
    float orig, rev;
    { HookDisableGuard guard("Global/CPopCycle/GetCurrentPercOther_Peds"); orig = CPopCycle::GetCurrentPercOther_Peds(); }
    rev = CPopCycle::GetCurrentPercOther_Peds();
    EXPECT_NEAR(orig, rev, 1e-5f);
}

GAME_DIFF_TEST(CPopCycle, IsPedInGroup) {
    // Test a few model/group combos
    for (int model = 0; model < 10; model++) {
        for (int group = 0; group < 5; group++) {
            bool orig, rev;
            { HookDisableGuard guard("Global/CPopCycle/IsPedInGroup");
              orig = CPopCycle::IsPedInGroup(model, (ePopcycleGroup)group); }
            rev = CPopCycle::IsPedInGroup(model, (ePopcycleGroup)group);
            EXPECT_EQ(orig, rev);
        }
    }
}

// IsPedAppropriateForCurrentZone, UpdatePercentages, UpdateAreaDodgyness — original crashes. Skipped.

// --- PlayerKilledADealer ---

GAME_DIFF_TEST(CPopCycle, PlayerKilledADealer_NoCrash) {
    { HookDisableGuard guard("Global/CPopCycle/PlayerKilledADealer");
      CPopCycle::PlayerKilledADealer(); }
    CPopCycle::PlayerKilledADealer();
    EXPECT_TRUE(true);
}
