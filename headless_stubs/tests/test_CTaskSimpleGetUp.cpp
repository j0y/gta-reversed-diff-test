// test_CTaskSimpleGetUp.cpp — Tests for CTaskSimpleGetUp.

#include "StdInc.h"
#include "TestFramework.h"
#include "Tasks/TaskTypes/TaskSimpleGetUp.h"

GAME_TEST(CTaskSimpleGetUp, GetTaskType) {
    CTaskSimpleGetUp task;
    EXPECT_EQ((int32)task.GetTaskType(), (int32)TASK_SIMPLE_GET_UP);
}

GAME_TEST(CTaskSimpleGetUp, IsSimple) {
    CTaskSimpleGetUp task;
    EXPECT_TRUE(task.IsSimple());
}

GAME_TEST(CTaskSimpleGetUp, Clone_PreservesType) {
    CTaskSimpleGetUp original;
    CTask* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;
    EXPECT_EQ((int32)cloned->GetTaskType(), (int32)TASK_SIMPLE_GET_UP);
    delete cloned;
}

GAME_TEST(CTaskSimpleGetUp, InitialState) {
    CTaskSimpleGetUp task;
    EXPECT_FALSE(task.m_bHasPedGotUp);
    EXPECT_FALSE(task.m_bIsFinished);
}
