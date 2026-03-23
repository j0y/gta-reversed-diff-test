// test_CEventCopCarBeingStolen.cpp — Tests for CEventCopCarBeingStolen.

#include "StdInc.h"
#include "TestFramework.h"
#include "Events/EventCopCarBeingStolen.h"
#include "Entity/Ped/Ped.h"
#include "ScenarioHelpers.h"

static Scenario::TestVehicle* s_copVeh = nullptr;
static bool s_triedCopVeh = false;

static CVehicle* GetCopCarTestVehicle() {
    if (!s_triedCopVeh) {
        s_triedCopVeh = true;
        int32 model = Scenario::FindLoadedVehicleModel();
        if (model >= 0)
            s_copVeh = new Scenario::TestVehicle(model, CVector(2466.0f, -1668.0f, 13.5f));
    }
    return s_copVeh ? s_copVeh->AsVehicle() : nullptr;
}

GAME_TEST(CEventCopCarBeingStolen, GetEventType) {
    auto* player = FindPlayerPed(0);
    auto* veh = GetCopCarTestVehicle();
    if (!player || !veh) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No ped or vehicle"); return; }
    CEventCopCarBeingStolen event(player, veh);
    EXPECT_EQ((int32)event.GetEventType(), (int32)EVENT_COP_CAR_BEING_STOLEN);
}

GAME_TEST(CEventCopCarBeingStolen, GetEventPriority) {
    auto* player = FindPlayerPed(0);
    auto* veh = GetCopCarTestVehicle();
    if (!player || !veh) return;
    CEventCopCarBeingStolen event(player, veh);
    EXPECT_EQ(event.GetEventPriority(), 38);
}

GAME_TEST(CEventCopCarBeingStolen, Clone_PreservesFields) {
    auto* player = FindPlayerPed(0);
    auto* veh = GetCopCarTestVehicle();
    if (!player || !veh) return;
    CEventCopCarBeingStolen original(player, veh);
    CEvent* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;
    EXPECT_EQ((int32)cloned->GetEventType(), (int32)EVENT_COP_CAR_BEING_STOLEN);
    auto* c = static_cast<CEventCopCarBeingStolen*>(cloned);
    EXPECT_EQ((uintptr_t)c->m_hijacker, (uintptr_t)player);
    EXPECT_EQ((uintptr_t)c->m_vehicle, (uintptr_t)veh);
    delete cloned;
}
