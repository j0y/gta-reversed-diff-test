// test_CTaskComplexScreamInCarThenLeave.cpp

#include "StdInc.h"
#include "TestFramework.h"
#include "Tasks/TaskTypes/TaskComplexScreamInCarThenLeave.h"
#include "ScenarioHelpers.h"

static Scenario::TestVehicle* s_screamVeh = nullptr;
static bool s_triedScreamVeh = false;

static CVehicle* GetScreamTestVehicle() {
    if (!s_triedScreamVeh) {
        s_triedScreamVeh = true;
        int32 model = Scenario::FindLoadedVehicleModel();
        if (model >= 0)
            s_screamVeh = new Scenario::TestVehicle(model, CVector(2428.0f, -1668.0f, 13.5f));
    }
    return s_screamVeh ? s_screamVeh->AsVehicle() : nullptr;
}

#define SCREAM_GUARD() if (!GetScreamTestVehicle()) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No vehicle"); return; }

GAME_TEST(CTaskComplexScreamInCarThenLeave, GetTaskType) {
    SCREAM_GUARD()
    CTaskComplexScreamInCarThenLeave task(GetScreamTestVehicle(), TARGET_DOOR_FRONT_LEFT);
    EXPECT_EQ((int32)task.GetTaskType(), (int32)TASK_COMPLEX_SCREAM_IN_CAR_THEN_LEAVE);
}

GAME_TEST(CTaskComplexScreamInCarThenLeave, IsComplex) {
    SCREAM_GUARD()
    CTaskComplexScreamInCarThenLeave task(GetScreamTestVehicle(), TARGET_DOOR_FRONT_LEFT);
    EXPECT_TRUE(task.IsComplex());
}

GAME_TEST(CTaskComplexScreamInCarThenLeave, Constructor_Fields) {
    SCREAM_GUARD()
    auto* veh = GetScreamTestVehicle();
    CTaskComplexScreamInCarThenLeave task(veh, TARGET_DOOR_FRONT_RIGHT);
    EXPECT_EQ((uintptr_t)task.m_veh, (uintptr_t)veh);
    EXPECT_EQ((int32)task.m_door, (int32)TARGET_DOOR_FRONT_RIGHT);
}

GAME_TEST(CTaskComplexScreamInCarThenLeave, Clone_PreservesType) {
    SCREAM_GUARD()
    CTaskComplexScreamInCarThenLeave original(GetScreamTestVehicle(), TARGET_DOOR_FRONT_LEFT);
    CTask* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;
    EXPECT_EQ((int32)cloned->GetTaskType(), (int32)TASK_COMPLEX_SCREAM_IN_CAR_THEN_LEAVE);
    delete cloned;
}
