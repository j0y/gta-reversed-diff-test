// test_CWanted.cpp — Differential tests for CWanted.
// Hook paths: Global/CWanted/<fn>
// CWanted is instance-based (per-player). Access via FindPlayerWanted().

#include "StdInc.h"
#include "TestFramework.h"

GAME_DIFF_TEST(CWanted, AreMiamiViceRequired) {
    auto* wanted = FindPlayerWanted();
    if (!wanted) return;
    bool orig, rev;
    { HookDisableGuard guard("Global/CWanted/AreMiamiViceRequired"); orig = wanted->AreMiamiViceRequired(); }
    rev = wanted->AreMiamiViceRequired();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CWanted, AreSwatRequired) {
    auto* wanted = FindPlayerWanted();
    if (!wanted) return;
    bool orig, rev;
    { HookDisableGuard guard("Global/CWanted/AreSwatRequired"); orig = wanted->AreSwatRequired(); }
    rev = wanted->AreSwatRequired();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CWanted, AreFbiRequired) {
    auto* wanted = FindPlayerWanted();
    if (!wanted) return;
    bool orig, rev;
    { HookDisableGuard guard("Global/CWanted/AreFbiRequired"); orig = wanted->AreFbiRequired(); }
    rev = wanted->AreFbiRequired();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CWanted, AreArmyRequired) {
    auto* wanted = FindPlayerWanted();
    if (!wanted) return;
    bool orig, rev;
    { HookDisableGuard guard("Global/CWanted/AreArmyRequired"); orig = wanted->AreArmyRequired(); }
    rev = wanted->AreArmyRequired();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CWanted, NumOfHelisRequired) {
    auto* wanted = FindPlayerWanted();
    if (!wanted) return;
    int32 orig, rev;
    { HookDisableGuard guard("Global/CWanted/NumOfHelisRequired"); orig = wanted->NumOfHelisRequired(); }
    rev = wanted->NumOfHelisRequired();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CWanted, SetMaximumWantedLevel) {
    // Static function — test that state change matches
    auto saved = CWanted::MaximumWantedLevel;
    auto savedMax = CWanted::MaximumChaosLevel;

    { HookDisableGuard guard("Global/CWanted/SetMaximumWantedLevel"); CWanted::SetMaximumWantedLevel(3); }
    auto origLevel = CWanted::MaximumWantedLevel;
    auto origMax = CWanted::MaximumChaosLevel;

    CWanted::MaximumWantedLevel = saved;
    CWanted::MaximumChaosLevel = savedMax;

    CWanted::SetMaximumWantedLevel(3);
    auto revLevel = CWanted::MaximumWantedLevel;
    auto revMax = CWanted::MaximumChaosLevel;

    CWanted::MaximumWantedLevel = saved;
    CWanted::MaximumChaosLevel = savedMax;

    EXPECT_EQ(origLevel, revLevel);
    EXPECT_EQ(origMax, revMax);
}
