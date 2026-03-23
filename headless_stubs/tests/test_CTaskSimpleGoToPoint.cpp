// test_CTaskSimpleGoToPoint.cpp — Tests for CTaskSimpleGoToPoint.
// Hook paths: Tasks/TaskTypes/CTaskSimpleGoToPoint/<fn>
//
// CTaskSimpleGoToPoint is a navigation task that makes a ped walk/run to
// a target position within a given radius. Tests verify construction,
// cloning, target point management, and the UpdatePoint method.

#include "StdInc.h"
#include "TestFramework.h"
#include "Tasks/TaskTypes/TaskSimpleGoToPoint.h"
#include "Entity/Ped/Ped.h"

// --- Construction & type ---

GAME_TEST(CTaskSimpleGoToPoint, GetTaskType) {
    CVector target(100.0f, 200.0f, 10.0f);
    CTaskSimpleGoToPoint task(PEDMOVE_WALK, target, 0.5f);
    EXPECT_EQ((int32)task.GetTaskType(), (int32)TASK_SIMPLE_GO_TO_POINT);
}

GAME_TEST(CTaskSimpleGoToPoint, IsSimple) {
    CVector target(0.0f, 0.0f, 0.0f);
    CTaskSimpleGoToPoint task(PEDMOVE_RUN, target);
    EXPECT_TRUE(task.IsSimple());
}

GAME_TEST(CTaskSimpleGoToPoint, Constructor_Walk) {
    CVector target(50.0f, 75.0f, 12.0f);
    CTaskSimpleGoToPoint task(PEDMOVE_WALK, target, 1.0f, true);
    EXPECT_EQ((int32)task.m_moveState, (int32)PEDMOVE_WALK);
    EXPECT_NEAR(task.m_vecTargetPoint.x, 50.0f, 1e-5f);
    EXPECT_NEAR(task.m_vecTargetPoint.y, 75.0f, 1e-5f);
    EXPECT_NEAR(task.m_vecTargetPoint.z, 12.0f, 1e-5f);
    EXPECT_NEAR(task.m_fRadius, 1.0f, 1e-5f);
    EXPECT_TRUE(task.gotoPointFlags.m_bMoveTowardsTargetPoint);
}

GAME_TEST(CTaskSimpleGoToPoint, Constructor_Run) {
    CVector target(0.0f, 0.0f, 0.0f);
    CTaskSimpleGoToPoint task(PEDMOVE_RUN, target, 2.0f, false);
    EXPECT_EQ((int32)task.m_moveState, (int32)PEDMOVE_RUN);
    EXPECT_NEAR(task.m_fRadius, 2.0f, 1e-5f);
    EXPECT_FALSE(task.gotoPointFlags.m_bMoveTowardsTargetPoint);
}

GAME_TEST(CTaskSimpleGoToPoint, Constructor_DefaultRadius) {
    CVector target(10.0f, 20.0f, 5.0f);
    CTaskSimpleGoToPoint task(PEDMOVE_WALK, target);
    EXPECT_NEAR(task.m_fRadius, 0.5f, 1e-5f);
}

// --- Clone ---

GAME_TEST(CTaskSimpleGoToPoint, Clone_PreservesFields) {
    CVector target(100.0f, 200.0f, 15.0f);
    CTaskSimpleGoToPoint original(PEDMOVE_SPRINT, target, 3.0f, true);
    CTask* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;

    EXPECT_EQ((int32)cloned->GetTaskType(), (int32)TASK_SIMPLE_GO_TO_POINT);

    auto* c = static_cast<CTaskSimpleGoToPoint*>(cloned);
    EXPECT_EQ((int32)c->m_moveState, (int32)PEDMOVE_SPRINT);
    EXPECT_NEAR(c->m_vecTargetPoint.x, 100.0f, 1e-5f);
    EXPECT_NEAR(c->m_vecTargetPoint.y, 200.0f, 1e-5f);
    EXPECT_NEAR(c->m_vecTargetPoint.z, 15.0f, 1e-5f);
    EXPECT_NEAR(c->m_fRadius, 3.0f, 1e-5f);
    EXPECT_TRUE(c->gotoPointFlags.m_bMoveTowardsTargetPoint);

    delete cloned;
}

// --- UpdatePoint ---

GAME_TEST(CTaskSimpleGoToPoint, UpdatePoint_ChangesTarget) {
    CVector target1(10.0f, 20.0f, 5.0f);
    CTaskSimpleGoToPoint task(PEDMOVE_WALK, target1, 0.5f);

    CVector target2(50.0f, 60.0f, 8.0f);
    task.UpdatePoint(target2, 2.0f);

    EXPECT_NEAR(task.m_vecTargetPoint.x, 50.0f, 1e-5f);
    EXPECT_NEAR(task.m_vecTargetPoint.y, 60.0f, 1e-5f);
    EXPECT_NEAR(task.m_vecTargetPoint.z, 8.0f, 1e-5f);
    EXPECT_NEAR(task.m_fRadius, 2.0f, 1e-5f);
}

// --- GoTo base class getters/setters ---

GAME_TEST(CTaskSimpleGoToPoint, GetSetTargetPt) {
    CVector target(10.0f, 20.0f, 5.0f);
    CTaskSimpleGoToPoint task(PEDMOVE_WALK, target);

    CVector got = task.GetTargetPt();
    EXPECT_NEAR(got.x, 10.0f, 1e-5f);
    EXPECT_NEAR(got.y, 20.0f, 1e-5f);

    CVector newTarget(99.0f, 88.0f, 77.0f);
    task.SetTargetPt(newTarget);
    got = task.GetTargetPt();
    EXPECT_NEAR(got.x, 99.0f, 1e-5f);
    EXPECT_NEAR(got.y, 88.0f, 1e-5f);
    EXPECT_NEAR(got.z, 77.0f, 1e-5f);
}

GAME_TEST(CTaskSimpleGoToPoint, GetSetTargetPtRadius) {
    CVector target(0.0f, 0.0f, 0.0f);
    CTaskSimpleGoToPoint task(PEDMOVE_WALK, target, 1.5f);

    EXPECT_NEAR(task.GetTargetPtRadius(), 1.5f, 1e-5f);

    task.SetTargetPtRadius(5.0f);
    EXPECT_NEAR(task.GetTargetPtRadius(), 5.0f, 1e-5f);
}

// --- MakeAbortable differential ---

GAME_DIFF_TEST(CTaskSimpleGoToPoint, MakeAbortable_Urgent) {
    auto* ped = FindPlayerPed(0);
    if (!ped) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No player ped"); return; }

    CVector target(2500.0f, -1660.0f, 13.5f);

    bool orig, rev;
    { HookDisableGuard guard("Tasks/TaskTypes/CTaskSimpleGoToPoint/MakeAbortable");
      CTaskSimpleGoToPoint t1(PEDMOVE_WALK, target, 0.5f);
      orig = t1.MakeAbortable(ped, ABORT_PRIORITY_URGENT, nullptr); }
    {
      CTaskSimpleGoToPoint t2(PEDMOVE_WALK, target, 0.5f);
      rev = t2.MakeAbortable(ped, ABORT_PRIORITY_URGENT, nullptr); }
    EXPECT_EQ(orig, rev);
}
