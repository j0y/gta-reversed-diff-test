// test_CTaskSimplePause.cpp — Tests for CTaskSimplePause.
// Hook paths: Tasks/TaskTypes/CTaskSimplePause/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "Tasks/TaskTypes/TaskSimplePause.h"

GAME_TEST(CTaskSimplePause, GetTaskType) {
    CTaskSimplePause task(1000);
    EXPECT_EQ((int32)task.GetTaskType(), (int32)TASK_SIMPLE_PAUSE);
}

GAME_TEST(CTaskSimplePause, IsSimple) {
    CTaskSimplePause task(1000);
    EXPECT_TRUE(task.IsSimple());
}

GAME_TEST(CTaskSimplePause, Clone_PreservesType) {
    CTaskSimplePause original(3000);
    CTask* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;
    EXPECT_EQ((int32)cloned->GetTaskType(), (int32)TASK_SIMPLE_PAUSE);
    delete cloned;
}

GAME_TEST(CTaskSimplePause, Clone_IsSeparateObject) {
    CTaskSimplePause original(1000);
    CTask* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    EXPECT_TRUE((uintptr_t)cloned != (uintptr_t)&original);
    delete cloned;
}

GAME_TEST(CTaskSimplePause, GetSubTask_Null) {
    CTaskSimplePause task(1000);
    EXPECT_EQ((uintptr_t)task.GetSubTask(), (uintptr_t)nullptr);
}
