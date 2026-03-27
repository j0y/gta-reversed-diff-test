// test_CSpecialPlateHandler.cpp — Differential tests for CSpecialPlateHandler.
// Hook paths: Global/CSpecialPlateHandler/<fn>
//
// Simple array-based plate text storage (carGenId → plate text).
// Init, Find, Add, Remove operate on a fixed-size array.

#include "StdInc.h"
#include "TestFramework.h"
#include "SpecialPlateHandler.h"

// --- Init ---

GAME_DIFF_TEST(CSpecialPlateHandler, Init) {
    CSpecialPlateHandler orig{}, rev{};
    // Dirty the structs first
    memset(&orig, 0xAB, sizeof(orig));
    memset(&rev, 0xAB, sizeof(rev));
    { HookDisableGuard guard("Global/CSpecialPlateHandler/Init");
      orig.Init(); }
    rev.Init();
    EXPECT_EQ(orig.m_nCount, rev.m_nCount);
    EXPECT_EQ(orig.m_nCount, (uint32)0);
}

// --- Add + Find round-trip ---

GAME_DIFF_TEST(CSpecialPlateHandler, AddAndFind) {
    CSpecialPlateHandler orig{}, rev{};
    orig.Init();
    rev.Init();

    // Add entries via original
    { HookDisableGuard guard("Global/CSpecialPlateHandler/Add");
      orig.Add(100, "GROVE4"); }
    { HookDisableGuard guard("Global/CSpecialPlateHandler/Add");
      orig.Add(200, "BALLA1"); }

    // Add same via reversed
    rev.Add(100, "GROVE4");
    rev.Add(200, "BALLA1");

    EXPECT_EQ(orig.m_nCount, rev.m_nCount);

    // Find via both
    char origText[16], revText[16];
    { HookDisableGuard guard("Global/CSpecialPlateHandler/Find");
      orig.Find(100, origText); }
    rev.Find(100, revText);
    EXPECT_EQ(strcmp(origText, revText), 0);

    { HookDisableGuard guard("Global/CSpecialPlateHandler/Find");
      orig.Find(200, origText); }
    rev.Find(200, revText);
    EXPECT_EQ(strcmp(origText, revText), 0);

    // Find non-existent
    { HookDisableGuard guard("Global/CSpecialPlateHandler/Find");
      orig.Find(999, origText); }
    rev.Find(999, revText);
    EXPECT_EQ(origText[0], revText[0]);  // both should be '\0'
}

// --- Remove ---

GAME_DIFF_TEST(CSpecialPlateHandler, Remove) {
    CSpecialPlateHandler orig{}, rev{};
    orig.Init(); rev.Init();

    // Add 3 entries
    orig.Add(10, "AAA"); orig.Add(20, "BBB"); orig.Add(30, "CCC");
    rev.Add(10, "AAA");  rev.Add(20, "BBB");  rev.Add(30, "CCC");

    // Remove middle entry (index 1)
    { HookDisableGuard guard("Global/CSpecialPlateHandler/Remove");
      orig.Remove(1); }
    rev.Remove(1);

    EXPECT_EQ(orig.m_nCount, rev.m_nCount);
}
