// test_CEventDraggedOutCar.cpp — Tests for CEventDraggedOutCar.

#include "StdInc.h"
#include "TestFramework.h"
#include "Events/EventDraggedOutCar.h"
#include "Entity/Ped/Ped.h"
#include "ScenarioHelpers.h"

static Scenario::TestVehicle* s_dragVeh = nullptr;
static bool s_triedDragVeh = false;

static CVehicle* GetDragTestVehicle() {
    if (!s_triedDragVeh) {
        s_triedDragVeh = true;
        int32 model = Scenario::FindLoadedVehicleModel();
        if (model >= 0)
            s_dragVeh = new Scenario::TestVehicle(model, CVector(2460.0f, -1668.0f, 13.5f));
    }
    return s_dragVeh ? s_dragVeh->AsVehicle() : nullptr;
}

GAME_TEST(CEventDraggedOutCar, GetEventType) {
    auto* player = FindPlayerPed(0);
    auto* veh = GetDragTestVehicle();
    if (!player || !veh) return;
    CEventDraggedOutCar event(veh, player, true);
    EXPECT_EQ((int32)event.GetEventType(), (int32)EVENT_DRAGGED_OUT_CAR);
}

GAME_TEST(CEventDraggedOutCar, GetEventPriority) {
    auto* player = FindPlayerPed(0);
    auto* veh = GetDragTestVehicle();
    if (!player || !veh) return;
    CEventDraggedOutCar event(veh, player, true);
    EXPECT_EQ(event.GetEventPriority(), 40);
}

GAME_TEST(CEventDraggedOutCar, GetLocalSoundLevel) {
    auto* player = FindPlayerPed(0);
    auto* veh = GetDragTestVehicle();
    if (!player || !veh) return;
    CEventDraggedOutCar event(veh, player, true);
    EXPECT_NEAR(event.GetLocalSoundLevel(), 100.0f, 1e-5f);
}

GAME_TEST(CEventDraggedOutCar, GetSourceEntity) {
    auto* player = FindPlayerPed(0);
    auto* veh = GetDragTestVehicle();
    if (!player || !veh) return;
    CEventDraggedOutCar event(veh, player, true);
    EXPECT_EQ((uintptr_t)event.GetSourceEntity(), (uintptr_t)player);
}

GAME_TEST(CEventDraggedOutCar, Constructor_DriverSeat) {
    auto* player = FindPlayerPed(0);
    auto* veh = GetDragTestVehicle();
    if (!player || !veh) return;
    CEventDraggedOutCar event(veh, player, true);
    EXPECT_TRUE(event.m_IsDriverSeat);
    EXPECT_EQ((uintptr_t)event.m_Vehicle, (uintptr_t)veh);
    EXPECT_EQ((uintptr_t)event.m_CarJacker, (uintptr_t)player);
}

GAME_TEST(CEventDraggedOutCar, Constructor_PassengerSeat) {
    auto* player = FindPlayerPed(0);
    auto* veh = GetDragTestVehicle();
    if (!player || !veh) return;
    CEventDraggedOutCar event(veh, player, false);
    EXPECT_FALSE(event.m_IsDriverSeat);
}

GAME_TEST(CEventDraggedOutCar, CloneEditable_PreservesFields) {
    auto* player = FindPlayerPed(0);
    auto* veh = GetDragTestVehicle();
    if (!player || !veh) return;
    CEventDraggedOutCar original(veh, player, true);
    auto* cloned = original.CloneEditable();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;
    EXPECT_EQ((int32)cloned->GetEventType(), (int32)EVENT_DRAGGED_OUT_CAR);
    auto* c = static_cast<CEventDraggedOutCar*>(cloned);
    EXPECT_EQ((uintptr_t)c->m_Vehicle, (uintptr_t)veh);
    EXPECT_EQ((uintptr_t)c->m_CarJacker, (uintptr_t)player);
    EXPECT_TRUE(c->m_IsDriverSeat);
    delete cloned;
}
