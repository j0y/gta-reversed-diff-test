// test_CEventLeanOnVehicle.cpp — Tests for CEventLeanOnVehicle.
// Triggered when a ped leans on a vehicle.

#include "StdInc.h"
#include "TestFramework.h"
#include "Events/EventLeanOnVehicle.h"
#include "ScenarioHelpers.h"

GAME_TEST(CEventLeanOnVehicle, GetEventType) {
    CEventLeanOnVehicle event(nullptr, 5000);
    EXPECT_EQ((int32)event.GetEventType(), (int32)EVENT_LEAN_ON_VEHICLE);
}

GAME_TEST(CEventLeanOnVehicle, GetEventPriority) {
    CEventLeanOnVehicle event(nullptr, 5000);
    EXPECT_EQ(event.GetEventPriority(), 12);
}

GAME_TEST(CEventLeanOnVehicle, GetLifeTime) {
    CEventLeanOnVehicle event(nullptr, 5000);
    EXPECT_EQ(event.GetLifeTime(), 0);
}

GAME_TEST(CEventLeanOnVehicle, AffectsPed_AlwaysTrue) {
    CEventLeanOnVehicle event(nullptr, 5000);
    EXPECT_TRUE(event.AffectsPed(nullptr));
}

GAME_TEST(CEventLeanOnVehicle, Constructor_Duration) {
    CEventLeanOnVehicle event(nullptr, 8000);
    EXPECT_EQ(event.m_leanAnimDurationInMs, 8000);
}

GAME_TEST(CEventLeanOnVehicle, Clone_PreservesDuration) {
    CEventLeanOnVehicle original(nullptr, 3000);
    CEvent* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;

    EXPECT_EQ((int32)cloned->GetEventType(), (int32)EVENT_LEAN_ON_VEHICLE);
    auto* c = static_cast<CEventLeanOnVehicle*>(cloned);
    EXPECT_EQ(c->m_leanAnimDurationInMs, 3000);

    delete cloned;
}

static Scenario::TestVehicle* s_leanVeh = nullptr;
static bool s_triedLeanVeh = false;

static CVehicle* GetLeanTestVehicle() {
    if (!s_triedLeanVeh) {
        s_triedLeanVeh = true;
        int32 model = Scenario::FindLoadedVehicleModel();
        if (model >= 0)
            s_leanVeh = new Scenario::TestVehicle(model, CVector(2474.0f, -1668.0f, 13.5f));
    }
    return s_leanVeh ? s_leanVeh->AsVehicle() : nullptr;
}

GAME_TEST(CEventLeanOnVehicle, Clone_WithVehicle) {
    auto* veh = GetLeanTestVehicle();
    if (!veh) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No vehicle"); return; }

    CEventLeanOnVehicle original(veh, 6000);
    CEvent* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;

    auto* c = static_cast<CEventLeanOnVehicle*>(cloned);
    EXPECT_EQ((uintptr_t)c->m_vehicle, (uintptr_t)veh);
    EXPECT_EQ(c->m_leanAnimDurationInMs, 6000);

    delete cloned;
}
