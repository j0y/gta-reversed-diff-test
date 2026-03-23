// test_CTaskSimpleShakeFist.cpp

#include "StdInc.h"
#include "TestFramework.h"
#include "Tasks/TaskTypes/TaskSimpleShakeFist.h"

GAME_TEST(CTaskSimpleShakeFist, GetTaskType) {
    CTaskSimpleShakeFist task;
    EXPECT_EQ((int32)task.GetTaskType(), (int32)TASK_SIMPLE_SHAKE_FIST);
}

GAME_TEST(CTaskSimpleShakeFist, IsSimple) {
    CTaskSimpleShakeFist task;
    EXPECT_TRUE(task.IsSimple());
}

GAME_TEST(CTaskSimpleShakeFist, InitialState) {
    CTaskSimpleShakeFist task;
    EXPECT_FALSE(task.m_hasAnimFinished);
}

GAME_TEST(CTaskSimpleShakeFist, Clone) {
    CTaskSimpleShakeFist original;
    CTask* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;
    EXPECT_EQ((int32)cloned->GetTaskType(), (int32)TASK_SIMPLE_SHAKE_FIST);
    delete cloned;
}
