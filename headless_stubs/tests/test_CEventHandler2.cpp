// test_CEventHandler2.cpp — Extended differential tests for CEventHandler.
// Hook paths: Events/CEventHandler/<fn>
// CEventHandler has 69 hooks — mostly ComputeEventResponseTask variants.

#include "StdInc.h"
#include "TestFramework.h"
#include "EventHandler.h"
#include "Entity/Ped/Ped.h"
#include "ScenarioHelpers.h"

// --- GetCurrentEventType ---

GAME_DIFF_TEST(CEventHandler2, GetCurrentEventType_Player) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    auto* handler = &player->GetEventHandler();
    int32 orig, rev;
    { HookDisableGuard guard("Events/CEventHandler/GetCurrentEventType");
      orig = (int32)handler->GetCurrentEventType(); }
    rev = (int32)handler->GetCurrentEventType();
    EXPECT_EQ(orig, rev);
}

// --- NPC ---

static Scenario::TestPed* s_eh2Npc = nullptr;
static bool s_triedEh2 = false;

static CPed* GetEh2Npc() {
    if (!s_triedEh2) {
        s_triedEh2 = true;
        int32 model = Scenario::FindLoadedPedModel();
        if (model >= 0)
            s_eh2Npc = new Scenario::TestPed(PED_TYPE_CIVMALE, model, CVector(2466.f, -1652.f, 13.5f));
    }
    return s_eh2Npc ? s_eh2Npc->AsPed() : nullptr;
}

GAME_DIFF_TEST(CEventHandler2, GetCurrentEventType_NPC) {
    auto* ped = GetEh2Npc();
    if (!ped) return;
    auto* handler = &ped->GetEventHandler();
    int32 orig, rev;
    { HookDisableGuard guard("Events/CEventHandler/GetCurrentEventType");
      orig = (int32)handler->GetCurrentEventType(); }
    rev = (int32)handler->GetCurrentEventType();
    EXPECT_EQ(orig, rev);
}
