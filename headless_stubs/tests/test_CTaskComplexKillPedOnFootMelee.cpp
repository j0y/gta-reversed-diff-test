// test_CTaskComplexKillPedOnFootMelee.cpp

#include "StdInc.h"
#include "TestFramework.h"
#include "Tasks/TaskTypes/TaskComplexKillPedOnFootMelee.h"
#include "Entity/Ped/Ped.h"
#include "ScenarioHelpers.h"

static Scenario::TestPed* s_meleePed = nullptr;
static bool s_triedMeleePed = false;

static CPed* GetMeleeTestPed() {
    if (!s_triedMeleePed) {
        s_triedMeleePed = true;
        int32 model = Scenario::FindLoadedPedModel();
        if (model >= 0)
            s_meleePed = new Scenario::TestPed(PED_TYPE_CIVMALE, model, CVector(2432.0f, -1672.0f, 13.5f));
    }
    return s_meleePed ? s_meleePed->AsPed() : nullptr;
}

#define MELEE_GUARD() if (!GetMeleeTestPed()) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No ped"); return; }

GAME_TEST(CTaskComplexKillPedOnFootMelee, GetTaskType) {
    MELEE_GUARD()
    CTaskComplexKillPedOnFootMelee task(GetMeleeTestPed());
    EXPECT_EQ((int32)task.GetTaskType(), (int32)TASK_COMPLEX_KILL_PED_ON_FOOT_MELEE);
}

GAME_TEST(CTaskComplexKillPedOnFootMelee, IsComplex) {
    MELEE_GUARD()
    CTaskComplexKillPedOnFootMelee task(GetMeleeTestPed());
    EXPECT_TRUE(task.IsComplex());
}

GAME_TEST(CTaskComplexKillPedOnFootMelee, Constructor_Target) {
    MELEE_GUARD()
    auto* ped = GetMeleeTestPed();
    CTaskComplexKillPedOnFootMelee task(ped);
    EXPECT_EQ((uintptr_t)task.m_target, (uintptr_t)ped);
}

GAME_TEST(CTaskComplexKillPedOnFootMelee, Clone_PreservesTarget) {
    MELEE_GUARD()
    auto* ped = GetMeleeTestPed();
    CTaskComplexKillPedOnFootMelee original(ped);
    CTask* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;
    EXPECT_EQ((int32)cloned->GetTaskType(), (int32)TASK_COMPLEX_KILL_PED_ON_FOOT_MELEE);
    auto* c = static_cast<CTaskComplexKillPedOnFootMelee*>(cloned);
    EXPECT_EQ((uintptr_t)c->m_target, (uintptr_t)ped);
    delete cloned;
}
