// test_CTaskComplexArrestPed.cpp

#include "StdInc.h"
#include "TestFramework.h"
#include "Tasks/TaskTypes/TaskComplexArrestPed.h"
#include "Entity/Ped/Ped.h"
#include "ScenarioHelpers.h"

static Scenario::TestPed* s_arrestPed = nullptr;
static bool s_triedArrestPed = false;

static CPed* GetArrestTestPed() {
    if (!s_triedArrestPed) {
        s_triedArrestPed = true;
        int32 model = Scenario::FindLoadedPedModel();
        if (model >= 0)
            s_arrestPed = new Scenario::TestPed(PED_TYPE_CIVMALE, model, CVector(2446.0f, -1672.0f, 13.5f));
    }
    return s_arrestPed ? s_arrestPed->AsPed() : nullptr;
}

#define ARREST_GUARD() if (!GetArrestTestPed()) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No ped"); return; }

GAME_TEST(CTaskComplexArrestPed, GetTaskType) {
    ARREST_GUARD()
    CTaskComplexArrestPed task(GetArrestTestPed());
    EXPECT_EQ((int32)task.GetTaskType(), (int32)TASK_COMPLEX_ARREST_PED);
}

GAME_TEST(CTaskComplexArrestPed, IsComplex) {
    ARREST_GUARD()
    CTaskComplexArrestPed task(GetArrestTestPed());
    EXPECT_TRUE(task.IsComplex());
}

GAME_TEST(CTaskComplexArrestPed, Constructor_Target) {
    ARREST_GUARD()
    auto* ped = GetArrestTestPed();
    CTaskComplexArrestPed task(ped);
    EXPECT_EQ((uintptr_t)task.m_PedToArrest, (uintptr_t)ped);
}

GAME_TEST(CTaskComplexArrestPed, Clone_PreservesTarget) {
    ARREST_GUARD()
    auto* ped = GetArrestTestPed();
    CTaskComplexArrestPed original(ped);
    CTask* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;
    EXPECT_EQ((int32)cloned->GetTaskType(), (int32)TASK_COMPLEX_ARREST_PED);
    auto* c = static_cast<CTaskComplexArrestPed*>(cloned);
    EXPECT_EQ((uintptr_t)c->m_PedToArrest, (uintptr_t)ped);
    delete cloned;
}
