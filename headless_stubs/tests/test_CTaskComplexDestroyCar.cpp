// test_CTaskComplexDestroyCar.cpp

#include "StdInc.h"
#include "TestFramework.h"
#include "Tasks/TaskTypes/TaskComplexDestroyCar.h"
#include "ScenarioHelpers.h"

static Scenario::TestVehicle* s_destroyVeh = nullptr;
static bool s_triedDestroyVeh = false;

static CVehicle* GetDestroyTestVehicle() {
    if (!s_triedDestroyVeh) {
        s_triedDestroyVeh = true;
        int32 model = Scenario::FindLoadedVehicleModel();
        if (model >= 0)
            s_destroyVeh = new Scenario::TestVehicle(model, CVector(2450.0f, -1668.0f, 13.5f));
    }
    return s_destroyVeh ? s_destroyVeh->AsVehicle() : nullptr;
}

#define DESTROY_GUARD() if (!GetDestroyTestVehicle()) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No vehicle"); return; }

GAME_TEST(CTaskComplexDestroyCar, GetTaskType) {
    DESTROY_GUARD()
    CTaskComplexDestroyCar task(GetDestroyTestVehicle());
    EXPECT_EQ((int32)task.GetTaskType(), (int32)TASK_COMPLEX_DESTROY_CAR);
}

GAME_TEST(CTaskComplexDestroyCar, IsComplex) {
    DESTROY_GUARD()
    CTaskComplexDestroyCar task(GetDestroyTestVehicle());
    EXPECT_TRUE(task.IsComplex());
}

GAME_TEST(CTaskComplexDestroyCar, Constructor_Vehicle) {
    DESTROY_GUARD()
    auto* veh = GetDestroyTestVehicle();
    CTaskComplexDestroyCar task(veh);
    EXPECT_EQ((uintptr_t)task.m_VehicleToDestroy, (uintptr_t)veh);
}

GAME_TEST(CTaskComplexDestroyCar, Clone_PreservesVehicle) {
    DESTROY_GUARD()
    auto* veh = GetDestroyTestVehicle();
    CTaskComplexDestroyCar original(veh);
    CTask* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;
    EXPECT_EQ((int32)cloned->GetTaskType(), (int32)TASK_COMPLEX_DESTROY_CAR);
    auto* c = static_cast<CTaskComplexDestroyCar*>(cloned);
    EXPECT_EQ((uintptr_t)c->m_VehicleToDestroy, (uintptr_t)veh);
    delete cloned;
}
