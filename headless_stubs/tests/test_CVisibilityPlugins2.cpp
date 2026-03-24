// test_CVisibilityPlugins2.cpp — Extended differential tests.
// Hook paths: Global/CVisibilityPlugins/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "VisibilityPlugins.h"
#include "Entity/Ped/Ped.h"
#include "Entity/Vehicle/Vehicle.h"
#include "ScenarioHelpers.h"

// --- SetClumpAlpha / GetClumpAlpha round-trip ---

GAME_DIFF_TEST(CVisibilityPlugins2, SetGetClumpAlpha_Player) {
    auto* player = FindPlayerPed(0);
    if (!player || !player->m_pRwClump) return;

    int32 saved = CVisibilityPlugins::GetClumpAlpha(player->m_pRwClump);
    int32 testVal = 128;

    { HookDisableGuard guard("Global/CVisibilityPlugins/SetClumpAlpha");
      CVisibilityPlugins::SetClumpAlpha(player->m_pRwClump, testVal); }
    int32 origAlpha = CVisibilityPlugins::GetClumpAlpha(player->m_pRwClump);

    CVisibilityPlugins::SetClumpAlpha(player->m_pRwClump, saved);
    CVisibilityPlugins::SetClumpAlpha(player->m_pRwClump, testVal);
    int32 revAlpha = CVisibilityPlugins::GetClumpAlpha(player->m_pRwClump);

    EXPECT_EQ(origAlpha, revAlpha);
    CVisibilityPlugins::SetClumpAlpha(player->m_pRwClump, saved);
}

// --- GetFrameHierarchyId ---

GAME_DIFF_TEST(CVisibilityPlugins2, GetFrameHierarchyId_Player) {
    auto* player = FindPlayerPed(0);
    if (!player || !player->m_pRwClump) return;
    auto* frame = RpClumpGetFrame(player->m_pRwClump);
    if (!frame) return;
    int32 orig, rev;
    { HookDisableGuard guard("Global/CVisibilityPlugins/GetFrameHierarchyId");
      orig = CVisibilityPlugins::GetFrameHierarchyId(frame); }
    rev = CVisibilityPlugins::GetFrameHierarchyId(frame);
    EXPECT_EQ(orig, rev);
}

// --- SetFrameHierarchyId / GetFrameHierarchyId round-trip ---

GAME_DIFF_TEST(CVisibilityPlugins2, SetGetFrameHierarchyId_RoundTrip) {
    auto* player = FindPlayerPed(0);
    if (!player || !player->m_pRwClump) return;
    auto* frame = RpClumpGetFrame(player->m_pRwClump);
    if (!frame) return;

    int32 saved = CVisibilityPlugins::GetFrameHierarchyId(frame);
    int32 testVal = 99;

    { HookDisableGuard guard("Global/CVisibilityPlugins/SetFrameHierarchyId");
      CVisibilityPlugins::SetFrameHierarchyId(frame, testVal); }
    int32 origVal = CVisibilityPlugins::GetFrameHierarchyId(frame);

    CVisibilityPlugins::SetFrameHierarchyId(frame, saved);
    CVisibilityPlugins::SetFrameHierarchyId(frame, testVal);
    int32 revVal = CVisibilityPlugins::GetFrameHierarchyId(frame);

    EXPECT_EQ(origVal, revVal);
    CVisibilityPlugins::SetFrameHierarchyId(frame, saved);
}

// GetModelInfoIndex takes RpAtomic*, not RpClump*. Skipped.
