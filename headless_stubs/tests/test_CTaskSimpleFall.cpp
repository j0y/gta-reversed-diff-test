// test_CTaskSimpleFall.cpp — Tests for CTaskSimpleFall.
// Hook paths: Tasks/TaskTypes/CTaskSimpleFall/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "Tasks/TaskTypes/TaskSimpleFall.h"

GAME_TEST(CTaskSimpleFall, GetTaskType) {
    CTaskSimpleFall task(ANIM_ID_KO_SHOT_FRONT_0, ANIM_GROUP_DEFAULT, 2000);
    EXPECT_EQ((int32)task.GetTaskType(), (int32)TASK_SIMPLE_FALL);
}

GAME_TEST(CTaskSimpleFall, IsSimple) {
    CTaskSimpleFall task(ANIM_ID_KO_SHOT_FRONT_0, ANIM_GROUP_DEFAULT, 2000);
    EXPECT_TRUE(task.IsSimple());
}

GAME_TEST(CTaskSimpleFall, Constructor_Fields) {
    CTaskSimpleFall task(ANIM_ID_KO_SHOT_FRONT_0, ANIM_GROUP_DEFAULT, 5000);
    EXPECT_EQ((int32)task.m_nAnimId, (int32)ANIM_ID_KO_SHOT_FRONT_0);
    EXPECT_EQ((int32)task.m_nAnimGroup, (int32)ANIM_GROUP_DEFAULT);
    EXPECT_EQ(task.m_nTotalDownTime, 5000);
    EXPECT_FALSE(task.IsFinished());
}

GAME_TEST(CTaskSimpleFall, Clone_PreservesFields) {
    CTaskSimpleFall original(ANIM_ID_KO_SHOT_FRONT_0, ANIM_GROUP_DEFAULT, 3000);
    CTask* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;

    EXPECT_EQ((int32)cloned->GetTaskType(), (int32)TASK_SIMPLE_FALL);
    auto* c = static_cast<CTaskSimpleFall*>(cloned);
    EXPECT_EQ((int32)c->m_nAnimId, (int32)ANIM_ID_KO_SHOT_FRONT_0);
    EXPECT_EQ(c->m_nTotalDownTime, 3000);
    EXPECT_FALSE(c->IsFinished());

    delete cloned;
}

GAME_TEST(CTaskSimpleFall, Clone_IsSeparateObject) {
    CTaskSimpleFall original(ANIM_ID_KO_SHOT_FRONT_0, ANIM_GROUP_DEFAULT, 1000);
    CTask* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    EXPECT_TRUE((uintptr_t)cloned != (uintptr_t)&original);
    delete cloned;
}

GAME_TEST(CTaskSimpleFall, DifferentDownTimes) {
    int32 times[] = { 0, 500, 2000, 5000, 10000 };
    for (int32 t : times) {
        CTaskSimpleFall task(ANIM_ID_KO_SHOT_FRONT_0, ANIM_GROUP_DEFAULT, t);
        EXPECT_EQ(task.m_nTotalDownTime, t);
    }
}
