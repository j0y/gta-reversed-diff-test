// test_CEntryExitManager.cpp — Differential tests for CEntryExitManager.
// Hook paths: Global/CEntryExitManager/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "EntryExitManager.h"

GAME_DIFF_TEST(CEntryExitManager, FindNearestEntryExit) {
    // Test positions across the map
    struct { CVector2D pos; float range; int32 ignoreArea; } tests[] = {
        { { 2488.0f, -1666.0f }, 50.0f, -1 },   // LS downtown
        { { 0.0f, 0.0f }, 100.0f, -1 },           // origin
        { { -2000.0f, 500.0f }, 50.0f, -1 },      // SF
        { { 2488.0f, -1666.0f }, 10.0f, 0 },      // LS, ignore area 0
    };
    for (auto& t : tests) {
        int32 orig, rev;
        { HookDisableGuard guard("Global/CEntryExitManager/FindNearestEntryExit");
          orig = CEntryExitManager::FindNearestEntryExit(t.pos, t.range, t.ignoreArea); }
        rev = CEntryExitManager::FindNearestEntryExit(t.pos, t.range, t.ignoreArea);
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(CEntryExitManager, GetEntryExitIndex) {
    // Empty string with _strnicmp matches first entry — test with non-existent name instead
    int32 orig, rev;
    { HookDisableGuard guard("Global/CEntryExitManager/GetEntryExitIndex");
      orig = CEntryExitManager::GetEntryExitIndex("ZZZNONEXISTENT999", 0, 0); }
    rev = CEntryExitManager::GetEntryExitIndex("ZZZNONEXISTENT999", 0, 0);
    EXPECT_EQ(orig, rev);
}
