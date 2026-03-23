// test_CTaskComplexEnterBoatAsDriver.cpp

#include "StdInc.h"
#include "TestFramework.h"
#include "Tasks/TaskTypes/TaskComplexEnterBoatAsDriver.h"
#include "ScenarioHelpers.h"

static Scenario::TestVehicle* s_boatEnterVeh = nullptr;
static bool s_triedBoatEnterVeh = false;

static CVehicle* GetBoatEnterVehicle() {
    if (!s_triedBoatEnterVeh) {
        s_triedBoatEnterVeh = true;
        int32 model = Scenario::FindLoadedVehicleModel();
        if (model >= 0)
            s_boatEnterVeh = new Scenario::TestVehicle(model, CVector(2418.0f, -1668.0f, 13.5f));
    }
    return s_boatEnterVeh ? s_boatEnterVeh->AsVehicle() : nullptr;
}

#define BOAT_ENTER_GUARD() if (!GetBoatEnterVehicle()) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No vehicle"); return; }

GAME_TEST(CTaskComplexEnterBoatAsDriver, GetTaskType) {
    BOAT_ENTER_GUARD()
    CTaskComplexEnterBoatAsDriver task(GetBoatEnterVehicle());
    EXPECT_EQ((int32)task.GetTaskType(), (int32)TASK_COMPLEX_ENTER_BOAT_AS_DRIVER);
}

GAME_TEST(CTaskComplexEnterBoatAsDriver, IsComplex) {
    BOAT_ENTER_GUARD()
    CTaskComplexEnterBoatAsDriver task(GetBoatEnterVehicle());
    EXPECT_TRUE(task.IsComplex());
}

GAME_TEST(CTaskComplexEnterBoatAsDriver, Clone_PreservesType) {
    BOAT_ENTER_GUARD()
    CTaskComplexEnterBoatAsDriver original(GetBoatEnterVehicle());
    CTask* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;
    EXPECT_EQ((int32)cloned->GetTaskType(), (int32)TASK_COMPLEX_ENTER_BOAT_AS_DRIVER);
    delete cloned;
}
