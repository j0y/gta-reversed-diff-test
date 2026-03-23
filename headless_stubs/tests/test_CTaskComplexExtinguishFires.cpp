// test_CTaskComplexExtinguishFires.cpp

#include "StdInc.h"
#include "TestFramework.h"
#include "Tasks/TaskTypes/TaskComplexExtinguishFires.h"

GAME_TEST(CTaskComplexExtinguishFires, GetTaskType) {
    CTaskComplexExtinguishFires task;
    EXPECT_EQ((int32)task.GetTaskType(), (int32)TASK_COMPLEX_EXTINGUISH_FIRES);
}

GAME_TEST(CTaskComplexExtinguishFires, IsComplex) {
    CTaskComplexExtinguishFires task;
    EXPECT_TRUE(task.IsComplex());
}

GAME_TEST(CTaskComplexExtinguishFires, Clone) {
    CTaskComplexExtinguishFires original;
    CTask* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;
    EXPECT_EQ((int32)cloned->GetTaskType(), (int32)TASK_COMPLEX_EXTINGUISH_FIRES);
    delete cloned;
}

GAME_TEST(CTaskComplexExtinguishFires, InitialState) {
    CTaskComplexExtinguishFires task;
    EXPECT_EQ((uintptr_t)task.m_fireToExt, (uintptr_t)nullptr);
}
