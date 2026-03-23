// test_CTaskComplexFleePoint.cpp

#include "StdInc.h"
#include "TestFramework.h"
#include "Tasks/TaskTypes/TaskComplexFleePoint.h"

GAME_TEST(CTaskComplexFleePoint, GetTaskType) {
    CVector point(100.0f, 200.0f, 10.0f);
    CTaskComplexFleePoint task(point, true, 30.0f, 10000);
    EXPECT_EQ((int32)task.GetTaskType(), (int32)TASK_COMPLEX_FLEE_POINT);
}

GAME_TEST(CTaskComplexFleePoint, IsComplex) {
    CVector point(0, 0, 0);
    CTaskComplexFleePoint task(point, false, 20.0f, 5000);
    EXPECT_TRUE(task.IsComplex());
}

GAME_TEST(CTaskComplexFleePoint, Constructor_Fields) {
    CVector point(50.0f, 60.0f, 5.0f);
    CTaskComplexFleePoint task(point, true, 25.0f, 8000);
    EXPECT_NEAR(task.m_fleeFromPos.x, 50.0f, 1e-5f);
    EXPECT_NEAR(task.m_fleeFromPos.y, 60.0f, 1e-5f);
    EXPECT_NEAR(task.m_safeDist, 25.0f, 1e-5f);
    EXPECT_EQ(task.m_durationMs, 8000);
    EXPECT_TRUE(task.m_bScream);
}

GAME_TEST(CTaskComplexFleePoint, Clone_PreservesFields) {
    CVector point(10.0f, 20.0f, 5.0f);
    CTaskComplexFleePoint original(point, false, 50.0f, 15000);
    CTask* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;
    EXPECT_EQ((int32)cloned->GetTaskType(), (int32)TASK_COMPLEX_FLEE_POINT);
    auto* c = static_cast<CTaskComplexFleePoint*>(cloned);
    EXPECT_NEAR(c->m_safeDist, 50.0f, 1e-5f);
    EXPECT_FALSE(c->m_bScream);
    delete cloned;
}

GAME_TEST(CTaskComplexFleePoint, SetFleePosition) {
    CVector point(0, 0, 0);
    CTaskComplexFleePoint task(point, false, 10.0f, 5000);
    CVector newPos(100.0f, 200.0f, 5.0f);
    task.SetFleePosition(newPos, 40.0f, true);
    EXPECT_NEAR(task.m_fleeFromPos.x, 100.0f, 1e-5f);
    EXPECT_NEAR(task.m_safeDist, 40.0f, 1e-5f);
    EXPECT_TRUE(task.m_bScream);
}
