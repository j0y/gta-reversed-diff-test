// test_CCheat2.cpp — Deep differential tests for CCheat.
// Hook paths: Global/CCheat/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "Cheat.h"

// --- IsZoneStreamingAllowed ---

GAME_DIFF_TEST(CCheat, IsZoneStreamingAllowed) {
    bool orig, rev;
    { HookDisableGuard guard("Global/CCheat/IsZoneStreamingAllowed");
      orig = CCheat::IsZoneStreamingAllowed(); }
    rev = CCheat::IsZoneStreamingAllowed();
    EXPECT_EQ(orig, rev);
}

// --- AddToCheatString with various chars ---

GAME_DIFF_TEST(CCheat, AddToCheatString_Letters) {
    // Save cheat string state
    char saved[30];
    memcpy(saved, &CCheat::m_CheatString, sizeof(saved));

    char letters[] = "ABCDEFGHIJ";
    for (char c : letters) {
        if (c == 0) break;
        { HookDisableGuard guard("Global/CCheat/AddToCheatString");
          CCheat::AddToCheatString(c); }
        char origState[30];
        memcpy(origState, &CCheat::m_CheatString, sizeof(origState));

        // Restore and do reversed
        memcpy(&CCheat::m_CheatString, saved, sizeof(saved));
        CCheat::AddToCheatString(c);
        char revState[30];
        memcpy(revState, &CCheat::m_CheatString, sizeof(revState));

        // Compare string states
        EXPECT_EQ(memcmp(origState, revState, sizeof(origState)), 0);

        // Update saved for next iteration
        memcpy(saved, revState, sizeof(saved));
    }

    // Restore original state
    memcpy(&CCheat::m_CheatString, saved, sizeof(saved));
}
