// test_CTaskSimpleLeaveGroup.cpp

#include "StdInc.h"
#include "TestFramework.h"
#include "Tasks/TaskTypes/TaskSimpleLeaveGroup.h"

GAME_TEST(CTaskSimpleLeaveGroup, GetTaskType) {
    CTaskSimpleLeaveGroup task;
    // Inherits from CTaskSimpleNone, so type is TASK_NONE
    EXPECT_EQ((int32)task.GetTaskType(), (int32)TASK_NONE);
}

GAME_TEST(CTaskSimpleLeaveGroup, IsSimple) {
    CTaskSimpleLeaveGroup task;
    EXPECT_TRUE(task.IsSimple());
}

GAME_TEST(CTaskSimpleLeaveGroup, MakeAbortable_AlwaysTrue) {
    CTaskSimpleLeaveGroup task;
    EXPECT_TRUE(task.MakeAbortable(nullptr));
}

GAME_TEST(CTaskSimpleLeaveGroup, ProcessPed_AlwaysTrue) {
    CTaskSimpleLeaveGroup task;
    EXPECT_TRUE(task.ProcessPed(nullptr));
}
