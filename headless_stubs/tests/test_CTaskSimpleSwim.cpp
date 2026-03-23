// test_CTaskSimpleSwim.cpp — Tests for CTaskSimpleSwim.
// Hook paths: Tasks/TaskTypes/CTaskSimpleSwim/<fn>
//
// CTaskSimpleSwim handles swimming behavior with multiple states (tread,
// sprint, dive, underwater). Tests verify construction, cloning, and type.

#include "StdInc.h"
#include "TestFramework.h"
#include "Tasks/TaskTypes/TaskSimpleSwim.h"
#include "Entity/Ped/Ped.h"

// --- Construction & type ---

GAME_TEST(CTaskSimpleSwim, GetTaskType) {
    CVector pos(0.0f, 0.0f, 0.0f);
    CTaskSimpleSwim task(&pos, nullptr);
    EXPECT_EQ((int32)task.GetTaskType(), (int32)TASK_SIMPLE_SWIM);
}

GAME_TEST(CTaskSimpleSwim, IsSimple) {
    CVector pos(0.0f, 0.0f, 0.0f);
    CTaskSimpleSwim task(&pos, nullptr);
    EXPECT_TRUE(task.IsSimple());
}

// --- Clone ---

GAME_TEST(CTaskSimpleSwim, Clone_PreservesType) {
    CVector pos(100.0f, 200.0f, 5.0f);
    CTaskSimpleSwim original(&pos, nullptr);
    CTask* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;

    EXPECT_EQ((int32)cloned->GetTaskType(), (int32)TASK_SIMPLE_SWIM);
    EXPECT_TRUE(cloned->IsSimple());

    delete cloned;
}

GAME_TEST(CTaskSimpleSwim, Clone_IsSeparateObject) {
    CVector pos(0.0f, 0.0f, 0.0f);
    CTaskSimpleSwim original(&pos, nullptr);
    CTask* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    EXPECT_TRUE((uintptr_t)cloned != (uintptr_t)&original);
    delete cloned;
}

// --- MakeAbortable differential ---

GAME_DIFF_TEST(CTaskSimpleSwim, MakeAbortable_Immediate) {
    auto* ped = FindPlayerPed(0);
    if (!ped) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No player ped"); return; }

    CVector pos(0.0f, 0.0f, 0.0f);

    bool orig, rev;
    { HookDisableGuard guard("Tasks/TaskTypes/CTaskSimpleSwim/MakeAbortable");
      CTaskSimpleSwim t1(&pos, nullptr);
      orig = t1.MakeAbortable(ped, ABORT_PRIORITY_IMMEDIATE, nullptr); }
    {
      CTaskSimpleSwim t2(&pos, nullptr);
      rev = t2.MakeAbortable(ped, ABORT_PRIORITY_IMMEDIATE, nullptr); }
    EXPECT_EQ(orig, rev);
}
