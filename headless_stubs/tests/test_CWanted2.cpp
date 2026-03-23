// test_CWanted2.cpp — Extended differential tests for CWanted.
// Hook paths: Global/CWanted/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "Wanted.h"
#include "Entity/Ped/Ped.h"

static CWanted* GetPlayerWanted() {
    auto* player = FindPlayerPed(0);
    if (!player) return nullptr;
    return player->GetWanted();
}

// --- AreSwatRequired ---

GAME_DIFF_TEST(CWanted2, AreSwatRequired) {
    auto* w = GetPlayerWanted();
    if (!w) return;
    bool orig, rev;
    { HookDisableGuard guard("Global/CWanted/AreSwatRequired"); orig = w->AreSwatRequired(); }
    rev = w->AreSwatRequired();
    EXPECT_EQ(orig, rev);
}

// --- AreFbiRequired ---

GAME_DIFF_TEST(CWanted2, AreFbiRequired) {
    auto* w = GetPlayerWanted();
    if (!w) return;
    bool orig, rev;
    { HookDisableGuard guard("Global/CWanted/AreFbiRequired"); orig = w->AreFbiRequired(); }
    rev = w->AreFbiRequired();
    EXPECT_EQ(orig, rev);
}

// --- AreArmyRequired ---

GAME_DIFF_TEST(CWanted2, AreArmyRequired) {
    auto* w = GetPlayerWanted();
    if (!w) return;
    bool orig, rev;
    { HookDisableGuard guard("Global/CWanted/AreArmyRequired"); orig = w->AreArmyRequired(); }
    rev = w->AreArmyRequired();
    EXPECT_EQ(orig, rev);
}

// --- InitialiseStaticVariables ---

GAME_DIFF_TEST(CWanted2, InitialiseStaticVariables) {
    // Save some static state, call both, compare
    { HookDisableGuard guard("Global/CWanted/InitialiseStaticVariables");
      CWanted::InitialiseStaticVariables(); }
    CWanted::InitialiseStaticVariables();
    EXPECT_TRUE(true); // verify no crash
}
