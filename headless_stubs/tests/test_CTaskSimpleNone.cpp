// test_CTaskSimpleNone.cpp — Tests for CTaskSimpleNone (null task).

#include "StdInc.h"
#include "TestFramework.h"
#include "Tasks/TaskTypes/TaskSimpleNone.h"
#include "Entity/Ped/Ped.h"

GAME_TEST(CTaskSimpleNone, GetTaskType) {
    CTaskSimpleNone task;
    EXPECT_EQ((int32)task.GetTaskType(), (int32)TASK_NONE);
}

GAME_TEST(CTaskSimpleNone, IsSimple) {
    CTaskSimpleNone task;
    EXPECT_TRUE(task.IsSimple());
}

GAME_TEST(CTaskSimpleNone, MakeAbortable_AlwaysTrue) {
    CTaskSimpleNone task;
    EXPECT_TRUE(task.MakeAbortable(nullptr));
}

GAME_TEST(CTaskSimpleNone, ProcessPed_AlwaysTrue) {
    CTaskSimpleNone task;
    // ProcessPed returns true immediately (task finishes instantly)
    EXPECT_TRUE(task.ProcessPed(nullptr));
}

GAME_TEST(CTaskSimpleNone, Clone) {
    CTaskSimpleNone original;
    CTask* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;
    EXPECT_EQ((int32)cloned->GetTaskType(), (int32)TASK_NONE);
    delete cloned;
}

GAME_TEST(CTaskSimpleNone, SizeValidation) {
    EXPECT_EQ(sizeof(CTaskSimpleNone), (size_t)0x8);
}
