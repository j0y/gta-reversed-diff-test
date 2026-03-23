// test_CTaskComplexDiveFromAttachedEntityAndGetUp.cpp

#include "StdInc.h"
#include "TestFramework.h"
#include "Tasks/TaskTypes/TaskComplexDiveFromAttachedEntityAndGetUp.h"

GAME_TEST(CTaskComplexDiveFromAttachedEntityAndGetUp, GetTaskType) {
    CTaskComplexDiveFromAttachedEntityAndGetUp task;
    EXPECT_EQ((int32)task.GetTaskType(), (int32)TASK_COMPLEX_DIVE_FROM_ATTACHED_ENTITY_AND_GET_UP);
}

GAME_TEST(CTaskComplexDiveFromAttachedEntityAndGetUp, IsComplex) {
    CTaskComplexDiveFromAttachedEntityAndGetUp task;
    EXPECT_TRUE(task.IsComplex());
}

GAME_TEST(CTaskComplexDiveFromAttachedEntityAndGetUp, Constructor_Default) {
    CTaskComplexDiveFromAttachedEntityAndGetUp task;
    EXPECT_EQ((int32)task.GetTaskType(), (int32)TASK_COMPLEX_DIVE_FROM_ATTACHED_ENTITY_AND_GET_UP);
}

GAME_TEST(CTaskComplexDiveFromAttachedEntityAndGetUp, Constructor_WithTime) {
    CTaskComplexDiveFromAttachedEntityAndGetUp task(3000);
    EXPECT_EQ((int32)task.GetTaskType(), (int32)TASK_COMPLEX_DIVE_FROM_ATTACHED_ENTITY_AND_GET_UP);
}

GAME_TEST(CTaskComplexDiveFromAttachedEntityAndGetUp, Clone) {
    CTaskComplexDiveFromAttachedEntityAndGetUp original(5000);
    CTask* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;
    EXPECT_EQ((int32)cloned->GetTaskType(), (int32)TASK_COMPLEX_DIVE_FROM_ATTACHED_ENTITY_AND_GET_UP);
    delete cloned;
}
