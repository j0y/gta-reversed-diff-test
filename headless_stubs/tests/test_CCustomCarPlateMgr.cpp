// test_CCustomCarPlateMgr.cpp — Differential tests for CCustomCarPlateMgr.
// Hook paths: Global/CCustomCarPlateMgr/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "CustomCarPlateMgr.h"

// --- GetMapRegionPlateDesign ---

GAME_DIFF_TEST(CCustomCarPlateMgr, GetMapRegionPlateDesign) {
    int8 orig, rev;
    { HookDisableGuard guard("Global/CCustomCarPlateMgr/GetMapRegionPlateDesign");
      orig = CCustomCarPlateMgr::GetMapRegionPlateDesign(); }
    rev = CCustomCarPlateMgr::GetMapRegionPlateDesign();
    EXPECT_EQ(orig, rev);
}

// --- GeneratePlateText ---
// Uses RNG internally, so seed before each call.

GAME_DIFF_TEST(CCustomCarPlateMgr, GeneratePlateText) {
    char origBuf[9] = {};
    char revBuf[9] = {};

    // Seed both exe and DLL CRTs — original inlines rand() to exe's CRT at 0x821B1E
    SeedBothRng(42);
    bool origOk;
    { HookDisableGuard guard("Global/CCustomCarPlateMgr/GeneratePlateText");
      origOk = CCustomCarPlateMgr::GeneratePlateText(origBuf, 8); }

    // Same seed, generate via reversed
    SeedBothRng(42);
    bool revOk = CCustomCarPlateMgr::GeneratePlateText(revBuf, 8);

    EXPECT_EQ(origOk, revOk);
    EXPECT_EQ(strcmp(origBuf, revBuf), 0);
}

GAME_DIFF_TEST(CCustomCarPlateMgr, GeneratePlateText_MultipleSeeds) {
    int seeds[] = { 0, 1, 42, 100, 999, 12345, 65535 };
    for (int seed : seeds) {
        char origBuf[9] = {};
        char revBuf[9] = {};

        SeedBothRng(seed);
        { HookDisableGuard guard("Global/CCustomCarPlateMgr/GeneratePlateText");
          CCustomCarPlateMgr::GeneratePlateText(origBuf, 8); }

        SeedBothRng(seed);
        CCustomCarPlateMgr::GeneratePlateText(revBuf, 8);

        EXPECT_EQ(strcmp(origBuf, revBuf), 0);
    }
}

// --- Behavior: plate text is reasonable ---

GAME_TEST(CCustomCarPlateMgr, GeneratePlateText_ValidChars) {
    srand(42);
    char buf[9] = {};
    CCustomCarPlateMgr::GeneratePlateText(buf, 8);

    // Should be non-empty
    EXPECT_TRUE(strlen(buf) > 0);
    // Should be printable ASCII
    for (int i = 0; buf[i]; i++) {
        EXPECT_TRUE(buf[i] >= ' ' && buf[i] <= '~');
    }
}

GAME_TEST(CCustomCarPlateMgr, GetMapRegionPlateDesign_InRange) {
    int8 design = CCustomCarPlateMgr::GetMapRegionPlateDesign();
    // Design index should be valid (0-2 for SA regions)
    EXPECT_TRUE(design >= 0);
    EXPECT_TRUE(design <= 5);
}
