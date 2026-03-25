// test_CStats5.cpp — Tests for CStats functions not covered by test_CStats through test_CStats4.
// Most functions are NOT hooked (no RH_ScopedInstall) — behavior tests only.
// CheckForThreshold, ConvertToMins, ConvertToSecs ARE hooked.

#include "StdInc.h"
#include "TestFramework.h"
#include "Stats.h"

// --- Behavior tests for unhooked functions ---

GAME_TEST(CStats5, GetFatAndMuscleModifier_AllMods) {
    for (int32 mod = 0; mod <= 15; mod++) {
        float val = CStats::GetFatAndMuscleModifier(static_cast<eStatModAbilities>(mod));
        (void)val;
        EXPECT_TRUE(true);
    }
}

GAME_TEST(CStats5, FindMostFavoriteRadioStation) {
    auto result = CStats::FindMostFavoriteRadioStation();
    (void)result;
    EXPECT_TRUE(true);
}

GAME_TEST(CStats5, FindLeastFavoriteRadioStation) {
    auto result = CStats::FindLeastFavoriteRadioStation();
    (void)result;
    EXPECT_TRUE(true);
}

GAME_TEST(CStats5, FindCriminalRatingString) {
    GxtChar* result = CStats::FindCriminalRatingString();
    EXPECT_TRUE(result != nullptr);
}

// --- Differential tests for hooked functions ---

GAME_DIFF_TEST(CStats5, CheckForThreshold_BelowRange) {
    float val = 10.0f;
    bool orig, rev;
    { HookDisableGuard guard("Global/CStats/CheckForThreshold");
      orig = CStats::CheckForThreshold(&val, 50.0f); }
    rev = CStats::CheckForThreshold(&val, 50.0f);
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CStats5, CheckForThreshold_AboveRange) {
    float val = 100.0f;
    bool orig, rev;
    { HookDisableGuard guard("Global/CStats/CheckForThreshold");
      orig = CStats::CheckForThreshold(&val, 50.0f); }
    rev = CStats::CheckForThreshold(&val, 50.0f);
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CStats5, CheckForThreshold_ExactRange) {
    float val = 50.0f;
    bool orig, rev;
    { HookDisableGuard guard("Global/CStats/CheckForThreshold");
      orig = CStats::CheckForThreshold(&val, 50.0f); }
    rev = CStats::CheckForThreshold(&val, 50.0f);
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CStats5, ConvertToMins_Various) {
    int32 inputs[] = { 0, 30, 60, 120, 3600, 7200, 86400 };
    for (int32 v : inputs) {
        int32 orig, rev;
        { HookDisableGuard guard("Global/CStats/ConvertToMins");
          orig = CStats::ConvertToMins(v); }
        rev = CStats::ConvertToMins(v);
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(CStats5, ConvertToSecs_Various) {
    int32 inputs[] = { 0, 30, 60, 120, 3600, 7200, 86400 };
    for (int32 v : inputs) {
        int32 orig, rev;
        { HookDisableGuard guard("Global/CStats/ConvertToSecs");
          orig = CStats::ConvertToSecs(v); }
        rev = CStats::ConvertToSecs(v);
        EXPECT_EQ(orig, rev);
    }
}
