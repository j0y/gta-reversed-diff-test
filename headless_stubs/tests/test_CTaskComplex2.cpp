// test_CTaskComplex2.cpp — Differential tests for CTaskComplex* subclass GetTaskType() methods.
// Tests both behavior (correct enum value) and differential (reversed vs original).

#include "StdInc.h"
#include "TestFramework.h"

#include "TaskComplexBeStill.h"
#include "TaskComplexFacial.h"
#include "TaskComplexUseGoggles.h"
#include "TaskComplexExtinguishFires.h"
#include "TaskComplexPolicePursuit.h"
#include "TaskComplexEnterAnyCarAsDriver.h"

// ---------------------------------------------------------------------------
// Behavior tests — verify GetTaskType() returns the correct enum
// ---------------------------------------------------------------------------

GAME_TEST(CTaskComplexBeStill, GetTaskType_ReturnsCorrect) {
    CTaskComplexBeStill task;
    EXPECT_EQ(task.GetTaskType(), TASK_COMPLEX_BE_STILL);
}

GAME_TEST(CTaskComplexBeStill, Clone_PreservesType) {
    CTaskComplexBeStill task;
    auto* cloned = task.Clone();
    EXPECT_EQ(cloned->GetTaskType(), TASK_COMPLEX_BE_STILL);
    delete cloned;
}

GAME_TEST(CTaskComplexFacial, GetTaskType_ReturnsCorrect) {
    CTaskComplexFacial task;
    EXPECT_EQ(task.GetTaskType(), TASK_COMPLEX_FACIAL);
}

GAME_TEST(CTaskComplexFacial, Clone_PreservesType) {
    CTaskComplexFacial task;
    auto* cloned = task.Clone();
    EXPECT_EQ(cloned->GetTaskType(), TASK_COMPLEX_FACIAL);
    delete cloned;
}

GAME_TEST(CTaskComplexUseGoggles, GetTaskType_ReturnsCorrect) {
    CTaskComplexUseGoggles task;
    EXPECT_EQ(task.GetTaskType(), TASK_COMPLEX_USE_GOGGLES);
}

GAME_TEST(CTaskComplexUseGoggles, Clone_PreservesType) {
    CTaskComplexUseGoggles task;
    auto* cloned = task.Clone();
    EXPECT_EQ(cloned->GetTaskType(), TASK_COMPLEX_USE_GOGGLES);
    delete cloned;
}

GAME_TEST(CTaskComplexExtinguishFires, GetTaskType_ReturnsCorrect) {
    CTaskComplexExtinguishFires task;
    EXPECT_EQ(task.GetTaskType(), TASK_COMPLEX_EXTINGUISH_FIRES);
}

GAME_TEST(CTaskComplexPolicePursuit, GetTaskType_ReturnsCorrect) {
    CTaskComplexPolicePursuit task;
    EXPECT_EQ(task.GetTaskType(), TASK_COMPLEX_POLICE_PURSUIT);
}

GAME_TEST(CTaskComplexEnterAnyCarAsDriver, GetTaskType_ReturnsCorrect) {
    CTaskComplexEnterAnyCarAsDriver task;
    EXPECT_EQ(task.GetTaskType(), TASK_COMPLEX_ENTER_ANY_CAR_AS_DRIVER);
}

GAME_TEST(CTaskComplexEnterAnyCarAsDriver, Clone_PreservesType) {
    CTaskComplexEnterAnyCarAsDriver task;
    auto* cloned = task.Clone();
    EXPECT_EQ(cloned->GetTaskType(), TASK_COMPLEX_ENTER_ANY_CAR_AS_DRIVER);
    delete cloned;
}

// ---------------------------------------------------------------------------
// Differential tests — compare reversed GetTaskType() vs original (hooked)
// Only for tasks that have RH_ScopedVMTInstall(GetTaskType, ...) in their .cpp
// ---------------------------------------------------------------------------

GAME_DIFF_TEST(CTaskComplexFacial, GetTaskType) {
    SuspendOtherThreads freeze;
    CTaskComplexFacial task;

    eTaskType orig;
    {
        HookDisableGuard guard("Tasks/TaskTypes/CTaskComplexFacial/GetTaskType");
        orig = task.GetTaskType();
    }
    eTaskType rev = task.GetTaskType();
    EXPECT_EQ(orig, rev);
    EXPECT_EQ(orig, TASK_COMPLEX_FACIAL);
}

GAME_DIFF_TEST(CTaskComplexUseGoggles, GetTaskType) {
    SuspendOtherThreads freeze;
    CTaskComplexUseGoggles task;

    eTaskType orig;
    {
        HookDisableGuard guard("Tasks/TaskTypes/CTaskComplexUseGoggles/GetTaskType");
        orig = task.GetTaskType();
    }
    eTaskType rev = task.GetTaskType();
    EXPECT_EQ(orig, rev);
    EXPECT_EQ(orig, TASK_COMPLEX_USE_GOGGLES);
}

GAME_DIFF_TEST(CTaskComplexExtinguishFires, GetTaskType) {
    SuspendOtherThreads freeze;
    CTaskComplexExtinguishFires task;

    eTaskType orig;
    {
        HookDisableGuard guard("Tasks/TaskTypes/CTaskComplexExtinguishFires/GetTaskType");
        orig = task.GetTaskType();
    }
    eTaskType rev = task.GetTaskType();
    EXPECT_EQ(orig, rev);
    EXPECT_EQ(orig, TASK_COMPLEX_EXTINGUISH_FIRES);
}

GAME_DIFF_TEST(CTaskComplexPolicePursuit, GetTaskType) {
    SuspendOtherThreads freeze;
    CTaskComplexPolicePursuit task;

    eTaskType orig;
    {
        HookDisableGuard guard("Tasks/TaskTypes/CTaskComplexPolicePursuit/GetTaskType");
        orig = task.GetTaskType();
    }
    eTaskType rev = task.GetTaskType();
    EXPECT_EQ(orig, rev);
    EXPECT_EQ(orig, TASK_COMPLEX_POLICE_PURSUIT);
}

GAME_DIFF_TEST(CTaskComplexEnterAnyCarAsDriver, GetTaskType) {
    SuspendOtherThreads freeze;
    CTaskComplexEnterAnyCarAsDriver task;

    eTaskType orig;
    {
        HookDisableGuard guard("Tasks/TaskTypes/CTaskComplexEnterAnyCarAsDriver/GetTaskType");
        orig = task.GetTaskType();
    }
    eTaskType rev = task.GetTaskType();
    EXPECT_EQ(orig, rev);
    EXPECT_EQ(orig, TASK_COMPLEX_ENTER_ANY_CAR_AS_DRIVER);
}

// ---------------------------------------------------------------------------
// Differential tests for Clone() — for tasks with hooked Clone
// ---------------------------------------------------------------------------

GAME_DIFF_TEST(CTaskComplexFacial, Clone) {
    SuspendOtherThreads freeze;
    CTaskComplexFacial task;

    CTask* origClone;
    {
        HookDisableGuard guard("Tasks/TaskTypes/CTaskComplexFacial/Clone");
        origClone = task.Clone();
    }
    CTask* revClone = task.Clone();

    EXPECT_EQ(origClone->GetTaskType(), revClone->GetTaskType());
    EXPECT_EQ(origClone->GetTaskType(), TASK_COMPLEX_FACIAL);

    delete origClone;
    delete revClone;
}
