// test_CTaskComplexKillPedOnFoot.cpp — Tests for CTaskComplexKillPedOnFoot.
// Hook paths: Tasks/TaskTypes/CTaskComplexKillPedOnFoot/<fn>
//
// Combat task — makes a ped kill a target on foot. Tests verify
// construction, cloning, and flag management.

#include "StdInc.h"
#include "TestFramework.h"
#include "Tasks/TaskTypes/TaskComplexKillPedOnFoot.h"
#include "Entity/Ped/Ped.h"
#include "ScenarioHelpers.h"

static Scenario::TestPed* s_killTarget = nullptr;
static bool s_triedKillTarget = false;

static CPed* GetKillTarget() {
    if (!s_triedKillTarget) {
        s_triedKillTarget = true;
        int32 model = Scenario::FindLoadedPedModel();
        if (model >= 0)
            s_killTarget = new Scenario::TestPed(PED_TYPE_CIVMALE, model, CVector(2486.0f, -1672.0f, 13.5f));
    }
    return s_killTarget ? s_killTarget->AsPed() : nullptr;
}

GAME_TEST(CTaskComplexKillPedOnFoot, GetTaskType) {
    auto* target = GetKillTarget();
    if (!target) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No target ped"); return; }
    CTaskComplexKillPedOnFoot task(target);
    EXPECT_EQ((int32)task.GetTaskType(), (int32)TASK_COMPLEX_KILL_PED_ON_FOOT);
}

GAME_TEST(CTaskComplexKillPedOnFoot, IsComplex) {
    auto* target = GetKillTarget();
    if (!target) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No target ped"); return; }
    CTaskComplexKillPedOnFoot task(target);
    EXPECT_TRUE(task.IsComplex());
}

GAME_TEST(CTaskComplexKillPedOnFoot, Constructor_Defaults) {
    auto* target = GetKillTarget();
    if (!target) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No target ped"); return; }
    CTaskComplexKillPedOnFoot task(target);
    EXPECT_EQ((uintptr_t)task.m_target, (uintptr_t)target);
    EXPECT_EQ(task.m_time, -1);
    EXPECT_EQ(task.m_pedFlags, 0);
    EXPECT_EQ(task.m_actionDelay, 0);
    EXPECT_EQ(task.m_actionChance, 0);
    EXPECT_EQ(task.m_nCompetence, (uint8)1);
}

GAME_TEST(CTaskComplexKillPedOnFoot, Constructor_CustomParams) {
    auto* target = GetKillTarget();
    if (!target) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No target ped"); return; }
    CTaskComplexKillPedOnFoot task(target, 5000, 3, 100, 50, 5);
    EXPECT_EQ(task.m_time, 5000);
    EXPECT_EQ(task.m_pedFlags, 3);
    EXPECT_EQ(task.m_actionDelay, 100);
    EXPECT_EQ(task.m_actionChance, 50);
    EXPECT_EQ(task.m_nCompetence, (uint8)5);
}

GAME_TEST(CTaskComplexKillPedOnFoot, Clone_PreservesTarget) {
    auto* target = GetKillTarget();
    if (!target) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No target ped"); return; }
    CTaskComplexKillPedOnFoot original(target, 3000, 1, 200, 75, 3);
    CTask* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;

    EXPECT_EQ((int32)cloned->GetTaskType(), (int32)TASK_COMPLEX_KILL_PED_ON_FOOT);
    auto* c = static_cast<CTaskComplexKillPedOnFoot*>(cloned);
    EXPECT_EQ((uintptr_t)c->m_target, (uintptr_t)target);
    EXPECT_EQ(c->m_time, 3000);
    EXPECT_EQ(c->m_nCompetence, (uint8)3);

    delete cloned;
}

GAME_TEST(CTaskComplexKillPedOnFoot, DefaultFlags) {
    auto* target = GetKillTarget();
    if (!target) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No target ped"); return; }
    CTaskComplexKillPedOnFoot task(target);
    EXPECT_TRUE(task.m_bShotFiredByPlayerFlag);
    EXPECT_TRUE(task.m_bNewTarget);
    EXPECT_TRUE(task.m_bRoomToDragPedOutOfCar);
    EXPECT_FALSE(task.m_bTargetKilled);
    EXPECT_FALSE(task.m_bAimImmediate);
}
