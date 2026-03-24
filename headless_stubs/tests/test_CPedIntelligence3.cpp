// test_CPedIntelligence3.cpp — Extended differential tests for CPedIntelligence.
// Hook paths: Entity/Ped/CPedIntelligence/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "Entity/Ped/Ped.h"
#include "PedIntelligence.h"
#include "ScenarioHelpers.h"

// --- IsRespondingToEvent with typed enum ---

GAME_DIFF_TEST(CPedIntelligence3, IsRespondingToEvent_Player) {
    auto* player = FindPlayerPed(0);
    if (!player || !player->GetIntelligence()) return;
    // Test a few common event types
    eEventType events[] = { EVENT_NONE, EVENT_FIRE_NEARBY, EVENT_DAMAGE };
    for (auto evt : events) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CPedIntelligence/IsRespondingToEvent");
          orig = player->GetIntelligence()->IsRespondingToEvent(evt); }
        rev = player->GetIntelligence()->IsRespondingToEvent(evt);
        EXPECT_EQ(orig, rev);
    }
}

// --- Spawned NPC ---

static Scenario::TestPed* s_intNpc = nullptr;
static bool s_triedIntNpc = false;

static CPed* GetIntNpc() {
    if (!s_triedIntNpc) {
        s_triedIntNpc = true;
        int32 model = Scenario::FindLoadedPedModel();
        if (model >= 0)
            s_intNpc = new Scenario::TestPed(PED_TYPE_CIVMALE, model, CVector(2494.f, -1668.f, 13.5f));
    }
    return s_intNpc ? s_intNpc->AsPed() : nullptr;
}

GAME_DIFF_TEST(CPedIntelligence3, IsRespondingToEvent_NPC) {
    auto* ped = GetIntNpc();
    if (!ped || !ped->GetIntelligence()) return;
    eEventType events[] = { EVENT_NONE, EVENT_FIRE_NEARBY, EVENT_DAMAGE };
    for (auto evt : events) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CPedIntelligence/IsRespondingToEvent");
          orig = ped->GetIntelligence()->IsRespondingToEvent(evt); }
        rev = ped->GetIntelligence()->IsRespondingToEvent(evt);
        EXPECT_EQ(orig, rev);
    }
}
