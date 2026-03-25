// test_CEntryExit2.cpp — CEntryExitManager differential tests.
// Hook paths: Global/CEntryExitManager/<fn>
//
// BUG CANDIDATE: GetEntryExitIndex returns different values for known names
// when comparing original vs reversed. The reversed code may use a different
// search order or matching logic. Needs disassembly investigation.

#include "StdInc.h"
#include "TestFramework.h"
#include "EntryExitManager.h"

GAME_DIFF_TEST(CEntryExit2, GetEntryExitIndex_KnownNames) {
    const char* names[] = { "BOIGYM1", "AMMU1", "DINER1", "BAR1", "SFHQ" };
    for (const char* name : names) {
        int32 orig, rev;
        { HookDisableGuard guard("Global/CEntryExitManager/GetEntryExitIndex");
          orig = CEntryExitManager::GetEntryExitIndex(name, 0, 0); }
        rev = CEntryExitManager::GetEntryExitIndex(name, 0, 0);
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(CEntryExit2, GetEntryExitIndex_Unknown) {
    int32 orig, rev;
    { HookDisableGuard guard("Global/CEntryExitManager/GetEntryExitIndex");
      orig = CEntryExitManager::GetEntryExitIndex("NONEXISTENT", 0, 0); }
    rev = CEntryExitManager::GetEntryExitIndex("NONEXISTENT", 0, 0);
    EXPECT_EQ(orig, rev);
}
