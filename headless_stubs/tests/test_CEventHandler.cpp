// test_CEventHandler.cpp — Differential tests for CEventHandler.
// Hook paths: Events/CEventHandler/<fn>
// Tests on player ped and spawned ped event handlers.
//
// The event handler drives all ped reactions — combat, fleeing, entering
// vehicles. Bugs here cause NPCs to ignore threats or behave incorrectly.
//
// Inspired by gta-reversed issues:
//   #888 — Aggressive pedestrians attack with weapons in the wrong way
//   #1180 — NPCs don't stand up after being hit

#include "StdInc.h"
#include "TestFramework.h"
#include "ScenarioHelpers.h"
#include "Events/EventHandler.h"
#include "PedIntelligence.h"
#include "Entity/Ped/Ped.h"

// --- Player ped event handler ---

GAME_DIFF_TEST(CEventHandler, GetCurrentEventType_Player) {
    auto* ped = FindPlayerPed(0);
    if (!ped) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No player ped"); return; }
    auto& handler = ped->GetEventHandler();
    int orig, rev;
    { HookDisableGuard guard("Events/CEventHandler/GetCurrentEventType");
      orig = (int)handler.GetCurrentEventType(); }
    rev = (int)handler.GetCurrentEventType();
    EXPECT_EQ(orig, rev);
}

// --- Spawned ped event handler ---

static Scenario::TestPed* s_evtPed = nullptr;
static bool s_triedEvtPed = false;

static CPed* GetEventTestPed() {
    if (!s_triedEvtPed) {
        s_triedEvtPed = true;
        int32 model = Scenario::FindLoadedPedModel();
        if (model >= 0) {
            s_evtPed = new Scenario::TestPed(PED_TYPE_CIVMALE, model, CVector(2490.0f, -1672.0f, 13.5f));
        }
    }
    return s_evtPed ? s_evtPed->AsPed() : nullptr;
}

GAME_DIFF_TEST(CEventHandler, GetCurrentEventType_SpawnedPed) {
    auto* ped = GetEventTestPed();
    if (!ped) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No spawned ped"); return; }
    auto& handler = ped->GetEventHandler();
    int orig, rev;
    { HookDisableGuard guard("Events/CEventHandler/GetCurrentEventType");
      orig = (int)handler.GetCurrentEventType(); }
    rev = (int)handler.GetCurrentEventType();
    EXPECT_EQ(orig, rev);
}
