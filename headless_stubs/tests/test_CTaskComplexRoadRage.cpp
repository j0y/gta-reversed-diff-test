// test_CTaskComplexRoadRage.cpp

#include "StdInc.h"
#include "TestFramework.h"
#include "Tasks/TaskTypes/TaskComplexRoadRage.h"
#include "Entity/Ped/Ped.h"
#include "ScenarioHelpers.h"

static Scenario::TestPed* s_ragePed = nullptr;
static bool s_triedRagePed = false;

static CPed* GetRageTestPed() {
    if (!s_triedRagePed) {
        s_triedRagePed = true;
        int32 model = Scenario::FindLoadedPedModel();
        if (model >= 0)
            s_ragePed = new Scenario::TestPed(PED_TYPE_CIVMALE, model, CVector(2434.0f, -1672.0f, 13.5f));
    }
    return s_ragePed ? s_ragePed->AsPed() : nullptr;
}

#define RAGE_GUARD() if (!GetRageTestPed()) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No ped"); return; }

GAME_TEST(CTaskComplexRoadRage, GetTaskType) {
    RAGE_GUARD()
    CTaskComplexRoadRage task(GetRageTestPed());
    EXPECT_EQ((int32)task.GetTaskType(), (int32)TASK_COMPLEX_ROAD_RAGE);
}

GAME_TEST(CTaskComplexRoadRage, IsComplex) {
    RAGE_GUARD()
    CTaskComplexRoadRage task(GetRageTestPed());
    EXPECT_TRUE(task.IsComplex());
}

GAME_TEST(CTaskComplexRoadRage, Constructor_Target) {
    RAGE_GUARD()
    auto* ped = GetRageTestPed();
    CTaskComplexRoadRage task(ped);
    EXPECT_EQ((uintptr_t)task.m_rageWith, (uintptr_t)ped);
}

GAME_TEST(CTaskComplexRoadRage, Clone_PreservesTarget) {
    RAGE_GUARD()
    auto* ped = GetRageTestPed();
    CTaskComplexRoadRage original(ped);
    CTask* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;
    EXPECT_EQ((int32)cloned->GetTaskType(), (int32)TASK_COMPLEX_ROAD_RAGE);
    auto* c = static_cast<CTaskComplexRoadRage*>(cloned);
    EXPECT_EQ((uintptr_t)c->m_rageWith, (uintptr_t)ped);
    delete cloned;
}
