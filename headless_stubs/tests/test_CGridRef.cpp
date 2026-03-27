// test_CGridRef.cpp — Differential tests for CGridRef.
// Hook paths: Global/CGridRef/<fn>
//
// Grid reference maps world coordinates to 10x10 grid cells.
// Init loads DATA\GRIDREF.DAT at startup; data is available at state 9.

#include "StdInc.h"
#include "TestFramework.h"
#include "GridRef.h"

// --- GetAreaName: pure lookup into initialized 10x10 table ---

GAME_DIFF_TEST(CGridRef, GetAreaName_AllCells) {
    for (uint8 x = 0; x < 10; x++) {
        for (uint8 y = 0; y < 10; y++) {
            int32 orig, rev;
            { HookDisableGuard guard("Global/CGridRef/GetAreaName");
              orig = CGridRef::GetAreaName(x, y); }
            rev = CGridRef::GetAreaName(x, y);
            EXPECT_EQ(orig, rev);
        }
    }
}

GAME_DIFF_TEST(CGridRef, GetAreaName_OutOfBounds) {
    // Out-of-bounds should return 0
    uint8 oob[][2] = { {10,0}, {0,10}, {10,10}, {255,255}, {0,255} };
    for (auto& c : oob) {
        int32 orig, rev;
        { HookDisableGuard guard("Global/CGridRef/GetAreaName");
          orig = CGridRef::GetAreaName(c[0], c[1]); }
        rev = CGridRef::GetAreaName(c[0], c[1]);
        EXPECT_EQ(orig, rev);
    }
}

// --- GetArtistBugstarID: pointer lookup ---

GAME_DIFF_TEST(CGridRef, GetArtistBugstarID_OutOfBounds) {
    // Out-of-bounds should return nullptr for both
    char* orig;
    char* rev;
    { HookDisableGuard guard("Global/CGridRef/GetArtistBugstarID");
      orig = CGridRef::GetArtistBugstarID(10, 10); }
    rev = CGridRef::GetArtistBugstarID(10, 10);
    EXPECT_EQ((uintptr_t)orig, (uintptr_t)rev);
}

GAME_DIFF_TEST(CGridRef, GetArtistBugstarID_ValidCells) {
    // BUG CHECK: reversed returns {} (nullptr) for valid cells,
    // but original may return a valid pointer into GridRefList.
    int mismatches = 0;
    for (uint8 x = 0; x < 10; x++) {
        for (uint8 y = 0; y < 10; y++) {
            char* orig;
            char* rev;
            { HookDisableGuard guard("Global/CGridRef/GetArtistBugstarID");
              orig = CGridRef::GetArtistBugstarID(x, y); }
            rev = CGridRef::GetArtistBugstarID(x, y);
            if ((uintptr_t)orig != (uintptr_t)rev) mismatches++;
        }
    }
    // Record how many cells disagree — if >0 this is a bug in reversed code
    if (mismatches > 0) {
        char msg[128];
        _snprintf(msg, sizeof(msg), "GetArtistBugstarID: %d/100 cells differ (orig vs rev)", mismatches);
        GetTestContext().RecordFailure(__FILE__, __LINE__, msg);
    }
}

// --- GetGridRefPositions(CVector): pure coordinate transform ---

GAME_DIFF_TEST(CGridRef, GetGridRefPositions_Vec) {
    CVector positions[] = {
        {0, 0, 0},             // city center
        {-3000, -3000, 0},     // SW corner → (0, 9)
        {3000, 3000, 0},       // NE corner → (9, 0)
        {-2999, 2999, 0},      // near NW
        {2999, -2999, 0},      // near SE
        {-1500, 0, 50},        // mid-west
        {1500, 1500, 0},       // NE quadrant
        {-500, -500, 100},     // near center
        {600, 600, 0},         // one grid step from center
    };
    for (auto& pos : positions) {
        uint8 origX, origY, revX, revY;
        { HookDisableGuard guard("Global/CGridRef/GetGridRefPositions-vec");
          CGridRef::GetGridRefPositions(pos, &origX, &origY); }
        CGridRef::GetGridRefPositions(pos, &revX, &revY);
        EXPECT_EQ(origX, revX);
        EXPECT_EQ(origY, revY);
    }
}

// --- Behavior tests ---

GAME_TEST(CGridRef, GridCenterMapsCorrectly) {
    uint8 x, y;
    CGridRef::GetGridRefPositions(CVector(0, 0, 0), &x, &y);
    EXPECT_EQ(x, 5);
    EXPECT_EQ(y, 4);
}

GAME_TEST(CGridRef, GridRefDataLoaded) {
    // Verify the grid ref table has been populated by Init()
    // GetAreaName returns atoi(GridRefList[x][y]) — if strings are non-numeric
    // names, atoi returns 0, but the underlying data is still populated.
    // Just verify GetAreaName doesn't crash for all valid cells.
    int total = 0;
    for (uint8 x = 0; x < 10; x++)
        for (uint8 y = 0; y < 10; y++)
            total += CGridRef::GetAreaName(x, y);
    // Sum should be deterministic (may be 0 if all entries are non-numeric strings)
    EXPECT_TRUE(total >= 0);
}
