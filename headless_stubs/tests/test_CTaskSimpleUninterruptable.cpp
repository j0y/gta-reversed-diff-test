// test_CTaskSimpleUninterruptable.cpp

#include "StdInc.h"
#include "TestFramework.h"
#include "Tasks/TaskTypes/TaskSimpleUninterruptable.h"

GAME_TEST(CTaskSimpleUninterruptable, GetTaskType) {
    CTaskSimpleUninterruptable task;
    EXPECT_EQ((int32)task.GetTaskType(), (int32)TASK_SIMPLE_UNINTERRUPTABLE);
}

GAME_TEST(CTaskSimpleUninterruptable, IsSimple) {
    CTaskSimpleUninterruptable task;
    EXPECT_TRUE(task.IsSimple());
}

GAME_TEST(CTaskSimpleUninterruptable, MakeAbortable_Leisure_False) {
    CTaskSimpleUninterruptable task;
    EXPECT_FALSE(task.MakeAbortable(nullptr, ABORT_PRIORITY_LEISURE));
}

GAME_TEST(CTaskSimpleUninterruptable, MakeAbortable_Urgent_False) {
    CTaskSimpleUninterruptable task;
    EXPECT_FALSE(task.MakeAbortable(nullptr, ABORT_PRIORITY_URGENT));
}

GAME_TEST(CTaskSimpleUninterruptable, MakeAbortable_Immediate_True) {
    CTaskSimpleUninterruptable task;
    EXPECT_TRUE(task.MakeAbortable(nullptr, ABORT_PRIORITY_IMMEDIATE));
}

GAME_TEST(CTaskSimpleUninterruptable, ProcessPed_NeverFinishes) {
    CTaskSimpleUninterruptable task;
    EXPECT_FALSE(task.ProcessPed(nullptr));
}

GAME_TEST(CTaskSimpleUninterruptable, Clone) {
    CTaskSimpleUninterruptable original;
    CTask* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;
    EXPECT_EQ((int32)cloned->GetTaskType(), (int32)TASK_SIMPLE_UNINTERRUPTABLE);
    delete cloned;
}

GAME_TEST(CTaskSimpleUninterruptable, SizeValidation) {
    EXPECT_EQ(sizeof(CTaskSimpleUninterruptable), (size_t)0x8);
}
