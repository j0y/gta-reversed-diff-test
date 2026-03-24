// test_InteriorManager.cpp — Differential tests for InteriorManager_c.
// Hook paths: Interior/InteriorManager_c/<fn>
//
// Tests interior manager queries. Most functions are safe in headless mode
// since they query data structures rather than rendering.

#include "StdInc.h"
#include "TestFramework.h"
#include "Interior/InteriorManager_c.h"
#include "Entity/Ped/Ped.h"

// --- IsGroupActive ---

GAME_DIFF_TEST(InteriorManager, IsGroupActive_Range) {
    for (int32 group = 0; group < 8; group++) {
        int8 orig, rev;
        { HookDisableGuard guard("Interior/InteriorManager_c/IsGroupActive");
          orig = g_interiorMan.IsGroupActive(group); }
        rev = g_interiorMan.IsGroupActive(group);
        EXPECT_EQ(orig, rev);
    }
}

// --- GetPedsInterior on player ---

GAME_DIFF_TEST(InteriorManager, GetPedsInterior_Player) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    Interior_c* orig;
    Interior_c* rev;
    { HookDisableGuard guard("Interior/InteriorManager_c/GetPedsInterior");
      orig = g_interiorMan.GetPedsInterior(player); }
    rev = g_interiorMan.GetPedsInterior(player);
    EXPECT_EQ(orig, rev);
}

// --- GetPedsInteriorGroup on player ---

GAME_DIFF_TEST(InteriorManager, GetPedsInteriorGroup_Player) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    InteriorGroup_c* orig;
    InteriorGroup_c* rev;
    { HookDisableGuard guard("Interior/InteriorManager_c/GetPedsInteriorGroup");
      orig = g_interiorMan.GetPedsInteriorGroup(player); }
    rev = g_interiorMan.GetPedsInteriorGroup(player);
    EXPECT_EQ(orig, rev);
}

// --- GetVectorsInterior ---

GAME_DIFF_TEST(InteriorManager, GetVectorsInterior_MultiplePositions) {
    CVector positions[] = {
        {2488.f, -1666.f, 13.f},    // Grove Street (exterior)
        {0.f, 0.f, 0.f},            // Origin
        {2495.f, -1665.f, 13.5f},   // Near player start
    };
    for (auto& pos : positions) {
        Interior_c* orig;
        Interior_c* rev;
        { HookDisableGuard guard("Interior/InteriorManager_c/GetVectorsInterior");
          orig = g_interiorMan.GetVectorsInterior(pos); }
        rev = g_interiorMan.GetVectorsInterior(pos);
        EXPECT_EQ(orig, rev);
    }
}

// --- GetInteriorFromPool (behavior — stateful, can't diff test) ---

GAME_TEST(InteriorManager, GetInteriorFromPool_Behavior) {
    Interior_c* result = g_interiorMan.GetInteriorFromPool();
    // Pool may be empty or have entries — just verify no crash
    // Return entry if we got one
    if (result) g_interiorMan.ReturnInteriorToPool(result);
    EXPECT_TRUE(true);
}
