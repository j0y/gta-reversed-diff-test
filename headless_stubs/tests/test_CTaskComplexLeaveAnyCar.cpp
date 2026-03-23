// test_CTaskComplexLeaveAnyCar.cpp

#include "StdInc.h"
#include "TestFramework.h"
#include "Tasks/TaskTypes/TaskComplexLeaveAnyCar.h"

GAME_TEST(CTaskComplexLeaveAnyCar, GetTaskType) {
    CTaskComplexLeaveAnyCar task(0, true, false);
    EXPECT_EQ((int32)task.GetTaskType(), (int32)TASK_COMPLEX_LEAVE_ANY_CAR);
}

GAME_TEST(CTaskComplexLeaveAnyCar, IsComplex) {
    CTaskComplexLeaveAnyCar task(0, true, false);
    EXPECT_TRUE(task.IsComplex());
}

GAME_TEST(CTaskComplexLeaveAnyCar, Constructor_Fields) {
    CTaskComplexLeaveAnyCar task(1000, true, true);
    EXPECT_EQ(task.m_nDelayTime, 1000);
    EXPECT_TRUE(task.m_sensibleLeaveCar);
    EXPECT_TRUE(task.m_forceGetOut);
}

GAME_TEST(CTaskComplexLeaveAnyCar, Constructor_Defaults) {
    CTaskComplexLeaveAnyCar task(0, false, false);
    EXPECT_EQ(task.m_nDelayTime, 0);
    EXPECT_FALSE(task.m_sensibleLeaveCar);
    EXPECT_FALSE(task.m_forceGetOut);
}

GAME_TEST(CTaskComplexLeaveAnyCar, Clone_PreservesFields) {
    CTaskComplexLeaveAnyCar original(500, true, false);
    CTask* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;
    auto* c = static_cast<CTaskComplexLeaveAnyCar*>(cloned);
    EXPECT_EQ(c->m_nDelayTime, 500);
    EXPECT_TRUE(c->m_sensibleLeaveCar);
    EXPECT_FALSE(c->m_forceGetOut);
    delete cloned;
}

GAME_TEST(CTaskComplexLeaveAnyCar, CreateNextSubTask_ReturnsNull) {
    CTaskComplexLeaveAnyCar task(0, true, false);
    // CreateNextSubTask always returns nullptr for this task
    EXPECT_EQ((uintptr_t)task.CreateNextSubTask(nullptr), (uintptr_t)nullptr);
}
