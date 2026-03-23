// test_CStats3.cpp — Extended deep differential tests for CStats.
// Hook paths: Global/CStats/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "Stats.h"

// --- GetStatID ---

GAME_DIFF_TEST(CStats3, GetStatID_FullRange) {
    for (int32 id = 0; id < 225; id++) {
        char* orig;
        char* rev;
        { HookDisableGuard guard("Global/CStats/GetStatID"); orig = CStats::GetStatID(static_cast<eStats>(id)); }
        rev = CStats::GetStatID(static_cast<eStats>(id));
        EXPECT_EQ((uintptr_t)orig, (uintptr_t)rev);
    }
}

// --- GetTimesMissionAttempted ---

GAME_DIFF_TEST(CStats3, GetTimesMissionAttempted_Various) {
    uint8 missions[] = { 0, 1, 5, 10, 20, 50, 100 };
    for (uint8 m : missions) {
        int8 orig, rev;
        { HookDisableGuard guard("Global/CStats/GetTimesMissionAttempted"); orig = CStats::GetTimesMissionAttempted(m); }
        rev = CStats::GetTimesMissionAttempted(m);
        EXPECT_EQ(orig, rev);
    }
}

// --- GetFullFavoriteRadioStationList ---

GAME_DIFF_TEST(CStats3, GetFullFavoriteRadioStationList) {
    int32* orig;
    int32* rev;
    { HookDisableGuard guard("Global/CStats/GetFullFavoriteRadioStationList"); orig = CStats::GetFullFavoriteRadioStationList(); }
    rev = CStats::GetFullFavoriteRadioStationList();
    // Should return same static array pointer
    EXPECT_EQ((uintptr_t)orig, (uintptr_t)rev);
}
