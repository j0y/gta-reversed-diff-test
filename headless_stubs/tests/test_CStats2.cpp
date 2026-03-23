// test_CStats2.cpp — Extended differential tests for CStats.
// Hook paths: Global/CStats/<fn>
//
// Tests stat query functions across the full stat ID range.

#include "StdInc.h"
#include "TestFramework.h"
#include "Stats.h"

// --- FindMaxNumberOfGroupMembers ---

GAME_DIFF_TEST(CStats, FindMaxNumberOfGroupMembers) {
    int32 orig, rev;
    { HookDisableGuard guard("Global/CStats/FindMaxNumberOfGroupMembers");
      orig = CStats::FindMaxNumberOfGroupMembers(); }
    rev = CStats::FindMaxNumberOfGroupMembers();
    EXPECT_EQ(orig, rev);
}

// --- IsStatFloat across full range ---

GAME_DIFF_TEST(CStats, IsStatFloat_FullRange) {
    for (int32 id = 0; id < 225; id++) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CStats/IsStatFloat");
          orig = CStats::IsStatFloat(static_cast<eStats>(id)); }
        rev = CStats::IsStatFloat(static_cast<eStats>(id));
        EXPECT_EQ(orig, rev);
    }
}

// --- IsStatCapped across full range ---

GAME_DIFF_TEST(CStats, IsStatCapped_FullRange) {
    for (int32 id = 0; id < 225; id++) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CStats/IsStatCapped");
          orig = CStats::IsStatCapped(static_cast<eStats>(id)); }
        rev = CStats::IsStatCapped(static_cast<eStats>(id));
        EXPECT_EQ(orig, rev);
    }
}

// --- SafeToShowThisStat across full range ---

GAME_DIFF_TEST(CStats, SafeToShowThisStat_FullRange) {
    for (int32 id = 0; id < 225; id++) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CStats/SafeToShowThisStat");
          orig = CStats::SafeToShowThisStat(static_cast<eStats>(id)); }
        rev = CStats::SafeToShowThisStat(static_cast<eStats>(id));
        EXPECT_EQ(orig, rev);
    }
}

// CheckForThreshold takes float*, not eStats — skipped (different signature)
