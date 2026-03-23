// test_CTaskComplexGoToPointAndStandStill.cpp — Tests for CTaskComplexGoToPointAndStandStill.
// Hook paths: Tasks/TaskTypes/CTaskComplexGoToPointAndStandStill/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "Tasks/TaskTypes/TaskComplexGoToPointAndStandStill.h"

GAME_TEST(CTaskComplexGoToPointAndStandStill, GetTaskType) {
    CVector target(100.0f, 200.0f, 10.0f);
    CTaskComplexGoToPointAndStandStill task(PEDMOVE_WALK, target);
    EXPECT_EQ((int32)task.GetTaskType(), (int32)TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL);
}

GAME_TEST(CTaskComplexGoToPointAndStandStill, IsComplex) {
    CVector target(0, 0, 0);
    CTaskComplexGoToPointAndStandStill task(PEDMOVE_RUN, target);
    EXPECT_TRUE(task.IsComplex());
}

GAME_TEST(CTaskComplexGoToPointAndStandStill, Constructor_Fields) {
    CVector target(50.0f, 75.0f, 12.0f);
    CTaskComplexGoToPointAndStandStill task(PEDMOVE_SPRINT, target, 3.0f, 5.0f, true, true);
    EXPECT_EQ((int32)task.m_moveState, (int32)PEDMOVE_SPRINT);
    EXPECT_NEAR(task.m_vecTargetPoint.x, 50.0f, 1e-5f);
    EXPECT_NEAR(task.m_vecTargetPoint.y, 75.0f, 1e-5f);
    EXPECT_NEAR(task.m_vecTargetPoint.z, 12.0f, 1e-5f);
    EXPECT_NEAR(task.m_fRadius, 3.0f, 1e-5f);
}

GAME_TEST(CTaskComplexGoToPointAndStandStill, Constructor_Defaults) {
    CVector target(10.0f, 20.0f, 5.0f);
    CTaskComplexGoToPointAndStandStill task(PEDMOVE_WALK, target);
    EXPECT_NEAR(task.m_fRadius, 1.5f, 1e-5f);
    EXPECT_FALSE(task.m_bStopExactly);
}

GAME_TEST(CTaskComplexGoToPointAndStandStill, Clone_PreservesFields) {
    CVector target(99.0f, 88.0f, 77.0f);
    CTaskComplexGoToPointAndStandStill original(PEDMOVE_RUN, target, 4.0f, 6.0f, true, false);
    CTask* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;

    EXPECT_EQ((int32)cloned->GetTaskType(), (int32)TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL);
    auto* c = static_cast<CTaskComplexGoToPointAndStandStill*>(cloned);
    EXPECT_EQ((int32)c->m_moveState, (int32)PEDMOVE_RUN);
    EXPECT_NEAR(c->m_vecTargetPoint.x, 99.0f, 1e-5f);
    EXPECT_NEAR(c->m_fRadius, 4.0f, 1e-5f);
    EXPECT_FALSE(c->m_bStopExactly);

    delete cloned;
}

GAME_TEST(CTaskComplexGoToPointAndStandStill, DifferentMoveStates) {
    CVector target(0, 0, 0);
    eMoveState states[] = { PEDMOVE_STILL, PEDMOVE_WALK, PEDMOVE_RUN, PEDMOVE_SPRINT };
    for (auto ms : states) {
        CTaskComplexGoToPointAndStandStill task(ms, target);
        EXPECT_EQ((int32)task.m_moveState, (int32)ms);
    }
}
