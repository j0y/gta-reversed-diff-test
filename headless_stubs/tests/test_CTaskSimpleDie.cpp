// test_CTaskSimpleDie.cpp — Tests for CTaskSimpleDie.

#include "StdInc.h"
#include "TestFramework.h"
#include "Tasks/TaskTypes/TaskSimpleDie.h"

GAME_TEST(CTaskSimpleDie, GetTaskType) {
    CTaskSimpleDie task(ANIM_GROUP_DEFAULT, ANIM_ID_KO_SHOT_FRONT_0, 4.0f, 0.0f);
    EXPECT_EQ((int32)task.GetTaskType(), (int32)TASK_SIMPLE_DIE);
}

GAME_TEST(CTaskSimpleDie, IsSimple) {
    CTaskSimpleDie task(ANIM_GROUP_DEFAULT, ANIM_ID_KO_SHOT_FRONT_0, 4.0f, 0.0f);
    EXPECT_TRUE(task.IsSimple());
}

GAME_TEST(CTaskSimpleDie, Constructor_Fields) {
    CTaskSimpleDie task(ANIM_GROUP_DEFAULT, ANIM_ID_KO_SHOT_FRONT_0, 8.0f, 1.5f);
    EXPECT_EQ((int32)task.m_animGroupId, (int32)ANIM_GROUP_DEFAULT);
    EXPECT_EQ((int32)task.m_animId, (int32)ANIM_ID_KO_SHOT_FRONT_0);
    EXPECT_NEAR(task.m_blendDelta, 8.0f, 1e-5f);
    EXPECT_NEAR(task.m_animSpeed, 1.5f, 1e-5f);
}

GAME_TEST(CTaskSimpleDie, Clone_PreservesType) {
    CTaskSimpleDie original(ANIM_GROUP_DEFAULT, ANIM_ID_KO_SHOT_FRONT_0, 4.0f, 0.0f);
    CTask* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;
    EXPECT_EQ((int32)cloned->GetTaskType(), (int32)TASK_SIMPLE_DIE);
    delete cloned;
}
