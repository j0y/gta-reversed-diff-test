// test_CStreamedScripts.cpp — Differential tests for CStreamedScripts.
// Hook paths: Scripts/CStreamedScripts/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "StreamedScripts.h"

// --- FindStreamedScript ---

GAME_DIFF_TEST(CStreamedScripts, FindStreamedScript) {
    const char* names[] = {
        "burg1", "drugs1", "heist1", "steal1", "ryder1",
        "sweet1", "cat1", "wuzi1", "zero1", "burg2",
        "NONEXISTENT_SCRIPT_XYZ"
    };
    for (const char* name : names) {
        int32 orig, rev;
        { HookDisableGuard guard("Scripts/CStreamedScripts/FindStreamedScript");
          orig = CTheScripts::StreamedScripts.FindStreamedScript(name); }
        rev = CTheScripts::StreamedScripts.FindStreamedScript(name);
        EXPECT_EQ(orig, rev);
    }
}

// --- FindStreamedScriptQuiet ---

GAME_DIFF_TEST(CStreamedScripts, FindStreamedScriptQuiet) {
    const char* names[] = {
        "burg1", "drugs1", "NONEXISTENT_SCRIPT_XYZ", ""
    };
    for (const char* name : names) {
        int32 orig, rev;
        { HookDisableGuard guard("Scripts/CStreamedScripts/FindStreamedScriptQuiet");
          orig = CTheScripts::StreamedScripts.FindStreamedScriptQuiet(name); }
        rev = CTheScripts::StreamedScripts.FindStreamedScriptQuiet(name);
        EXPECT_EQ(orig, rev);
    }
}

// --- GetProperIndexFromIndexUsedByScript ---

GAME_DIFF_TEST(CStreamedScripts, GetProperIndexFromIndexUsedByScript) {
    int16 indices[] = { 0, 1, 2, 5, 10, 50, 100, -1 };
    for (int16 idx : indices) {
        int16 orig, rev;
        { HookDisableGuard guard("Scripts/CStreamedScripts/GetProperIndexFromIndexUsedByScript");
          orig = CTheScripts::StreamedScripts.GetProperIndexFromIndexUsedByScript(idx); }
        rev = CTheScripts::StreamedScripts.GetProperIndexFromIndexUsedByScript(idx);
        EXPECT_EQ(orig, rev);
    }
}

// --- GetStreamedScriptFilename ---

GAME_DIFF_TEST(CStreamedScripts, GetStreamedScriptFilename) {
    // Test first few valid indices
    for (uint16 i = 0; i < 10; i++) {
        const char* orig;
        const char* rev;
        { HookDisableGuard guard("Scripts/CStreamedScripts/GetStreamedScriptFilename");
          orig = CTheScripts::StreamedScripts.GetStreamedScriptFilename(i); }
        rev = CTheScripts::StreamedScripts.GetStreamedScriptFilename(i);
        if (orig && rev) {
            EXPECT_EQ(strcmp(orig, rev), 0);
        } else {
            EXPECT_EQ(orig, rev); // both null
        }
    }
}
