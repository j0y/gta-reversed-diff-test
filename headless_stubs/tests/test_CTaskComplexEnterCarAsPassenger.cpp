// test_CTaskComplexEnterCarAsPassenger.cpp — Tests for CTaskComplexEnterCarAsPassenger.

#include "StdInc.h"
#include "TestFramework.h"
#include "Tasks/TaskTypes/TaskComplexEnterCarAsPassenger.h"
#include "ScenarioHelpers.h"

static Scenario::TestVehicle* s_passVeh = nullptr;
static bool s_triedPassVeh = false;

static CVehicle* GetPassengerTestVehicle() {
    if (!s_triedPassVeh) {
        s_triedPassVeh = true;
        int32 model = Scenario::FindLoadedVehicleModel();
        if (model >= 0)
            s_passVeh = new Scenario::TestVehicle(model, CVector(2458.0f, -1668.0f, 13.5f));
    }
    return s_passVeh ? s_passVeh->AsVehicle() : nullptr;
}

#define PASS_GUARD() if (!GetPassengerTestVehicle()) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No vehicle"); return; }

GAME_TEST(CTaskComplexEnterCarAsPassenger, GetTaskType) {
    PASS_GUARD()
    CTaskComplexEnterCarAsPassenger task(GetPassengerTestVehicle());
    EXPECT_EQ((int32)task.GetTaskType(), (int32)TASK_COMPLEX_ENTER_CAR_AS_PASSENGER);
}

GAME_TEST(CTaskComplexEnterCarAsPassenger, IsComplex) {
    PASS_GUARD()
    CTaskComplexEnterCarAsPassenger task(GetPassengerTestVehicle());
    EXPECT_TRUE(task.IsComplex());
}

GAME_TEST(CTaskComplexEnterCarAsPassenger, Clone_PreservesType) {
    PASS_GUARD()
    CTaskComplexEnterCarAsPassenger original(GetPassengerTestVehicle());
    CTask* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;
    EXPECT_EQ((int32)cloned->GetTaskType(), (int32)TASK_COMPLEX_ENTER_CAR_AS_PASSENGER);
    delete cloned;
}
