// test_CEventVehicleThreat.cpp — Tests for CEventVehicleThreat.

#include "StdInc.h"
#include "TestFramework.h"
#include "Events/EventVehicleThreat.h"
#include "ScenarioHelpers.h"

static Scenario::TestVehicle* s_threatVeh = nullptr;
static bool s_triedThreatVeh = false;

static CVehicle* GetThreatTestVehicle() {
    if (!s_triedThreatVeh) {
        s_triedThreatVeh = true;
        int32 model = Scenario::FindLoadedVehicleModel();
        if (model >= 0)
            s_threatVeh = new Scenario::TestVehicle(model, CVector(2464.0f, -1668.0f, 13.5f));
    }
    return s_threatVeh ? s_threatVeh->AsVehicle() : nullptr;
}

#define THREAT_GUARD() if (!GetThreatTestVehicle()) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No vehicle"); return; }

GAME_TEST(CEventVehicleThreat, GetEventType) {
    THREAT_GUARD()
    CEventVehicleThreat event(GetThreatTestVehicle());
    EXPECT_EQ((int32)event.GetEventType(), (int32)EVENT_VEHICLE_THREAT);
}

GAME_TEST(CEventVehicleThreat, GetEventPriority) {
    THREAT_GUARD()
    CEventVehicleThreat event(GetThreatTestVehicle());
    EXPECT_EQ(event.GetEventPriority(), 19);
}

GAME_TEST(CEventVehicleThreat, CloneEditable_PreservesVehicle) {
    THREAT_GUARD()
    auto* veh = GetThreatTestVehicle();
    CEventVehicleThreat original(veh);
    auto* cloned = original.CloneEditable();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;
    EXPECT_EQ((int32)cloned->GetEventType(), (int32)EVENT_VEHICLE_THREAT);
    auto* c = static_cast<CEventVehicleThreat*>(cloned);
    EXPECT_EQ((uintptr_t)c->m_Vehicle, (uintptr_t)veh);
    delete cloned;
}
