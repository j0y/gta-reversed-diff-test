// test_CStats.cpp — Differential tests for CStats.
// Hook paths: Global/CStats/<fn>

#include "StdInc.h"
#include "TestFramework.h"

GAME_DIFF_TEST(CStats, ConvertToMins) {
    int inputs[] = { 0, 30, 60, 90, 120, 3600, 7200 };
    for (int secs : inputs) {
        EXPECT_MATCHES_ORIGINAL("Global/CStats/ConvertToMins", CStats::ConvertToMins, 0, secs);
    }
}

GAME_DIFF_TEST(CStats, ConvertToSecs) {
    int inputs[] = { 0, 30, 60, 90, 120, 3600, 7200 };
    for (int secs : inputs) {
        EXPECT_MATCHES_ORIGINAL("Global/CStats/ConvertToSecs", CStats::ConvertToSecs, 0, secs);
    }
}

GAME_DIFF_TEST(CStats, IsStatFloat) {
    // Test a range of stat IDs
    for (int id = 0; id < 50; id++) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CStats/IsStatFloat"); orig = CStats::IsStatFloat(static_cast<eStats>(id)); }
        rev = CStats::IsStatFloat(static_cast<eStats>(id));
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(CStats, IsStatCapped) {
    for (int id = 0; id < 50; id++) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CStats/IsStatCapped"); orig = CStats::IsStatCapped(static_cast<eStats>(id)); }
        rev = CStats::IsStatCapped(static_cast<eStats>(id));
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(CStats, FindCriminalRatingNumber) {
    int orig, rev;
    { HookDisableGuard guard("Global/CStats/FindCriminalRatingNumber"); orig = CStats::FindCriminalRatingNumber(); }
    rev = CStats::FindCriminalRatingNumber();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CStats, GetPercentageProgress) {
    float orig, rev;
    { HookDisableGuard guard("Global/CStats/GetPercentageProgress"); orig = CStats::GetPercentageProgress(); }
    rev = CStats::GetPercentageProgress();
    EXPECT_NEAR(orig, rev, 1e-5f);
}

GAME_DIFF_TEST(CStats, SafeToShowThisStat) {
    // Test a range of stat IDs
    int statIds[] = { 0, 1, 2, 5, 10, 20, 50, 100, 150, 200 };
    for (int id : statIds) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CStats/SafeToShowThisStat"); orig = CStats::SafeToShowThisStat((eStats)id); }
        rev = CStats::SafeToShowThisStat((eStats)id);
        EXPECT_EQ(orig, rev);
    }
}
