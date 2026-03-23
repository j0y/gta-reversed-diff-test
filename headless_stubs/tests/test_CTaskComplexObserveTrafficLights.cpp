// test_CTaskComplexObserveTrafficLights.cpp

#include "StdInc.h"
#include "TestFramework.h"
#include "Tasks/TaskTypes/TaskComplexObserveTrafficLights.h"

GAME_TEST(CTaskComplexObserveTrafficLights, GetTaskType) {
    CTaskComplexObserveTrafficLights task;
    EXPECT_EQ((int32)task.GetTaskType(), (int32)TASK_COMPLEX_OBSERVE_TRAFFIC_LIGHTS);
}

GAME_TEST(CTaskComplexObserveTrafficLights, IsComplex) {
    CTaskComplexObserveTrafficLights task;
    EXPECT_TRUE(task.IsComplex());
}

GAME_TEST(CTaskComplexObserveTrafficLights, MakeAbortable_AlwaysTrue) {
    CTaskComplexObserveTrafficLights task;
    EXPECT_TRUE(task.MakeAbortable(nullptr));
}

GAME_TEST(CTaskComplexObserveTrafficLights, Clone) {
    CTaskComplexObserveTrafficLights original;
    CTask* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;
    EXPECT_EQ((int32)cloned->GetTaskType(), (int32)TASK_COMPLEX_OBSERVE_TRAFFIC_LIGHTS);
    delete cloned;
}
