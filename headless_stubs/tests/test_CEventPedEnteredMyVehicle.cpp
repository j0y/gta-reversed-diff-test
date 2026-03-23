// test_CEventPedEnteredMyVehicle.cpp

#include "StdInc.h"
#include "TestFramework.h"
#include "Events/EventPedEnteredMyVehicle.h"
#include "Entity/Ped/Ped.h"
#include "ScenarioHelpers.h"

static Scenario::TestVehicle* s_enteredVeh = nullptr;
static bool s_triedEnteredVeh = false;

static CVehicle* GetEnteredTestVehicle() {
    if (!s_triedEnteredVeh) {
        s_triedEnteredVeh = true;
        int32 model = Scenario::FindLoadedVehicleModel();
        if (model >= 0)
            s_enteredVeh = new Scenario::TestVehicle(model, CVector(2454.0f, -1668.0f, 13.5f));
    }
    return s_enteredVeh ? s_enteredVeh->AsVehicle() : nullptr;
}

GAME_TEST(CEventPedEnteredMyVehicle, GetEventType) {
    auto* player = FindPlayerPed(0);
    auto* veh = GetEnteredTestVehicle();
    if (!player || !veh) return;
    CEventPedEnteredMyVehicle event(player, veh, TARGET_DOOR_FRONT_LEFT);
    EXPECT_EQ((int32)event.GetEventType(), (int32)EVENT_PED_ENTERED_MY_VEHICLE);
}

GAME_TEST(CEventPedEnteredMyVehicle, GetEventPriority) {
    auto* player = FindPlayerPed(0);
    auto* veh = GetEnteredTestVehicle();
    if (!player || !veh) return;
    CEventPedEnteredMyVehicle event(player, veh, TARGET_DOOR_FRONT_LEFT);
    EXPECT_EQ(event.GetEventPriority(), 39);
}

GAME_TEST(CEventPedEnteredMyVehicle, GetLocalSoundLevel) {
    auto* player = FindPlayerPed(0);
    auto* veh = GetEnteredTestVehicle();
    if (!player || !veh) return;
    CEventPedEnteredMyVehicle event(player, veh, TARGET_DOOR_FRONT_LEFT);
    EXPECT_NEAR(event.GetLocalSoundLevel(), 60.0f, 1e-5f);
}

GAME_TEST(CEventPedEnteredMyVehicle, Constructor_Fields) {
    auto* player = FindPlayerPed(0);
    auto* veh = GetEnteredTestVehicle();
    if (!player || !veh) return;
    CEventPedEnteredMyVehicle event(player, veh, TARGET_DOOR_FRONT_RIGHT);
    EXPECT_EQ((uintptr_t)event.m_PedThatEntered, (uintptr_t)player);
    EXPECT_EQ((uintptr_t)event.m_Vehicle, (uintptr_t)veh);
    EXPECT_EQ((int32)event.m_TargetDoor, (int32)TARGET_DOOR_FRONT_RIGHT);
}

GAME_TEST(CEventPedEnteredMyVehicle, GetSourceEntity) {
    auto* player = FindPlayerPed(0);
    auto* veh = GetEnteredTestVehicle();
    if (!player || !veh) return;
    CEventPedEnteredMyVehicle event(player, veh, TARGET_DOOR_FRONT_LEFT);
    EXPECT_EQ((uintptr_t)event.GetSourceEntity(), (uintptr_t)player);
}
