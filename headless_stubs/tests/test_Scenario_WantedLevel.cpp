// test_Scenario_WantedLevel.cpp — Scenario: player at various wanted levels.
//
// Sets the player's wanted level to 0-6, then differentially tests all
// CWanted queries that depend on wanted level: cop type thresholds,
// heli count, pursuit state, chaos level mapping.
// Restores wanted level to 0 after each test.

#include "StdInc.h"
#include "TestFramework.h"
#include "Wanted.h"

static CWanted* GetWanted() { return FindPlayerWanted(); }

// Save/restore wanted state
struct WantedSaver {
    uint32 wantedLevel, chaosLevel, wantedBeforeParole, chaosBeforeParole;
    WantedSaver() {
        auto* w = GetWanted();
        wantedLevel = w->m_nWantedLevel;
        chaosLevel = w->m_nChaosLevel;
        wantedBeforeParole = w->m_nWantedLevelBeforeParole;
        chaosBeforeParole = w->m_nChaosLevelBeforeParole;
    }
    ~WantedSaver() {
        auto* w = GetWanted();
        w->m_nWantedLevel = wantedLevel;
        w->m_nChaosLevel = chaosLevel;
        w->m_nWantedLevelBeforeParole = wantedBeforeParole;
        w->m_nChaosLevelBeforeParole = chaosBeforeParole;
    }
};

// --- Differential: SetWantedLevel at each level ---

GAME_DIFF_TEST(Scenario_WantedLevel, SetWantedLevel_AllLevels) {
    auto* w = GetWanted();
    if (!w) return;
    WantedSaver saver;

    for (uint32 level = 0; level <= 6; level++) {
        // Reversed
        w->m_nChaosLevel = 0;
        w->m_nWantedLevel = 0;
        w->SetWantedLevel(level);
        uint32 revWanted = w->m_nWantedLevel;
        uint32 revChaos = w->m_nChaosLevel;

        // Original
        w->m_nChaosLevel = 0;
        w->m_nWantedLevel = 0;
        { HookDisableGuard guard("Global/CWanted/SetWantedLevel");
          w->SetWantedLevel(level); }
        EXPECT_EQ(revWanted, w->m_nWantedLevel);
        EXPECT_EQ(revChaos, w->m_nChaosLevel);
    }
}

GAME_DIFF_TEST(Scenario_WantedLevel, SetWantedLevelNoDrop) {
    auto* w = GetWanted();
    if (!w) return;
    WantedSaver saver;

    // Set to 2, then NoDrop to 1 (should stay at 2)
    w->SetWantedLevel(2);
    w->SetWantedLevelNoDrop(1);
    uint32 revWanted1 = w->m_nWantedLevel;

    // NoDrop to 4 (should go to 4)
    w->SetWantedLevelNoDrop(4);
    uint32 revWanted2 = w->m_nWantedLevel;

    // Original
    w->m_nChaosLevel = 0;
    w->m_nWantedLevel = 0;
    { HookDisableGuard guard("Global/CWanted/SetWantedLevel");
      w->SetWantedLevel(2); }
    { HookDisableGuard guard("Global/CWanted/SetWantedLevelNoDrop");
      w->SetWantedLevelNoDrop(1); }
    EXPECT_EQ(revWanted1, w->m_nWantedLevel);

    { HookDisableGuard guard("Global/CWanted/SetWantedLevelNoDrop");
      w->SetWantedLevelNoDrop(4); }
    EXPECT_EQ(revWanted2, w->m_nWantedLevel);
}

// --- Cop type thresholds at each wanted level ---

GAME_DIFF_TEST(Scenario_WantedLevel, CopThresholds_AllLevels) {
    auto* w = GetWanted();
    if (!w) return;
    WantedSaver saver;

    for (uint32 level = 0; level <= 6; level++) {
        w->m_nChaosLevel = 0;
        w->m_nWantedLevel = 0;
        w->SetWantedLevel(level);

        bool revVice, origVice;
        revVice = w->AreMiamiViceRequired();
        { HookDisableGuard guard("Global/CWanted/AreMiamiViceRequired");
          origVice = w->AreMiamiViceRequired(); }
        EXPECT_EQ(revVice, origVice);

        bool revSwat, origSwat;
        revSwat = w->AreSwatRequired();
        { HookDisableGuard guard("Global/CWanted/AreSwatRequired");
          origSwat = w->AreSwatRequired(); }
        EXPECT_EQ(revSwat, origSwat);

        bool revFbi, origFbi;
        revFbi = w->AreFbiRequired();
        { HookDisableGuard guard("Global/CWanted/AreFbiRequired");
          origFbi = w->AreFbiRequired(); }
        EXPECT_EQ(revFbi, origFbi);

        bool revArmy, origArmy;
        revArmy = w->AreArmyRequired();
        { HookDisableGuard guard("Global/CWanted/AreArmyRequired");
          origArmy = w->AreArmyRequired(); }
        EXPECT_EQ(revArmy, origArmy);

        int32 revHelis, origHelis;
        revHelis = w->NumOfHelisRequired();
        { HookDisableGuard guard("Global/CWanted/NumOfHelisRequired");
          origHelis = w->NumOfHelisRequired(); }
        EXPECT_EQ(revHelis, origHelis);
    }
}

// --- UpdateWantedLevel at different chaos levels ---

GAME_DIFF_TEST(Scenario_WantedLevel, UpdateWantedLevel) {
    auto* w = GetWanted();
    if (!w) return;
    WantedSaver saver;

    // Test chaos→wanted mapping at several chaos levels
    uint32 chaosLevels[] = { 0, 50, 200, 500, 1500, 3000, 6000 };
    for (uint32 chaos : chaosLevels) {
        w->m_nChaosLevel = chaos;
        w->m_nWantedLevel = 0;
        w->UpdateWantedLevel();
        uint32 revWanted = w->m_nWantedLevel;

        w->m_nChaosLevel = chaos;
        w->m_nWantedLevel = 0;
        { HookDisableGuard guard("Global/CWanted/UpdateWantedLevel");
          w->UpdateWantedLevel(); }
        EXPECT_EQ(revWanted, w->m_nWantedLevel);
    }
}

// --- ClearWantedLevelAndGoOnParole ---

GAME_DIFF_TEST(Scenario_WantedLevel, ClearAndParole) {
    auto* w = GetWanted();
    if (!w) return;
    WantedSaver saver;

    w->SetWantedLevel(3);
    w->ClearWantedLevelAndGoOnParole();
    uint32 revWanted = w->m_nWantedLevel;
    uint32 revParole = w->m_nWantedLevelBeforeParole;

    // Reset and do original
    w->m_nWantedLevel = 0;
    w->m_nChaosLevel = 0;
    w->m_nWantedLevelBeforeParole = 0;
    { HookDisableGuard guard("Global/CWanted/SetWantedLevel");
      w->SetWantedLevel(3); }
    { HookDisableGuard guard("Global/CWanted/ClearWantedLevelAndGoOnParole");
      w->ClearWantedLevelAndGoOnParole(); }
    EXPECT_EQ(revWanted, w->m_nWantedLevel);
    EXPECT_EQ(revParole, w->m_nWantedLevelBeforeParole);
}

// --- Behavior tests ---

GAME_TEST(Scenario_WantedLevel, DefaultLevel_IsZero) {
    auto* w = GetWanted();
    if (!w) return;
    // At game state 9 with no crimes, wanted level should be 0
    EXPECT_EQ(w->GetWantedLevel(), 0u);
}

GAME_TEST(Scenario_WantedLevel, ThresholdMonotonic) {
    auto* w = GetWanted();
    if (!w) return;
    WantedSaver saver;

    // Verify monotonic progression: higher levels should never require
    // FEWER cops/helis than lower levels.
    int32 prevHelis = 0;
    for (uint32 level = 0; level <= 6; level++) {
        w->SetWantedLevel(level);
        int32 helis = w->NumOfHelisRequired();
        EXPECT_TRUE(helis >= prevHelis);
        prevHelis = helis;
    }
}

GAME_TEST(Scenario_WantedLevel, SetThenGet_Roundtrip) {
    auto* w = GetWanted();
    if (!w) return;
    WantedSaver saver;

    for (uint32 level = 0; level <= 6; level++) {
        w->SetWantedLevel(level);
        EXPECT_EQ(w->GetWantedLevel(), level);
    }
}
