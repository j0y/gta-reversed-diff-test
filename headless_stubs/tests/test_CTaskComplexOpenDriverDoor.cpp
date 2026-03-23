// test_CTaskComplexOpenDriverDoor.cpp

#include "StdInc.h"
#include "TestFramework.h"
#include "Tasks/TaskTypes/TaskComplexOpenDriverDoor.h"
#include "ScenarioHelpers.h"

static Scenario::TestVehicle* s_doorVeh = nullptr;
static bool s_triedDoorVeh = false;

static CVehicle* GetDoorTestVehicle() {
    if (!s_triedDoorVeh) {
        s_triedDoorVeh = true;
        int32 model = Scenario::FindLoadedVehicleModel();
        if (model >= 0)
            s_doorVeh = new Scenario::TestVehicle(model, CVector(2422.0f, -1668.0f, 13.5f));
    }
    return s_doorVeh ? s_doorVeh->AsVehicle() : nullptr;
}

#define DOOR_GUARD() if (!GetDoorTestVehicle()) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No vehicle"); return; }

GAME_TEST(CTaskComplexOpenDriverDoor, GetTaskType) {
    DOOR_GUARD()
    CTaskComplexOpenDriverDoor task(GetDoorTestVehicle());
    EXPECT_EQ((int32)task.GetTaskType(), (int32)TASK_COMPLEX_CAR_OPEN_DRIVER_DOOR);
}

GAME_TEST(CTaskComplexOpenDriverDoor, IsComplex) {
    DOOR_GUARD()
    CTaskComplexOpenDriverDoor task(GetDoorTestVehicle());
    EXPECT_TRUE(task.IsComplex());
}

GAME_TEST(CTaskComplexOpenDriverDoor, Clone_PreservesType) {
    DOOR_GUARD()
    CTaskComplexOpenDriverDoor original(GetDoorTestVehicle());
    CTask* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;
    EXPECT_EQ((int32)cloned->GetTaskType(), (int32)TASK_COMPLEX_CAR_OPEN_DRIVER_DOOR);
    delete cloned;
}
