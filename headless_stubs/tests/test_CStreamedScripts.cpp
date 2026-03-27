// test_CStreamedScripts.cpp — Differential tests for CStreamedScripts.
// Hook paths: Scripts/CStreamedScripts/<fn>
//
// Streamed scripts are loaded from SCM at startup.
// Tests use the script table that's populated by state 9.

#include "StdInc.h"
#include "TestFramework.h"
#include "StreamedScripts.h"
#include "TheScripts.h"

// --- FindStreamedScript / FindStreamedScriptQuiet ---

GAME_DIFF_TEST(CStreamedScripts, FindStreamedScriptQuiet_KnownScripts) {
    // Test with names that may or may not exist — both should agree
    const char* names[] = {
        "BURG1", "SWEET1", "RYDER1", "CRASH1", "SMOKE1",
        "CAT1", "ZERO1", "WUZI1", "TORENO1", "RIOT1",
        "NONEXISTENT_SCRIPT", "", "A",
    };
    for (const char* name : names) {
        int32 orig, rev;
        { HookDisableGuard guard("Scripts/CStreamedScripts/FindStreamedScriptQuiet");
          orig = CTheScripts::StreamedScripts.FindStreamedScriptQuiet(name); }
        rev = CTheScripts::StreamedScripts.FindStreamedScriptQuiet(name);
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(CStreamedScripts, FindStreamedScript_KnownScripts) {
    const char* names[] = {
        "BURG1", "SWEET1", "NONEXISTENT_SCRIPT", "",
    };
    for (const char* name : names) {
        int32 orig, rev;
        { HookDisableGuard guard("Scripts/CStreamedScripts/FindStreamedScript");
          orig = CTheScripts::StreamedScripts.FindStreamedScript(name); }
        rev = CTheScripts::StreamedScripts.FindStreamedScript(name);
        EXPECT_EQ(orig, rev);
    }
}

// --- GetProperIndexFromIndexUsedByScript ---

GAME_DIFF_TEST(CStreamedScripts, GetProperIndexFromIndexUsedByScript) {
    // Test a range of SCM indices
    // Only test indices that are likely valid (small values)
    int mismatches = 0;
    for (int16 idx = 0; idx < 20; idx++) {
        int16 orig, rev;
        { HookDisableGuard guard("Scripts/CStreamedScripts/GetProperIndexFromIndexUsedByScript");
          orig = CTheScripts::StreamedScripts.GetProperIndexFromIndexUsedByScript(idx); }
        rev = CTheScripts::StreamedScripts.GetProperIndexFromIndexUsedByScript(idx);
        if (orig != rev) mismatches++;
    }
    if (mismatches > 0) {
        char msg[128];
        _snprintf(msg, sizeof(msg), "GetProperIndexFromIndexUsedByScript: %d/20 indices differ", mismatches);
        GetTestContext().RecordFailure(__FILE__, __LINE__, msg);
    }
}
