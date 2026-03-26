// test_C3dMarkers.cpp — Differential tests for C3dMarkers.
// Hook paths: Global/C3dMarkers/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "3dMarkers.h"

// --- DirectionArrowFindFirstFreeSlot ---

GAME_DIFF_TEST(C3dMarkers, DirectionArrowFindFirstFreeSlot) {
    int32 orig, rev;
    { HookDisableGuard guard("Global/C3dMarkers/DirectionArrowFindFirstFreeSlot");
      orig = C3dMarkers::DirectionArrowFindFirstFreeSlot(); }
    rev = C3dMarkers::DirectionArrowFindFirstFreeSlot();
    EXPECT_EQ(orig, rev);
}

// --- User3dMarkerFindFirstFreeSlot ---

GAME_DIFF_TEST(C3dMarkers, User3dMarkerFindFirstFreeSlot) {
    int32 orig, rev;
    { HookDisableGuard guard("Global/C3dMarkers/User3dMarkerFindFirstFreeSlot");
      orig = C3dMarkers::User3dMarkerFindFirstFreeSlot(); }
    rev = C3dMarkers::User3dMarkerFindFirstFreeSlot();
    EXPECT_EQ(orig, rev);
}

// --- User3dMarkerSet + Delete round-trip ---

GAME_DIFF_TEST(C3dMarkers, User3dMarkerSetDelete) {
    // Set a marker via original, check slot
    int32 origSlot;
    { HookDisableGuard guard("Global/C3dMarkers/User3dMarkerSet");
      origSlot = C3dMarkers::User3dMarkerSet(100.f, 200.f, 10.f, HUD_COLOUR_RED); }

    // Delete it
    if (origSlot >= 0) {
        { HookDisableGuard guard("Global/C3dMarkers/User3dMarkerDelete");
          C3dMarkers::User3dMarkerDelete(origSlot); }
    }

    // Set via reversed
    int32 revSlot = C3dMarkers::User3dMarkerSet(100.f, 200.f, 10.f, HUD_COLOUR_RED);
    if (revSlot >= 0)
        C3dMarkers::User3dMarkerDelete(revSlot);

    // Both should find the same slot (first free)
    EXPECT_EQ(origSlot, revSlot);
}

// --- User3dMarkerDeleteAll ---

GAME_DIFF_TEST(C3dMarkers, User3dMarkerDeleteAll) {
    // Place a marker, delete all, check slot is free again
    int32 slot = C3dMarkers::User3dMarkerSet(50.f, 50.f, 5.f, HUD_COLOUR_BLUE);

    { HookDisableGuard guard("Global/C3dMarkers/User3dMarkerDeleteAll");
      C3dMarkers::User3dMarkerDeleteAll(); }
    int32 origFree;
    { HookDisableGuard guard("Global/C3dMarkers/User3dMarkerFindFirstFreeSlot");
      origFree = C3dMarkers::User3dMarkerFindFirstFreeSlot(); }

    // Place again and delete via reversed
    int32 slot2 = C3dMarkers::User3dMarkerSet(50.f, 50.f, 5.f, HUD_COLOUR_BLUE);
    C3dMarkers::User3dMarkerDeleteAll();
    int32 revFree = C3dMarkers::User3dMarkerFindFirstFreeSlot();

    EXPECT_EQ(origFree, revFree);
}

// --- NumActiveMarkers after operations ---

GAME_TEST(C3dMarkers, NumActiveMarkers_Sanity) {
    // At state 9, NumActiveMarkers should be reasonable
    EXPECT_TRUE(C3dMarkers::NumActiveMarkers < 100);
}
