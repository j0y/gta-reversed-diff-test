// test_CTaskComplexLeaveCarAndWander.cpp

#include "StdInc.h"
#include "TestFramework.h"
#include "Tasks/TaskTypes/TaskComplexLeaveCarAndWander.h"
#include "ScenarioHelpers.h"

static Scenario::TestVehicle* s_wanderVeh = nullptr;
static bool s_triedWanderVeh = false;

static CVehicle* GetWanderLeaveVehicle() {
    if (!s_triedWanderVeh) {
        s_triedWanderVeh = true;
        int32 model = Scenario::FindLoadedVehicleModel();
        if (model >= 0)
            s_wanderVeh = new Scenario::TestVehicle(model, CVector(2440.0f, -1668.0f, 13.5f));
    }
    return s_wanderVeh ? s_wanderVeh->AsVehicle() : nullptr;
}

#define WANDER_VEH_GUARD() if (!GetWanderLeaveVehicle()) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No vehicle"); return; }

GAME_TEST(CTaskComplexLeaveCarAndWander, GetTaskType) {
    WANDER_VEH_GUARD()
    CTaskComplexLeaveCarAndWander task(GetWanderLeaveVehicle());
    EXPECT_EQ((int32)task.GetTaskType(), (int32)TASK_COMPLEX_LEAVE_CAR_AND_WANDER);
}

GAME_TEST(CTaskComplexLeaveCarAndWander, IsComplex) {
    WANDER_VEH_GUARD()
    CTaskComplexLeaveCarAndWander task(GetWanderLeaveVehicle());
    EXPECT_TRUE(task.IsComplex());
}

GAME_TEST(CTaskComplexLeaveCarAndWander, Constructor_Fields) {
    WANDER_VEH_GUARD()
    auto* veh = GetWanderLeaveVehicle();
    CTaskComplexLeaveCarAndWander task(veh, TARGET_DOOR_FRONT_RIGHT, 500, false);
    EXPECT_EQ((uintptr_t)task.m_veh, (uintptr_t)veh);
    EXPECT_EQ((int32)task.m_door, (int32)TARGET_DOOR_FRONT_RIGHT);
    EXPECT_EQ(task.m_leaveDelayMs, 500);
    EXPECT_FALSE(task.m_bSensibleLeaveCar);
}

GAME_TEST(CTaskComplexLeaveCarAndWander, Constructor_Defaults) {
    WANDER_VEH_GUARD()
    CTaskComplexLeaveCarAndWander task(GetWanderLeaveVehicle());
    EXPECT_EQ((int32)task.m_door, (int32)TARGET_DOOR_FRONT_LEFT);
    EXPECT_EQ(task.m_leaveDelayMs, 0);
    EXPECT_TRUE(task.m_bSensibleLeaveCar);
}

GAME_TEST(CTaskComplexLeaveCarAndWander, Clone_PreservesType) {
    WANDER_VEH_GUARD()
    CTaskComplexLeaveCarAndWander original(GetWanderLeaveVehicle(), TARGET_DOOR_REAR_LEFT, 1000);
    CTask* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;
    EXPECT_EQ((int32)cloned->GetTaskType(), (int32)TASK_COMPLEX_LEAVE_CAR_AND_WANDER);
    delete cloned;
}
