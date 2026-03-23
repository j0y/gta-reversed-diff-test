// test_CTaskComplexPolicePursuit.cpp

#include "StdInc.h"
#include "TestFramework.h"
#include "Tasks/TaskTypes/TaskComplexPolicePursuit.h"

GAME_TEST(CTaskComplexPolicePursuit, GetTaskType) {
    CTaskComplexPolicePursuit task;
    EXPECT_EQ((int32)task.GetTaskType(), (int32)TASK_COMPLEX_POLICE_PURSUIT);
}

GAME_TEST(CTaskComplexPolicePursuit, IsComplex) {
    CTaskComplexPolicePursuit task;
    EXPECT_TRUE(task.IsComplex());
}

GAME_TEST(CTaskComplexPolicePursuit, DefaultFlags) {
    CTaskComplexPolicePursuit task;
    EXPECT_FALSE(task.m_IsRoadBlockCop);
    EXPECT_FALSE(task.m_IsPlayerInCullZone);
    EXPECT_TRUE(task.m_CouldJoinPursuit);
}

GAME_TEST(CTaskComplexPolicePursuit, Clone_PreservesType) {
    CTaskComplexPolicePursuit original;
    CTask* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;
    EXPECT_EQ((int32)cloned->GetTaskType(), (int32)TASK_COMPLEX_POLICE_PURSUIT);
    delete cloned;
}

GAME_TEST(CTaskComplexPolicePursuit, CreateNextSubTask_ReturnsNull) {
    CTaskComplexPolicePursuit task;
    EXPECT_EQ((uintptr_t)task.CreateNextSubTask(nullptr), (uintptr_t)nullptr);
}

GAME_TEST(CTaskComplexPolicePursuit, SizeValidation) {
    EXPECT_EQ(sizeof(CTaskComplexPolicePursuit), (size_t)0x18);
}
