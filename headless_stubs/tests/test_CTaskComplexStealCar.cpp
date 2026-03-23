// test_CTaskComplexStealCar.cpp

#include "StdInc.h"
#include "TestFramework.h"
#include "Tasks/TaskTypes/TaskComplexStealCar.h"
#include "ScenarioHelpers.h"

static Scenario::TestVehicle* s_stealVeh = nullptr;
static bool s_triedStealVeh = false;

static CVehicle* GetStealTestVehicle() {
    if (!s_triedStealVeh) {
        s_triedStealVeh = true;
        int32 model = Scenario::FindLoadedVehicleModel();
        if (model >= 0)
            s_stealVeh = new Scenario::TestVehicle(model, CVector(2448.0f, -1668.0f, 13.5f));
    }
    return s_stealVeh ? s_stealVeh->AsVehicle() : nullptr;
}

#define STEAL_GUARD() if (!GetStealTestVehicle()) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No vehicle"); return; }

GAME_TEST(CTaskComplexStealCar, GetTaskType) {
    STEAL_GUARD()
    CTaskComplexStealCar task(GetStealTestVehicle());
    EXPECT_EQ((int32)task.GetTaskType(), (int32)TASK_COMPLEX_STEAL_CAR);
}

GAME_TEST(CTaskComplexStealCar, IsComplex) {
    STEAL_GUARD()
    CTaskComplexStealCar task(GetStealTestVehicle());
    EXPECT_TRUE(task.IsComplex());
}

GAME_TEST(CTaskComplexStealCar, Constructor_Vehicle) {
    STEAL_GUARD()
    auto* veh = GetStealTestVehicle();
    CTaskComplexStealCar task(veh);
    EXPECT_EQ((uintptr_t)task.m_veh, (uintptr_t)veh);
}

GAME_TEST(CTaskComplexStealCar, Clone_PreservesVehicle) {
    STEAL_GUARD()
    auto* veh = GetStealTestVehicle();
    CTaskComplexStealCar original(veh);
    CTask* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;
    EXPECT_EQ((int32)cloned->GetTaskType(), (int32)TASK_COMPLEX_STEAL_CAR);
    auto* c = static_cast<CTaskComplexStealCar*>(cloned);
    EXPECT_EQ((uintptr_t)c->m_veh, (uintptr_t)veh);
    delete cloned;
}
