// test_CEventGunAimedAt.cpp — Tests for CEventGunAimedAt.

#include "StdInc.h"
#include "TestFramework.h"
#include "Events/EventGunAimedAt.h"
#include "Entity/Ped/Ped.h"
#include "ScenarioHelpers.h"

GAME_TEST(CEventGunAimedAt, GetEventType) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    CEventGunAimedAt event(player);
    EXPECT_EQ((int32)event.GetEventType(), (int32)EVENT_GUN_AIMED_AT);
}

GAME_TEST(CEventGunAimedAt, GetEventPriority) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    CEventGunAimedAt event(player);
    EXPECT_EQ(event.GetEventPriority(), 50);
}

GAME_TEST(CEventGunAimedAt, GetSourceEntity) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    CEventGunAimedAt event(player);
    EXPECT_EQ((uintptr_t)event.GetSourceEntity(), (uintptr_t)player);
}

GAME_TEST(CEventGunAimedAt, IsCriminalEvent_Player) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    CEventGunAimedAt event(player);
    // Player aiming at someone is a criminal event
    bool result = event.IsCriminalEvent();
    EXPECT_TRUE(result);
}

static Scenario::TestPed* s_aimerPed = nullptr;
static bool s_triedAimerPed = false;

static CPed* GetNonPlayerAimer() {
    if (!s_triedAimerPed) {
        s_triedAimerPed = true;
        int32 model = Scenario::FindLoadedPedModel();
        if (model >= 0)
            s_aimerPed = new Scenario::TestPed(PED_TYPE_CIVMALE, model, CVector(2462.0f, -1672.0f, 13.5f));
    }
    return s_aimerPed ? s_aimerPed->AsPed() : nullptr;
}

GAME_TEST(CEventGunAimedAt, IsCriminalEvent_NPC) {
    auto* npc = GetNonPlayerAimer();
    if (!npc) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No NPC"); return; }
    CEventGunAimedAt event(npc);
    // NPC aiming is not a criminal event
    EXPECT_FALSE(event.IsCriminalEvent());
}

GAME_TEST(CEventGunAimedAt, CloneEditable_PreservesPed) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    CEventGunAimedAt original(player);
    auto* cloned = original.CloneEditable();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;
    EXPECT_EQ((int32)cloned->GetEventType(), (int32)EVENT_GUN_AIMED_AT);
    auto* c = static_cast<CEventGunAimedAt*>(cloned);
    EXPECT_EQ((uintptr_t)c->m_AimedBy, (uintptr_t)player);
    delete cloned;
}
