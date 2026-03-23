// test_CTaskSimpleAchieveHeading.cpp — Tests for CTaskSimpleAchieveHeading.
// Hook paths: Tasks/TaskTypes/CTaskSimpleAchieveHeading/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "Tasks/TaskTypes/TaskSimpleAchieveHeading.h"
#include "Entity/Ped/Ped.h"

GAME_TEST(CTaskSimpleAchieveHeading, GetTaskType) {
    CTaskSimpleAchieveHeading task(1.57f);
    EXPECT_EQ((int32)task.GetTaskType(), (int32)TASK_SIMPLE_ACHIEVE_HEADING);
}

GAME_TEST(CTaskSimpleAchieveHeading, IsSimple) {
    CTaskSimpleAchieveHeading task(0.0f);
    EXPECT_TRUE(task.IsSimple());
}

GAME_TEST(CTaskSimpleAchieveHeading, Clone_PreservesType) {
    CTaskSimpleAchieveHeading original(3.14f, 0.8f, 0.1f);
    CTask* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;
    EXPECT_EQ((int32)cloned->GetTaskType(), (int32)TASK_SIMPLE_ACHIEVE_HEADING);
    delete cloned;
}

GAME_TEST(CTaskSimpleAchieveHeading, Clone_IsSeparateObject) {
    CTaskSimpleAchieveHeading original(1.0f);
    CTask* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    EXPECT_TRUE((uintptr_t)cloned != (uintptr_t)&original);
    delete cloned;
}

GAME_DIFF_TEST(CTaskSimpleAchieveHeading, MakeAbortable_Urgent) {
    auto* ped = FindPlayerPed(0);
    if (!ped) return;

    bool orig, rev;
    { HookDisableGuard guard("Tasks/TaskTypes/CTaskSimpleAchieveHeading/MakeAbortable");
      CTaskSimpleAchieveHeading t1(1.0f);
      orig = t1.MakeAbortable(ped, ABORT_PRIORITY_URGENT, nullptr); }
    { CTaskSimpleAchieveHeading t2(1.0f);
      rev = t2.MakeAbortable(ped, ABORT_PRIORITY_URGENT, nullptr); }
    EXPECT_EQ(orig, rev);
}

GAME_TEST(CTaskSimpleAchieveHeading, DifferentHeadings) {
    float headings[] = { 0.0f, 1.57f, 3.14f, -1.57f, 6.28f };
    for (float h : headings) {
        CTaskSimpleAchieveHeading task(h);
        EXPECT_EQ((int32)task.GetTaskType(), (int32)TASK_SIMPLE_ACHIEVE_HEADING);
    }
}
