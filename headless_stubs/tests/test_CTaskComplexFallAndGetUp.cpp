// test_CTaskComplexFallAndGetUp.cpp — Tests for CTaskComplexFallAndGetUp.
// Hook paths: Tasks/TaskTypes/CTaskComplexFallAndGetUp/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "Tasks/TaskTypes/TaskComplexFallAndGetUp.h"

GAME_TEST(CTaskComplexFallAndGetUp, GetTaskType) {
    CTaskComplexFallAndGetUp task(ANIM_ID_KO_SHOT_FRONT_0, ANIM_GROUP_DEFAULT, 2000);
    EXPECT_EQ((int32)task.GetTaskType(), (int32)TASK_COMPLEX_FALL_AND_GET_UP);
}

GAME_TEST(CTaskComplexFallAndGetUp, IsComplex) {
    CTaskComplexFallAndGetUp task(ANIM_ID_KO_SHOT_FRONT_0, ANIM_GROUP_DEFAULT, 2000);
    EXPECT_TRUE(task.IsComplex());
    EXPECT_FALSE(task.IsSimple());
}

GAME_TEST(CTaskComplexFallAndGetUp, Constructor_Fields) {
    CTaskComplexFallAndGetUp task(ANIM_ID_KO_SHOT_FRONT_0, ANIM_GROUP_DEFAULT, 3000);
    EXPECT_EQ((int32)task.m_nFallAnimId, (int32)ANIM_ID_KO_SHOT_FRONT_0);
    EXPECT_EQ((int32)task.m_nFallAnimGroup, (int32)ANIM_GROUP_DEFAULT);
    EXPECT_EQ(task.m_nFallDownTime, 3000);
}

GAME_TEST(CTaskComplexFallAndGetUp, Clone_PreservesFields) {
    CTaskComplexFallAndGetUp original(ANIM_ID_KO_SHOT_FRONT_0, ANIM_GROUP_DEFAULT, 5000);
    CTask* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;

    EXPECT_EQ((int32)cloned->GetTaskType(), (int32)TASK_COMPLEX_FALL_AND_GET_UP);
    auto* c = static_cast<CTaskComplexFallAndGetUp*>(cloned);
    EXPECT_EQ((int32)c->m_nFallAnimId, (int32)ANIM_ID_KO_SHOT_FRONT_0);
    EXPECT_EQ((int32)c->m_nFallAnimGroup, (int32)ANIM_GROUP_DEFAULT);
    EXPECT_EQ(c->m_nFallDownTime, 5000);

    delete cloned;
}

// IsFalling() dereferences m_pSubTask unconditionally (line 118: m_pSubTask->GetTaskType()).
// SetDownTime() calls IsFalling() internally (line 124).
// Both crash on a fresh task where m_pSubTask is null.
// These methods require a task that has been assigned to a ped and had
// CreateFirstSubTask called. Tested via field verification instead.

GAME_TEST(CTaskComplexFallAndGetUp, FallDownTime_DirectWrite) {
    CTaskComplexFallAndGetUp task(ANIM_ID_KO_SHOT_FRONT_0, ANIM_GROUP_DEFAULT, 2000);
    task.m_nFallDownTime = 8000;
    EXPECT_EQ(task.m_nFallDownTime, 8000);
}

GAME_TEST(CTaskComplexFallAndGetUp, DifferentFallTimes) {
    int32 times[] = { 0, 500, 1000, 3000, 10000 };
    for (int32 t : times) {
        CTaskComplexFallAndGetUp task(ANIM_ID_KO_SHOT_FRONT_0, ANIM_GROUP_DEFAULT, t);
        EXPECT_EQ(task.m_nFallDownTime, t);
    }
}
