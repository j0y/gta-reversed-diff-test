// test_CStats4.cpp — Extended differential tests for CStats.
// Hook paths: Global/CStats/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "Stats.h"

GAME_DIFF_TEST(CStats4, GetPercentageProgress) {
    float orig, rev;
    { HookDisableGuard guard("Global/CStats/GetPercentageProgress"); orig = CStats::GetPercentageProgress(); }
    rev = CStats::GetPercentageProgress();
    EXPECT_NEAR(orig, rev, 1e-3f);
}

GAME_DIFF_TEST(CStats4, FindCriminalRatingNumber) {
    int32 orig, rev;
    { HookDisableGuard guard("Global/CStats/FindCriminalRatingNumber"); orig = CStats::FindCriminalRatingNumber(); }
    rev = CStats::FindCriminalRatingNumber();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CStats4, GetTimesMissionAttempted_Range) {
    for (int32 id = 0; id < 10; id++) {
        int32 orig, rev;
        { HookDisableGuard guard("Global/CStats/GetTimesMissionAttempted"); orig = CStats::GetTimesMissionAttempted(id); }
        rev = CStats::GetTimesMissionAttempted(id);
        EXPECT_EQ(orig, rev);
    }
}

// --- GetStatValue (overloaded: -OG) ---

GAME_DIFF_TEST(CStats4, GetStatValue_OG_Range) {
    for (int32 id = 0; id < 30; id++) {
        float orig, rev;
        { HookDisableGuard guard("Global/CStats/GetStatValue--OG");
          orig = CStats::GetStatValue(static_cast<eStats>(id)); }
        rev = CStats::GetStatValue(static_cast<eStats>(id));
        EXPECT_NEAR(orig, rev, 1e-3f);
    }
}

// --- FindMaxNumberOfGroupMembers ---

GAME_DIFF_TEST(CStats4, FindMaxNumberOfGroupMembers) {
    int32 orig, rev;
    { HookDisableGuard guard("Global/CStats/FindMaxNumberOfGroupMembers");
      orig = CStats::FindMaxNumberOfGroupMembers(); }
    rev = CStats::FindMaxNumberOfGroupMembers();
    EXPECT_EQ(orig, rev);
}

// --- GetStatValue FULL SWEEP (all 225 stat IDs) ---

GAME_DIFF_TEST(CStats4, GetStatValue_OG_FullSweep) {
    for (int32 id = 0; id < 225; id++) {
        float orig, rev;
        { HookDisableGuard guard("Global/CStats/GetStatValue--OG");
          orig = CStats::GetStatValue(static_cast<eStats>(id)); }
        rev = CStats::GetStatValue(static_cast<eStats>(id));
        EXPECT_NEAR(orig, rev, 1e-3f);
    }
}
