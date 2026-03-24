// test_CPedAttractorManager.cpp — Differential tests for CPedAttractorManager.
// Hook paths: Global/CPedAttractorManager/<fn>
// HasEmptySlot/HasQueueTailArrivedAtSlot take args — skipped for now.

#include "StdInc.h"
#include "TestFramework.h"
#include "PedAttractorManager.h"
#include "Entity/Ped/Ped.h"

GAME_DIFF_TEST(CPedAttractorManager, IsPedRegisteredWithEffect_Ped) {
    auto* player = FindPlayerPed(0);
    auto* mgr = GetPedAttractorManager();
    if (!player || !mgr) return;
    bool orig, rev;
    { HookDisableGuard guard("Global/CPedAttractorManager/IsPedRegisteredWithEffect-Ped");
      orig = mgr->IsPedRegisteredWithEffect(player); }
    rev = mgr->IsPedRegisteredWithEffect(player);
    EXPECT_EQ(orig, rev);
}
