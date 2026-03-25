// test_CPedAttractorManager2.cpp — Differential tests for CPedAttractorManager.
// Hook paths: Global/CPedAttractorManager/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "PedAttractorManager.h"
#include "ScenarioHelpers.h"

// --- IsPedRegisteredWithEffect with spawned ped ---

GAME_DIFF_TEST(CPedAttractorManager2, IsPedRegisteredWithEffect_SpawnedPed) {
    int32 model = Scenario::FindLoadedPedModel();
    if (model < 0) return;
    Scenario::TestPed ped(PED_TYPE_CIVMALE, model, CVector(2450.f, -1665.f, 13.f));
    if (!ped) return;
    bool orig, rev;
    { HookDisableGuard guard("Global/CPedAttractorManager/IsPedRegisteredWithEffect-Ped");
      orig = GetPedAttractorManager()->IsPedRegisteredWithEffect(ped.AsPed()); }
    rev = GetPedAttractorManager()->IsPedRegisteredWithEffect(ped.AsPed());
    EXPECT_EQ(orig, rev);
}

// --- IsPedRegisteredWithEffect with player ped ---

GAME_DIFF_TEST(CPedAttractorManager2, IsPedRegisteredWithEffect_Player) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    bool orig, rev;
    { HookDisableGuard guard("Global/CPedAttractorManager/IsPedRegisteredWithEffect-Ped");
      orig = GetPedAttractorManager()->IsPedRegisteredWithEffect(player); }
    rev = GetPedAttractorManager()->IsPedRegisteredWithEffect(player);
    EXPECT_EQ(orig, rev);
}

// HasEmptySlot and HasQueueTailArrivedAtSlot crash with null args in original code — skip
