// test_CTaskComplexLeaveBoat.cpp

#include "StdInc.h"
#include "TestFramework.h"
#include "Tasks/TaskTypes/TaskComplexLeaveBoat.h"
#include "ScenarioHelpers.h"

static Scenario::TestVehicle* s_leaveBoatVeh = nullptr;
static bool s_triedLeaveBoatVeh = false;

static CVehicle* GetLeaveBoatVehicle() {
    if (!s_triedLeaveBoatVeh) {
        s_triedLeaveBoatVeh = true;
        int32 model = Scenario::FindLoadedVehicleModel();
        if (model >= 0)
            s_leaveBoatVeh = new Scenario::TestVehicle(model, CVector(2442.0f, -1668.0f, 13.5f));
    }
    return s_leaveBoatVeh ? s_leaveBoatVeh->AsVehicle() : nullptr;
}

#define BOAT_GUARD() if (!GetLeaveBoatVehicle()) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No vehicle"); return; }

GAME_TEST(CTaskComplexLeaveBoat, GetTaskType) {
    BOAT_GUARD()
    CTaskComplexLeaveBoat task(GetLeaveBoatVehicle(), 0);
    EXPECT_EQ((int32)task.GetTaskType(), (int32)TASK_COMPLEX_LEAVE_BOAT);
}

GAME_TEST(CTaskComplexLeaveBoat, IsComplex) {
    BOAT_GUARD()
    CTaskComplexLeaveBoat task(GetLeaveBoatVehicle(), 0);
    EXPECT_TRUE(task.IsComplex());
}

GAME_TEST(CTaskComplexLeaveBoat, Constructor_Fields) {
    BOAT_GUARD()
    auto* veh = GetLeaveBoatVehicle();
    CTaskComplexLeaveBoat task(veh, 2000);
    EXPECT_EQ((uintptr_t)task.m_veh, (uintptr_t)veh);
    EXPECT_EQ(task.m_delayTimeMs, (uint32)2000);
}

GAME_TEST(CTaskComplexLeaveBoat, Clone_PreservesFields) {
    BOAT_GUARD()
    auto* veh = GetLeaveBoatVehicle();
    CTaskComplexLeaveBoat original(veh, 3000);
    CTask* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;
    EXPECT_EQ((int32)cloned->GetTaskType(), (int32)TASK_COMPLEX_LEAVE_BOAT);
    auto* c = static_cast<CTaskComplexLeaveBoat*>(cloned);
    EXPECT_EQ((uintptr_t)c->m_veh, (uintptr_t)veh);
    delete cloned;
}
