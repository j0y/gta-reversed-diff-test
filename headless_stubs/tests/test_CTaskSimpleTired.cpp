// test_CTaskSimpleTired.cpp — Tests for CTaskSimpleTired.
// Hook paths: Tasks/TaskTypes/CTaskSimpleTired/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "Tasks/TaskTypes/TaskSimpleTired.h"

GAME_TEST(CTaskSimpleTired, GetTaskType) {
    CTaskSimpleTired task(5000);
    EXPECT_EQ((int32)task.GetTaskType(), (int32)TASK_SIMPLE_TIRED);
}

GAME_TEST(CTaskSimpleTired, IsSimple) {
    CTaskSimpleTired task(5000);
    EXPECT_TRUE(task.IsSimple());
}

GAME_TEST(CTaskSimpleTired, Constructor_Duration) {
    CTaskSimpleTired task(8000);
    EXPECT_EQ(task.m_TiredDurationMs, (uint32)8000);
}

GAME_TEST(CTaskSimpleTired, Clone_PreservesType) {
    CTaskSimpleTired original(4000);
    CTask* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;
    EXPECT_EQ((int32)cloned->GetTaskType(), (int32)TASK_SIMPLE_TIRED);
    delete cloned;
}

GAME_TEST(CTaskSimpleTired, DifferentDurations) {
    uint32 durations[] = { 0, 1000, 5000, 15000 };
    for (uint32 d : durations) {
        CTaskSimpleTired task(d);
        EXPECT_EQ(task.m_TiredDurationMs, d);
    }
}
