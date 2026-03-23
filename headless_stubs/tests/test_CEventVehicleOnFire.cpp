// test_CEventVehicleOnFire.cpp — Tests for CEventVehicleOnFire.

#include "StdInc.h"
#include "TestFramework.h"
#include "Events/EventVehicleOnFire.h"
#include "ScenarioHelpers.h"

static Scenario::TestVehicle* s_fireVeh = nullptr;
static bool s_triedFireVeh = false;

static CVehicle* GetFireTestVehicle() {
    if (!s_triedFireVeh) {
        s_triedFireVeh = true;
        int32 model = Scenario::FindLoadedVehicleModel();
        if (model >= 0)
            s_fireVeh = new Scenario::TestVehicle(model, CVector(2470.0f, -1668.0f, 13.5f));
    }
    return s_fireVeh ? s_fireVeh->AsVehicle() : nullptr;
}

#define FIRE_VEH_GUARD() if (!GetFireTestVehicle()) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No vehicle"); return; }

GAME_TEST(CEventVehicleOnFire, GetEventType) {
    FIRE_VEH_GUARD()
    CEventVehicleOnFire event(GetFireTestVehicle());
    EXPECT_EQ((int32)event.GetEventType(), (int32)EVENT_VEHICLE_ON_FIRE);
}

GAME_TEST(CEventVehicleOnFire, GetEventPriority) {
    FIRE_VEH_GUARD()
    CEventVehicleOnFire event(GetFireTestVehicle());
    EXPECT_EQ(event.GetEventPriority(), 54);
}

GAME_TEST(CEventVehicleOnFire, GetLocalSoundLevel) {
    FIRE_VEH_GUARD()
    CEventVehicleOnFire event(GetFireTestVehicle());
    EXPECT_NEAR(event.GetLocalSoundLevel(), 60.0f, 1e-5f);
}

GAME_TEST(CEventVehicleOnFire, CloneEditable_PreservesVehicle) {
    FIRE_VEH_GUARD()
    auto* veh = GetFireTestVehicle();
    CEventVehicleOnFire original(veh);
    auto* cloned = original.CloneEditable();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;
    EXPECT_EQ((int32)cloned->GetEventType(), (int32)EVENT_VEHICLE_ON_FIRE);
    auto* c = static_cast<CEventVehicleOnFire*>(cloned);
    EXPECT_EQ((uintptr_t)c->m_vehicle, (uintptr_t)veh);
    delete cloned;
}
