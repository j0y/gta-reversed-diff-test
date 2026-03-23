// test_CTaskComplexLeaveCarAndFlee.cpp

#include "StdInc.h"
#include "TestFramework.h"
#include "Tasks/TaskTypes/TaskComplexLeaveCarAndFlee.h"
#include "ScenarioHelpers.h"

static Scenario::TestVehicle* s_fleeVeh = nullptr;
static bool s_triedFleeVeh = false;

static CVehicle* GetFleeTestVehicle() {
    if (!s_triedFleeVeh) {
        s_triedFleeVeh = true;
        int32 model = Scenario::FindLoadedVehicleModel();
        if (model >= 0)
            s_fleeVeh = new Scenario::TestVehicle(model, CVector(2444.0f, -1668.0f, 13.5f));
    }
    return s_fleeVeh ? s_fleeVeh->AsVehicle() : nullptr;
}

#define FLEE_VEH_GUARD() if (!GetFleeTestVehicle()) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No vehicle"); return; }

GAME_TEST(CTaskComplexLeaveCarAndFlee, GetTaskType) {
    FLEE_VEH_GUARD()
    CVector fleePoint(2500.0f, -1660.0f, 13.5f);
    CTaskComplexLeaveCarAndFlee task(GetFleeTestVehicle(), fleePoint);
    EXPECT_EQ((int32)task.GetTaskType(), (int32)TASK_COMPLEX_LEAVE_CAR_AND_FLEE);
}

GAME_TEST(CTaskComplexLeaveCarAndFlee, IsComplex) {
    FLEE_VEH_GUARD()
    CVector fleePoint(0, 0, 0);
    CTaskComplexLeaveCarAndFlee task(GetFleeTestVehicle(), fleePoint);
    EXPECT_TRUE(task.IsComplex());
}

GAME_TEST(CTaskComplexLeaveCarAndFlee, Constructor_Fields) {
    FLEE_VEH_GUARD()
    auto* veh = GetFleeTestVehicle();
    CVector fleePoint(100.0f, 200.0f, 10.0f);
    CTaskComplexLeaveCarAndFlee task(veh, fleePoint, TARGET_DOOR_FRONT_RIGHT, 500, true);
    EXPECT_EQ((uintptr_t)task.m_veh, (uintptr_t)veh);
    EXPECT_NEAR(task.m_fleePoint.x, 100.0f, 1e-5f);
    EXPECT_NEAR(task.m_fleePoint.y, 200.0f, 1e-5f);
    EXPECT_EQ((int32)task.m_door, (int32)TARGET_DOOR_FRONT_RIGHT);
    EXPECT_EQ(task.m_fleeDelayMs, 500);
    EXPECT_TRUE(task.m_waitForCarToSlowDown);
}

GAME_TEST(CTaskComplexLeaveCarAndFlee, Clone_PreservesType) {
    FLEE_VEH_GUARD()
    CVector fleePoint(50.0f, 60.0f, 5.0f);
    CTaskComplexLeaveCarAndFlee original(GetFleeTestVehicle(), fleePoint);
    CTask* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;
    EXPECT_EQ((int32)cloned->GetTaskType(), (int32)TASK_COMPLEX_LEAVE_CAR_AND_FLEE);
    delete cloned;
}
