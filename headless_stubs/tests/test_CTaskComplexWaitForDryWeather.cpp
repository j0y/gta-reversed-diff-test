// test_CTaskComplexWaitForDryWeather.cpp

#include "StdInc.h"
#include "TestFramework.h"
#include "Tasks/TaskTypes/TaskComplexWaitForDryWeather.h"

GAME_TEST(CTaskComplexWaitForDryWeather, GetTaskType) {
    CTaskComplexWaitForDryWeather task;
    EXPECT_EQ((int32)task.GetTaskType(), (int32)TASK_COMPLEX_WAIT_FOR_DRY_WEATHER);
}

GAME_TEST(CTaskComplexWaitForDryWeather, IsComplex) {
    CTaskComplexWaitForDryWeather task;
    EXPECT_TRUE(task.IsComplex());
}

GAME_TEST(CTaskComplexWaitForDryWeather, Clone) {
    CTaskComplexWaitForDryWeather original;
    CTask* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;
    EXPECT_EQ((int32)cloned->GetTaskType(), (int32)TASK_COMPLEX_WAIT_FOR_DRY_WEATHER);
    delete cloned;
}
