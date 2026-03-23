// test_CTaskComplexSmartFleePoint.cpp

#include "StdInc.h"
#include "TestFramework.h"
#include "Tasks/TaskTypes/TaskComplexSmartFleePoint.h"

GAME_TEST(CTaskComplexSmartFleePoint, GetTaskType) {
    CVector pos(100.0f, 200.0f, 10.0f);
    CTaskComplexSmartFleePoint task(pos, true, 30.0f, 10000);
    EXPECT_EQ((int32)task.GetTaskType(), (int32)TASK_COMPLEX_SMART_FLEE_POINT);
}

GAME_TEST(CTaskComplexSmartFleePoint, IsComplex) {
    CVector pos(0, 0, 0);
    CTaskComplexSmartFleePoint task(pos, false, 20.0f, 5000);
    EXPECT_TRUE(task.IsComplex());
}

GAME_TEST(CTaskComplexSmartFleePoint, Constructor_Fields) {
    CVector pos(50.0f, 60.0f, 5.0f);
    CTaskComplexSmartFleePoint task(pos, true, 25.0f, 8000);
    EXPECT_NEAR(task.m_fleePoint.x, 50.0f, 1e-5f);
    EXPECT_NEAR(task.m_fleePoint.y, 60.0f, 1e-5f);
    EXPECT_NEAR(task.m_safeDist, 25.0f, 1e-5f);
    EXPECT_EQ(task.m_fleeTimeMs, 8000);
    EXPECT_TRUE(task.m_doScream);
    EXPECT_EQ((int32)task.m_moveState, (int32)PEDMOVE_SPRINT);
}

GAME_TEST(CTaskComplexSmartFleePoint, Clone_PreservesFields) {
    CVector pos(10.0f, 20.0f, 5.0f);
    CTaskComplexSmartFleePoint original(pos, false, 50.0f, 15000);
    CTask* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;
    EXPECT_EQ((int32)cloned->GetTaskType(), (int32)TASK_COMPLEX_SMART_FLEE_POINT);
    auto* c = static_cast<CTaskComplexSmartFleePoint*>(cloned);
    EXPECT_NEAR(c->m_safeDist, 50.0f, 1e-5f);
    EXPECT_FALSE(c->m_doScream);
    delete cloned;
}
