// test_CTaskComplexGoToPointAiming.cpp

#include "StdInc.h"
#include "TestFramework.h"
#include "Tasks/TaskTypes/TaskComplexGoToPointAiming.h"
#include "Entity/Ped/Ped.h"

GAME_TEST(CTaskComplexGoToPointAiming, GetTaskType) {
    CVector movePos(100.0f, 200.0f, 10.0f);
    CVector aimPos(110.0f, 210.0f, 10.0f);
    CTaskComplexGoToPointAiming task(PEDMOVE_RUN, movePos, nullptr, aimPos, 1.0f, 5.0f);
    EXPECT_EQ((int32)task.GetTaskType(), (int32)TASK_COMPLEX_GO_TO_POINT_AIMING);
}

GAME_TEST(CTaskComplexGoToPointAiming, IsComplex) {
    CVector v(0, 0, 0);
    CTaskComplexGoToPointAiming task(PEDMOVE_WALK, v, nullptr, v, 1.0f, 5.0f);
    EXPECT_TRUE(task.IsComplex());
}

GAME_TEST(CTaskComplexGoToPointAiming, Constructor_Fields) {
    CVector movePos(50.0f, 60.0f, 5.0f);
    CVector aimPos(70.0f, 80.0f, 5.0f);
    auto* player = FindPlayerPed(0);
    CTaskComplexGoToPointAiming task(PEDMOVE_SPRINT, movePos, player, aimPos, 2.0f, 8.0f);
    EXPECT_EQ((int32)task.m_moveState, (int32)PEDMOVE_SPRINT);
    EXPECT_NEAR(task.m_movePos.x, 50.0f, 1e-5f);
    EXPECT_NEAR(task.m_aimPos.x, 70.0f, 1e-5f);
    EXPECT_NEAR(task.m_moveTargetRadius, 2.0f, 1e-5f);
    EXPECT_NEAR(task.m_slowDownDistance, 8.0f, 1e-5f);
}

GAME_TEST(CTaskComplexGoToPointAiming, Clone_PreservesType) {
    CVector v(10.0f, 20.0f, 5.0f);
    CTaskComplexGoToPointAiming original(PEDMOVE_RUN, v, nullptr, v, 1.5f, 3.0f);
    CTask* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;
    EXPECT_EQ((int32)cloned->GetTaskType(), (int32)TASK_COMPLEX_GO_TO_POINT_AIMING);
    auto* c = static_cast<CTaskComplexGoToPointAiming*>(cloned);
    EXPECT_NEAR(c->m_movePos.x, 10.0f, 1e-5f);
    EXPECT_NEAR(c->m_moveTargetRadius, 1.5f, 1e-5f);
    delete cloned;
}
