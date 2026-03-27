// test_COnscreenTimer.cpp — Differential tests for COnscreenTimer.
// Hook paths: Global/COnscreenTimer/<fn>
//
// All functions operate on the COnscreenTimer struct members.
// No rendering, no complex game state.

#include "StdInc.h"
#include "TestFramework.h"
#include "OnscreenTimer.h"

// --- AddClock ---

GAME_DIFF_TEST(COnscreenTimer, AddClock) {
    COnscreenTimer orig{}, rev{};
    orig.Init(); rev.Init();

    // Add with GXT key
    { HookDisableGuard guard("Global/COnscreenTimer/AddClock");
      orig.AddClock(42, (char*)"TIMER1", eTimerDirection::DECREASE); }
    rev.AddClock(42, (char*)"TIMER1", eTimerDirection::DECREASE);

    EXPECT_EQ(orig.m_Clock.m_nVarId, rev.m_Clock.m_nVarId);
    EXPECT_EQ(orig.m_Clock.m_nTimerDirection, rev.m_Clock.m_nTimerDirection);
    EXPECT_EQ(strcmp(orig.m_Clock.m_szDescriptionTextKey, rev.m_Clock.m_szDescriptionTextKey), 0);
}

GAME_DIFF_TEST(COnscreenTimer, AddClock_NullGxt) {
    COnscreenTimer orig{}, rev{};
    orig.Init(); rev.Init();

    { HookDisableGuard guard("Global/COnscreenTimer/AddClock");
      orig.AddClock(99, nullptr, eTimerDirection::INCREASE); }
    rev.AddClock(99, nullptr, eTimerDirection::INCREASE);

    EXPECT_EQ(orig.m_Clock.m_nVarId, rev.m_Clock.m_nVarId);
    EXPECT_EQ(orig.m_Clock.m_szDescriptionTextKey[0], rev.m_Clock.m_szDescriptionTextKey[0]);
}

GAME_DIFF_TEST(COnscreenTimer, AddClock_AlreadyActive) {
    COnscreenTimer orig{}, rev{};
    orig.Init(); rev.Init();
    // Add first clock
    orig.AddClock(10, (char*)"FIRST", eTimerDirection::DECREASE);
    rev.AddClock(10, (char*)"FIRST", eTimerDirection::DECREASE);
    // Try to add second — should be ignored
    { HookDisableGuard guard("Global/COnscreenTimer/AddClock");
      orig.AddClock(20, (char*)"SECOND", eTimerDirection::INCREASE); }
    rev.AddClock(20, (char*)"SECOND", eTimerDirection::INCREASE);

    EXPECT_EQ(orig.m_Clock.m_nVarId, rev.m_Clock.m_nVarId);
    EXPECT_EQ(orig.m_Clock.m_nVarId, (uint32)10);  // first one kept
}

// --- ClearClock ---

GAME_DIFF_TEST(COnscreenTimer, ClearClock) {
    COnscreenTimer orig{}, rev{};
    orig.Init(); rev.Init();
    orig.AddClock(42, (char*)"TEST", eTimerDirection::DECREASE);
    rev.AddClock(42, (char*)"TEST", eTimerDirection::DECREASE);

    { HookDisableGuard guard("Global/COnscreenTimer/ClearClock");
      orig.ClearClock(42); }
    rev.ClearClock(42);

    EXPECT_EQ(orig.m_Clock.m_nVarId, rev.m_Clock.m_nVarId);
    EXPECT_EQ(orig.m_Clock.m_bEnabled, rev.m_Clock.m_bEnabled);
    EXPECT_EQ(orig.m_Clock.m_nVarId, (uint32)0);
}

GAME_DIFF_TEST(COnscreenTimer, ClearClock_WrongId) {
    COnscreenTimer orig{}, rev{};
    orig.Init(); rev.Init();
    orig.AddClock(42, (char*)"TEST", eTimerDirection::DECREASE);
    rev.AddClock(42, (char*)"TEST", eTimerDirection::DECREASE);

    // Clear with wrong varId — should be no-op
    { HookDisableGuard guard("Global/COnscreenTimer/ClearClock");
      orig.ClearClock(99); }
    rev.ClearClock(99);

    EXPECT_EQ(orig.m_Clock.m_nVarId, rev.m_Clock.m_nVarId);
    EXPECT_EQ(orig.m_Clock.m_nVarId, (uint32)42);  // unchanged
}

// --- SetClockBeepCountdownSecs ---

GAME_DIFF_TEST(COnscreenTimer, SetClockBeepCountdownSecs) {
    COnscreenTimer orig{}, rev{};
    orig.Init(); rev.Init();
    orig.AddClock(42, (char*)"TEST", eTimerDirection::DECREASE);
    rev.AddClock(42, (char*)"TEST", eTimerDirection::DECREASE);

    { HookDisableGuard guard("Global/COnscreenTimer/SetClockBeepCountdownSecs");
      orig.SetClockBeepCountdownSecs(42, 10); }
    rev.SetClockBeepCountdownSecs(42, 10);

    EXPECT_EQ(orig.m_Clock.m_nClockBeepCountdownSecs, rev.m_Clock.m_nClockBeepCountdownSecs);
}

// --- AddCounter ---

GAME_DIFF_TEST(COnscreenTimer, AddCounter) {
    COnscreenTimer orig{}, rev{};
    orig.Init(); rev.Init();

    { HookDisableGuard guard("Global/COnscreenTimer/AddCounter");
      orig.AddCounter(100, eOnscreenCounter::SIMPLE, (char*)"CNT1", 0); }
    rev.AddCounter(100, eOnscreenCounter::SIMPLE, (char*)"CNT1", 0);

    EXPECT_EQ(orig.m_aCounters[0].m_nVarId, rev.m_aCounters[0].m_nVarId);
    EXPECT_EQ(orig.m_aCounters[0].m_nType, rev.m_aCounters[0].m_nType);
    EXPECT_EQ(strcmp(orig.m_aCounters[0].m_szDescriptionTextKey,
                     rev.m_aCounters[0].m_szDescriptionTextKey), 0);
    EXPECT_EQ(orig.m_aCounters[0].m_bFlashWhenFirstDisplayed,
              rev.m_aCounters[0].m_bFlashWhenFirstDisplayed);
}

GAME_DIFF_TEST(COnscreenTimer, AddCounter_MultipleSlots) {
    COnscreenTimer orig{}, rev{};
    orig.Init(); rev.Init();

    for (uint16 i = 0; i < 4; i++) {
        { HookDisableGuard guard("Global/COnscreenTimer/AddCounter");
          orig.AddCounter(100 + i, eOnscreenCounter::SIMPLE, nullptr, i); }
        rev.AddCounter(100 + i, eOnscreenCounter::SIMPLE, nullptr, i);
    }

    for (uint16 i = 0; i < 4; i++) {
        EXPECT_EQ(orig.m_aCounters[i].m_nVarId, rev.m_aCounters[i].m_nVarId);
    }
}

// --- ClearCounter ---

GAME_DIFF_TEST(COnscreenTimer, ClearCounter) {
    COnscreenTimer orig{}, rev{};
    orig.Init(); rev.Init();
    orig.AddCounter(100, eOnscreenCounter::SIMPLE, (char*)"CNT1", 0);
    rev.AddCounter(100, eOnscreenCounter::SIMPLE, (char*)"CNT1", 0);
    orig.AddCounter(200, eOnscreenCounter::DOUBLE, (char*)"CNT2", 1);
    rev.AddCounter(200, eOnscreenCounter::DOUBLE, (char*)"CNT2", 1);

    // Clear only counter with varId 100
    { HookDisableGuard guard("Global/COnscreenTimer/ClearCounter");
      orig.ClearCounter(100); }
    rev.ClearCounter(100);

    EXPECT_EQ(orig.m_aCounters[0].m_nVarId, rev.m_aCounters[0].m_nVarId);
    EXPECT_EQ(orig.m_aCounters[0].m_nVarId, (uint32)0);   // cleared
    EXPECT_EQ(orig.m_aCounters[1].m_nVarId, rev.m_aCounters[1].m_nVarId);
    EXPECT_EQ(orig.m_aCounters[1].m_nVarId, (uint32)200);  // unchanged
}

// --- ClearClock: probe m_szDescriptionTextKey side effect ---
// Reversed code: sets m_nVarId=0 THEN writes m_szDescriptionTextKey[m_nVarId] = '\0'
// which always writes to index 0. But if original evaluates the index BEFORE zeroing
// m_nVarId, it writes to a different index (the old varId value).

GAME_DIFF_TEST(COnscreenTimer, ClearClock_TextKeyBytes) {
    COnscreenTimer orig{}, rev{};
    orig.Init(); rev.Init();

    // Use varId=5 so if original uses old varId as index, it writes to [5] not [0]
    orig.AddClock(5, (char*)"ABCDEFG", eTimerDirection::DECREASE);
    rev.AddClock(5, (char*)"ABCDEFG", eTimerDirection::DECREASE);

    { HookDisableGuard guard("Global/COnscreenTimer/ClearClock");
      orig.ClearClock(5); }
    rev.ClearClock(5);

    // Compare the full text key buffer
    for (int i = 0; i < 10; i++) {
        EXPECT_EQ(orig.m_Clock.m_szDescriptionTextKey[i],
                  rev.m_Clock.m_szDescriptionTextKey[i]);
    }
}

// --- SetClockBeepCountdownSecs: probe loop behavior ---
// Reversed loops over m_aCounters but condition/body only touches m_Clock.
// If original doesn't loop (just a single check), behavior is identical.
// But if the loop causes any side effects (counter iteration), it could diverge.

GAME_DIFF_TEST(COnscreenTimer, SetClockBeepCountdownSecs_WithCounters) {
    COnscreenTimer orig{}, rev{};
    orig.Init(); rev.Init();

    // Set up clock and multiple counters
    orig.AddClock(42, (char*)"CLK", eTimerDirection::DECREASE);
    rev.AddClock(42, (char*)"CLK", eTimerDirection::DECREASE);
    orig.AddCounter(100, eOnscreenCounter::SIMPLE, (char*)"C1", 0);
    rev.AddCounter(100, eOnscreenCounter::SIMPLE, (char*)"C1", 0);
    orig.AddCounter(200, eOnscreenCounter::DOUBLE, (char*)"C2", 1);
    rev.AddCounter(200, eOnscreenCounter::DOUBLE, (char*)"C2", 1);

    { HookDisableGuard guard("Global/COnscreenTimer/SetClockBeepCountdownSecs");
      orig.SetClockBeepCountdownSecs(42, 15); }
    rev.SetClockBeepCountdownSecs(42, 15);

    EXPECT_EQ(orig.m_Clock.m_nClockBeepCountdownSecs, rev.m_Clock.m_nClockBeepCountdownSecs);
    // Verify counters weren't corrupted
    EXPECT_EQ(orig.m_aCounters[0].m_nVarId, rev.m_aCounters[0].m_nVarId);
    EXPECT_EQ(orig.m_aCounters[1].m_nVarId, rev.m_aCounters[1].m_nVarId);
}

GAME_DIFF_TEST(COnscreenTimer, SetClockBeepCountdownSecs_WrongVarId) {
    COnscreenTimer orig{}, rev{};
    orig.Init(); rev.Init();
    orig.AddClock(42, (char*)"CLK", eTimerDirection::DECREASE);
    rev.AddClock(42, (char*)"CLK", eTimerDirection::DECREASE);

    // Wrong varId — should be no-op
    { HookDisableGuard guard("Global/COnscreenTimer/SetClockBeepCountdownSecs");
      orig.SetClockBeepCountdownSecs(99, 15); }
    rev.SetClockBeepCountdownSecs(99, 15);

    // Both should agree — value should be unchanged from before the call
    EXPECT_EQ(orig.m_Clock.m_nClockBeepCountdownSecs, rev.m_Clock.m_nClockBeepCountdownSecs);
}

// --- SetCounterFlashWhenFirstDisplayed ---

GAME_DIFF_TEST(COnscreenTimer, SetCounterFlashWhenFirstDisplayed) {
    COnscreenTimer orig{}, rev{};
    orig.Init(); rev.Init();
    orig.AddCounter(100, eOnscreenCounter::SIMPLE, nullptr, 0);
    rev.AddCounter(100, eOnscreenCounter::SIMPLE, nullptr, 0);

    { HookDisableGuard guard("Global/COnscreenTimer/SetCounterFlashWhenFirstDisplayed");
      orig.SetCounterFlashWhenFirstDisplayed(100, false); }
    rev.SetCounterFlashWhenFirstDisplayed(100, false);

    EXPECT_EQ(orig.m_aCounters[0].m_bFlashWhenFirstDisplayed,
              rev.m_aCounters[0].m_bFlashWhenFirstDisplayed);
    EXPECT_EQ(rev.m_aCounters[0].m_bFlashWhenFirstDisplayed, false);
}
