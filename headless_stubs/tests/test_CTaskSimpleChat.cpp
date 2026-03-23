// test_CTaskSimpleChat.cpp

#include "StdInc.h"
#include "TestFramework.h"
#include "Tasks/TaskTypes/TaskSimpleChat.h"

GAME_TEST(CTaskSimpleChat, GetTaskType) {
    CTaskSimpleChat task(5000);
    EXPECT_EQ((int32)task.GetTaskType(), (int32)TASK_SIMPLE_CHAT);
}

GAME_TEST(CTaskSimpleChat, Clone) {
    CTaskSimpleChat original(3000);
    CTask* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;
    EXPECT_EQ((int32)cloned->GetTaskType(), (int32)TASK_SIMPLE_CHAT);
    delete cloned;
}

GAME_TEST(CTaskSimpleChat, IsNotInterruptable) {
    CTaskSimpleChat task(5000);
    EXPECT_FALSE(task.IsInterruptable(nullptr));
}
