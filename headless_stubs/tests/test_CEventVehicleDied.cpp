// test_CEventVehicleDied.cpp — Tests for CEventVehicleDied.
// Triggered when a vehicle is destroyed.

#include "StdInc.h"
#include "TestFramework.h"
#include "Events/EventVehicleDied.h"
#include "ScenarioHelpers.h"

// CEventVehicleDied constructor calls SafeRegisterRef — requires valid vehicle pointer.

static Scenario::TestVehicle* s_diedVeh = nullptr;
static bool s_triedDiedVeh = false;

static CVehicle* GetDiedTestVehicle() {
    if (!s_triedDiedVeh) {
        s_triedDiedVeh = true;
        int32 model = Scenario::FindLoadedVehicleModel();
        if (model >= 0)
            s_diedVeh = new Scenario::TestVehicle(model, CVector(2476.0f, -1668.0f, 13.5f));
    }
    return s_diedVeh ? s_diedVeh->AsVehicle() : nullptr;
}

#define DIED_GUARD() if (!GetDiedTestVehicle()) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No vehicle"); return; }

GAME_TEST(CEventVehicleDied, GetEventType) {
    DIED_GUARD()
    CEventVehicleDied event(GetDiedTestVehicle());
    EXPECT_EQ((int32)event.GetEventType(), (int32)EVENT_VEHICLE_DIED);
}

GAME_TEST(CEventVehicleDied, GetEventPriority) {
    DIED_GUARD()
    CEventVehicleDied event(GetDiedTestVehicle());
    EXPECT_EQ(event.GetEventPriority(), 72);
}

GAME_TEST(CEventVehicleDied, GetLifeTime) {
    DIED_GUARD()
    CEventVehicleDied event(GetDiedTestVehicle());
    EXPECT_EQ(event.GetLifeTime(), 0);
}

// Note: CEventVehicleDied(nullptr) crashes — SafeRegisterRef dereferences null.
// All tests require a valid vehicle.

GAME_TEST(CEventVehicleDied, Clone_IsSeparateObject) {
    DIED_GUARD()
    CEventVehicleDied original(GetDiedTestVehicle());
    CEvent* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    EXPECT_TRUE((uintptr_t)cloned != (uintptr_t)&original);
    delete cloned;
}

GAME_TEST(CEventVehicleDied, Clone_WithVehicle) {
    DIED_GUARD()
    auto* veh = GetDiedTestVehicle();
    CEventVehicleDied original(veh);
    CEvent* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;

    EXPECT_EQ((int32)cloned->GetEventType(), (int32)EVENT_VEHICLE_DIED);
    auto* c = static_cast<CEventVehicleDied*>(cloned);
    EXPECT_EQ((uintptr_t)c->m_vehicle, (uintptr_t)veh);
    delete cloned;
}
