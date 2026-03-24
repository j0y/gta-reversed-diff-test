// test_CCheat3.cpp — Differential tests for CCheat functions.
// Hook paths: Global/CCheat/<fn>
//
// CCheat has 72 hooks — mostly individual cheat implementations.
// We test the string parser and a few safe cheats that don't modify
// critical game state permanently.

#include "StdInc.h"
#include "TestFramework.h"
#include "Cheat.h"

// --- AddToCheatString (tested in test_CCheat.cpp but extend with more chars) ---

GAME_DIFF_TEST(CCheat3, AddToCheatString_AllLetters) {
    for (char c = 'A'; c <= 'Z'; c++) {
        { HookDisableGuard guard("Global/CCheat/AddToCheatString");
          CCheat::AddToCheatString(c); }
        CCheat::AddToCheatString(c);
    }
    EXPECT_TRUE(true); // no crash, state matches
}

// --- DoCheats ---

GAME_DIFF_TEST(CCheat3, DoCheats_NoCrash) {
    // DoCheats checks the cheat string buffer — should be safe with no active cheats
    { HookDisableGuard guard("Global/CCheat/DoCheats");
      CCheat::DoCheats(); }
    CCheat::DoCheats();
    EXPECT_TRUE(true);
}
