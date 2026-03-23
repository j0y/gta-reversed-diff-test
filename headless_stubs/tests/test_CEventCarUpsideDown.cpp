// test_CEventCarUpsideDown.cpp — Tests for CEventCarUpsideDown.

#include "StdInc.h"
#include "TestFramework.h"
#include "Events/EventCarUpsideDown.h"
#include "ScenarioHelpers.h"

static Scenario::TestVehicle* s_usdVeh = nullptr;
static bool s_triedUsdVeh = false;

static CVehicle* GetUpsideDownTestVehicle() {
    if (!s_triedUsdVeh) {
        s_triedUsdVeh = true;
        int32 model = Scenario::FindLoadedVehicleModel();
        if (model >= 0)
            s_usdVeh = new Scenario::TestVehicle(model, CVector(2472.0f, -1668.0f, 13.5f));
    }
    return s_usdVeh ? s_usdVeh->AsVehicle() : nullptr;
}

#define USD_GUARD() if (!GetUpsideDownTestVehicle()) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No vehicle"); return; }

GAME_TEST(CEventCarUpsideDown, GetEventType) {
    USD_GUARD()
    CEventCarUpsideDown event(GetUpsideDownTestVehicle());
    EXPECT_EQ((int32)event.GetEventType(), (int32)EVENT_CAR_UPSIDE_DOWN);
}

GAME_TEST(CEventCarUpsideDown, GetEventPriority) {
    USD_GUARD()
    CEventCarUpsideDown event(GetUpsideDownTestVehicle());
    EXPECT_EQ(event.GetEventPriority(), 18);
}

GAME_TEST(CEventCarUpsideDown, GetLifeTime) {
    USD_GUARD()
    CEventCarUpsideDown event(GetUpsideDownTestVehicle());
    EXPECT_EQ(event.GetLifeTime(), 0);
}

GAME_TEST(CEventCarUpsideDown, Clone_PreservesVehicle) {
    USD_GUARD()
    auto* veh = GetUpsideDownTestVehicle();
    CEventCarUpsideDown original(veh);
    CEvent* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;

    EXPECT_EQ((int32)cloned->GetEventType(), (int32)EVENT_CAR_UPSIDE_DOWN);
    auto* c = static_cast<CEventCarUpsideDown*>(cloned);
    EXPECT_EQ((uintptr_t)c->m_vehicle, (uintptr_t)veh);

    delete cloned;
}
