// test_CWorld8.cpp — Extended CWorld differential tests.
// Hook paths: Global/CWorld/<fn>
// Tests world entity search functions.

#include "StdInc.h"
#include "TestFramework.h"
#include "World.h"
#include "ScenarioHelpers.h"

GAME_DIFF_TEST(CWorld8, FindPlayerSlotWithPedPointer) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    int32 orig, rev;
    // FindPlayerSlotWithPedPointer searches player slots for a matching ped
    { HookDisableGuard guard("Global/CWorld/FindPlayerSlotWithPedPointer");
      orig = CWorld::FindPlayerSlotWithPedPointer((void*)player); }
    rev = CWorld::FindPlayerSlotWithPedPointer((void*)player);
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CWorld8, FindPlayerSlotWithPedPointer_NoMatch) {
    // Null pointer should not match any slot
    int32 orig, rev;
    { HookDisableGuard guard("Global/CWorld/FindPlayerSlotWithPedPointer");
      orig = CWorld::FindPlayerSlotWithPedPointer(nullptr); }
    rev = CWorld::FindPlayerSlotWithPedPointer(nullptr);
    EXPECT_EQ(orig, rev);
}

// BUG CANDIDATE: FindNearestObjectOfType returns different entities for same input.
// Could be a tie-breaking difference when multiple entities are equidistant.
GAME_DIFF_TEST(CWorld8, FindNearestObjectOfType_GroveStreet) {
    CVector pos(2488.f, -1666.f, 13.5f);
    CEntity* orig = nullptr;
    CEntity* rev = nullptr;
    { HookDisableGuard guard("Global/CWorld/FindNearestObjectOfType");
      orig = CWorld::FindNearestObjectOfType(-1, pos, 50.f, true, true, true, true, true, true); }
    rev = CWorld::FindNearestObjectOfType(-1, pos, 50.f, true, true, true, true, true, true);
    EXPECT_EQ(orig, rev);
}

// FindObjectsInRange — additional positions
GAME_DIFF_TEST(CWorld8, FindObjectsInRange_SF) {
    CVector pos(-2170.f, 250.f, 30.f);
    int16 origCount = 0, revCount = 0;
    CEntity* origEnts[32] = {};
    CEntity* revEnts[32] = {};
    { HookDisableGuard guard("Global/CWorld/FindObjectsInRange");
      CWorld::FindObjectsInRange(pos, 50.f, true, &origCount, 32, origEnts, false, true, true, false, false); }
    CWorld::FindObjectsInRange(pos, 50.f, true, &revCount, 32, revEnts, false, true, true, false, false);
    EXPECT_EQ(origCount, revCount);
}

GAME_DIFF_TEST(CWorld8, FindObjectsInRange_SmallRadius) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    CVector pos = player->GetPosition();
    int16 origCount = 0, revCount = 0;
    CEntity* origEnts[16] = {};
    CEntity* revEnts[16] = {};
    { HookDisableGuard guard("Global/CWorld/FindObjectsInRange");
      CWorld::FindObjectsInRange(pos, 5.f, true, &origCount, 16, origEnts, false, true, true, false, false); }
    CWorld::FindObjectsInRange(pos, 5.f, true, &revCount, 16, revEnts, false, true, true, false, false);
    EXPECT_EQ(origCount, revCount);
}
