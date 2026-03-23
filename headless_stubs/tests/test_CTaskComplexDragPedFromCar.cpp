// test_CTaskComplexDragPedFromCar.cpp

#include "StdInc.h"
#include "TestFramework.h"
#include "Tasks/TaskTypes/TaskComplexDragPedFromCar.h"
#include "Entity/Ped/Ped.h"
#include "ScenarioHelpers.h"

static Scenario::TestPed* s_dragPed = nullptr;
static bool s_triedDragPed = false;

static CPed* GetDragTestPed() {
    if (!s_triedDragPed) {
        s_triedDragPed = true;
        int32 model = Scenario::FindLoadedPedModel();
        if (model >= 0)
            s_dragPed = new Scenario::TestPed(PED_TYPE_CIVMALE, model, CVector(2426.0f, -1672.0f, 13.5f));
    }
    return s_dragPed ? s_dragPed->AsPed() : nullptr;
}

#define DRAG_GUARD() if (!GetDragTestPed()) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No ped"); return; }

GAME_TEST(CTaskComplexDragPedFromCar, GetTaskType) {
    DRAG_GUARD()
    CTaskComplexDragPedFromCar task(GetDragTestPed());
    EXPECT_EQ((int32)task.GetTaskType(), (int32)TASK_COMPLEX_DRAG_PED_FROM_CAR);
}

GAME_TEST(CTaskComplexDragPedFromCar, IsComplex) {
    DRAG_GUARD()
    CTaskComplexDragPedFromCar task(GetDragTestPed());
    EXPECT_TRUE(task.IsComplex());
}

GAME_TEST(CTaskComplexDragPedFromCar, Constructor_Target) {
    DRAG_GUARD()
    auto* ped = GetDragTestPed();
    CTaskComplexDragPedFromCar task(ped);
    EXPECT_EQ((uintptr_t)task.m_Ped, (uintptr_t)ped);
}

GAME_TEST(CTaskComplexDragPedFromCar, Clone_PreservesTarget) {
    DRAG_GUARD()
    auto* ped = GetDragTestPed();
    CTaskComplexDragPedFromCar original(ped, 3000);
    CTask* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;
    EXPECT_EQ((int32)cloned->GetTaskType(), (int32)TASK_COMPLEX_DRAG_PED_FROM_CAR);
    auto* c = static_cast<CTaskComplexDragPedFromCar*>(cloned);
    EXPECT_EQ((uintptr_t)c->m_Ped, (uintptr_t)ped);
    delete cloned;
}
