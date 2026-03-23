// test_CTaskComplexDestroyCarMelee.cpp

#include "StdInc.h"
#include "TestFramework.h"
#include "Tasks/TaskTypes/TaskComplexDestroyCarMelee.h"
#include "ScenarioHelpers.h"

static Scenario::TestVehicle* s_meleeCarVeh = nullptr;
static bool s_triedMeleeCarVeh = false;

static CVehicle* GetMeleeCarVehicle() {
    if (!s_triedMeleeCarVeh) {
        s_triedMeleeCarVeh = true;
        int32 model = Scenario::FindLoadedVehicleModel();
        if (model >= 0)
            s_meleeCarVeh = new Scenario::TestVehicle(model, CVector(2430.0f, -1668.0f, 13.5f));
    }
    return s_meleeCarVeh ? s_meleeCarVeh->AsVehicle() : nullptr;
}

#define MELEE_CAR_GUARD() if (!GetMeleeCarVehicle()) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No vehicle"); return; }

GAME_TEST(CTaskComplexDestroyCarMelee, GetTaskType) {
    MELEE_CAR_GUARD()
    CTaskComplexDestroyCarMelee task(GetMeleeCarVehicle());
    EXPECT_EQ((int32)task.GetTaskType(), (int32)TASK_COMPLEX_DESTROY_CAR_MELEE);
}

GAME_TEST(CTaskComplexDestroyCarMelee, IsComplex) {
    MELEE_CAR_GUARD()
    CTaskComplexDestroyCarMelee task(GetMeleeCarVehicle());
    EXPECT_TRUE(task.IsComplex());
}

GAME_TEST(CTaskComplexDestroyCarMelee, Constructor_Vehicle) {
    MELEE_CAR_GUARD()
    auto* veh = GetMeleeCarVehicle();
    CTaskComplexDestroyCarMelee task(veh);
    EXPECT_EQ((uintptr_t)task.m_VehToDestroy, (uintptr_t)veh);
}

GAME_TEST(CTaskComplexDestroyCarMelee, Clone_PreservesVehicle) {
    MELEE_CAR_GUARD()
    auto* veh = GetMeleeCarVehicle();
    CTaskComplexDestroyCarMelee original(veh);
    CTask* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;
    EXPECT_EQ((int32)cloned->GetTaskType(), (int32)TASK_COMPLEX_DESTROY_CAR_MELEE);
    auto* c = static_cast<CTaskComplexDestroyCarMelee*>(cloned);
    EXPECT_EQ((uintptr_t)c->m_VehToDestroy, (uintptr_t)veh);
    delete cloned;
}
