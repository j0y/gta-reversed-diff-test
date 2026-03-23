// test_CTaskComplexCrossRoadLookAndAchieveHeading.cpp
// Hook paths: Tasks/TaskTypes/CTaskComplexCrossRoadLookAndAchieveHeading/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "Tasks/TaskTypes/TaskComplexCrossRoadLookAndAchieveHeading.h"

GAME_TEST(CTaskComplexCrossRoadLookAndAchieveHeading, GetTaskType) {
    CTaskComplexCrossRoadLookAndAchieveHeading task(2000, 1.57f);
    EXPECT_EQ((int32)task.GetTaskType(), (int32)TASK_COMPLEX_CROSS_ROAD_LOOK_AND_ACHIEVE_HEADING);
}

GAME_TEST(CTaskComplexCrossRoadLookAndAchieveHeading, IsComplex) {
    CTaskComplexCrossRoadLookAndAchieveHeading task(2000, 0.0f);
    EXPECT_TRUE(task.IsComplex());
}

GAME_TEST(CTaskComplexCrossRoadLookAndAchieveHeading, Constructor_Fields) {
    CTaskComplexCrossRoadLookAndAchieveHeading task(5000, 3.14f);
    EXPECT_EQ(task.m_nDuration, 5000);
    EXPECT_NEAR(task.m_fTargetHeading, 3.14f, 1e-5f);
}

GAME_TEST(CTaskComplexCrossRoadLookAndAchieveHeading, Clone_PreservesFields) {
    CTaskComplexCrossRoadLookAndAchieveHeading original(3000, 1.0f);
    CTask* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;

    EXPECT_EQ((int32)cloned->GetTaskType(), (int32)TASK_COMPLEX_CROSS_ROAD_LOOK_AND_ACHIEVE_HEADING);
    auto* c = static_cast<CTaskComplexCrossRoadLookAndAchieveHeading*>(cloned);
    EXPECT_EQ(c->m_nDuration, 3000);
    EXPECT_NEAR(c->m_fTargetHeading, 1.0f, 1e-5f);

    delete cloned;
}

GAME_TEST(CTaskComplexCrossRoadLookAndAchieveHeading, DifferentHeadings) {
    float headings[] = { 0.0f, 1.57f, 3.14f, -1.57f, 6.28f };
    for (float h : headings) {
        CTaskComplexCrossRoadLookAndAchieveHeading task(1000, h);
        EXPECT_NEAR(task.m_fTargetHeading, h, 1e-5f);
    }
}
