// test_CTaskSimple2.cpp — Differential tests for CTaskSimple* subclass GetTaskType() methods.
// Tests both behavior (correct enum value) and differential (reversed vs original).

#include "StdInc.h"
#include "TestFramework.h"

#include "TaskSimpleStandStill.h"
#include "TaskSimplePause.h"
#include "TaskSimpleDead.h"
#include "TaskSimpleTired.h"
#include "TaskSimplePlayerOnFire.h"
#include "TaskSimpleTurn180.h"
#include "TaskSimpleShakeFist.h"
#include "TaskSimpleClearLookAt.h"
#include "TaskSimpleDuckToggle.h"
#include "TaskSimpleFinishBrain.h"
#include "TaskSimpleIKManager.h"
#include "TaskSimpleFacial.h"

// ---------------------------------------------------------------------------
// Behavior tests — verify GetTaskType() returns the correct enum
// ---------------------------------------------------------------------------

GAME_TEST(CTaskSimpleStandStill, GetTaskType_ReturnsCorrect) {
    CTaskSimpleStandStill task(1000);
    EXPECT_EQ(task.GetTaskType(), TASK_SIMPLE_STAND_STILL);
}

GAME_TEST(CTaskSimpleStandStill, Clone_PreservesType) {
    CTaskSimpleStandStill task(2000, true, false, 4.0f);
    auto* cloned = task.Clone();
    EXPECT_EQ(cloned->GetTaskType(), TASK_SIMPLE_STAND_STILL);
    delete cloned;
}

GAME_TEST(CTaskSimplePause, GetTaskType_ReturnsCorrect) {
    CTaskSimplePause task(500);
    EXPECT_EQ(task.GetTaskType(), TASK_SIMPLE_PAUSE);
}

GAME_TEST(CTaskSimplePause, Clone_PreservesType2) {
    CTaskSimplePause task(500);
    auto* cloned = task.Clone();
    EXPECT_EQ(cloned->GetTaskType(), TASK_SIMPLE_PAUSE);
    delete cloned;
}

GAME_TEST(CTaskSimpleDead, GetTaskType_ReturnsCorrect) {
    CTaskSimpleDead task(0, false);
    EXPECT_EQ(task.GetTaskType(), TASK_SIMPLE_DEAD);
}

GAME_TEST(CTaskSimpleDead, Clone_PreservesType) {
    CTaskSimpleDead task(1000, true);
    auto* cloned = task.Clone();
    EXPECT_EQ(cloned->GetTaskType(), TASK_SIMPLE_DEAD);
    delete cloned;
}

GAME_TEST(CTaskSimpleTired, GetTaskType_ReturnsCorrect) {
    CTaskSimpleTired task(3000);
    EXPECT_EQ(task.GetTaskType(), TASK_SIMPLE_TIRED);
}

GAME_TEST(CTaskSimpleTired, Clone_PreservesType2) {
    CTaskSimpleTired task(3000);
    auto* cloned = task.Clone();
    EXPECT_EQ(cloned->GetTaskType(), TASK_SIMPLE_TIRED);
    delete cloned;
}

GAME_TEST(CTaskSimplePlayerOnFire, GetTaskType_ReturnsCorrect) {
    CTaskSimplePlayerOnFire task;
    EXPECT_EQ(task.GetTaskType(), TASK_SIMPLE_PLAYER_ON_FIRE);
}

GAME_TEST(CTaskSimpleTurn180, GetTaskType_ReturnsCorrect) {
    CTaskSimpleTurn180 task;
    EXPECT_EQ(task.GetTaskType(), TASK_SIMPLE_TURN_180);
}

GAME_TEST(CTaskSimpleShakeFist, GetTaskType_ReturnsCorrect) {
    CTaskSimpleShakeFist task;
    EXPECT_EQ(task.GetTaskType(), TASK_SIMPLE_SHAKE_FIST);
}

GAME_TEST(CTaskSimpleClearLookAt, GetTaskType_ReturnsCorrect) {
    CTaskSimpleClearLookAt task;
    EXPECT_EQ(task.GetTaskType(), TASK_SIMPLE_CLEAR_LOOK_AT);
}

GAME_TEST(CTaskSimpleDuckToggle, GetTaskType_ReturnsCorrect) {
    CTaskSimpleDuckToggle task(CTaskSimpleDuckToggle::eMode::OFF);
    EXPECT_EQ(task.GetTaskType(), TASK_SIMPLE_DUCK_TOGGLE);
}

GAME_TEST(CTaskSimpleFinishBrain, GetTaskType_ReturnsCorrect) {
    CTaskSimpleFinishBrain task;
    EXPECT_EQ(task.GetTaskType(), TASK_SIMPLE_FINISH_BRAIN);
}

GAME_TEST(CTaskSimpleIKManager, GetTaskType_ReturnsCorrect) {
    CTaskSimpleIKManager task;
    EXPECT_EQ(task.GetTaskType(), TASK_SIMPLE_IK_MANAGER);
}

GAME_TEST(CTaskSimpleFacial, GetTaskType_ReturnsCorrect) {
    CTaskSimpleFacial task(eFacialExpression::ANGER, 1000);
    EXPECT_EQ(task.GetTaskType(), TASK_SIMPLE_FACIAL);
}

// ---------------------------------------------------------------------------
// Differential tests — compare reversed GetTaskType() vs original (hooked)
// Only for tasks that have RH_ScopedVMTInstall(GetTaskType, ...) in their .cpp
// ---------------------------------------------------------------------------

GAME_DIFF_TEST(CTaskSimplePlayerOnFire, GetTaskType) {
    SuspendOtherThreads freeze;
    CTaskSimplePlayerOnFire task;

    eTaskType orig;
    {
        HookDisableGuard guard("Tasks/TaskTypes/CTaskSimplePlayerOnFire/GetTaskType");
        orig = task.GetTaskType();
    }
    eTaskType rev = task.GetTaskType();
    EXPECT_EQ(orig, rev);
    EXPECT_EQ(orig, TASK_SIMPLE_PLAYER_ON_FIRE);
}

GAME_DIFF_TEST(CTaskSimpleTurn180, GetTaskType) {
    SuspendOtherThreads freeze;
    CTaskSimpleTurn180 task;

    eTaskType orig;
    {
        HookDisableGuard guard("Tasks/TaskTypes/CTaskSimpleTurn180/GetTaskType");
        orig = task.GetTaskType();
    }
    eTaskType rev = task.GetTaskType();
    EXPECT_EQ(orig, rev);
    EXPECT_EQ(orig, TASK_SIMPLE_TURN_180);
}

GAME_DIFF_TEST(CTaskSimpleShakeFist, GetTaskType) {
    SuspendOtherThreads freeze;
    CTaskSimpleShakeFist task;

    eTaskType orig;
    {
        HookDisableGuard guard("Tasks/TaskTypes/CTaskSimpleShakeFist/GetTaskType");
        orig = task.GetTaskType();
    }
    eTaskType rev = task.GetTaskType();
    EXPECT_EQ(orig, rev);
    EXPECT_EQ(orig, TASK_SIMPLE_SHAKE_FIST);
}

GAME_DIFF_TEST(CTaskSimpleClearLookAt, GetTaskType) {
    SuspendOtherThreads freeze;
    CTaskSimpleClearLookAt task;

    eTaskType orig;
    {
        HookDisableGuard guard("Tasks/TaskTypes/CTaskSimpleClearLookAt/GetTaskType");
        orig = task.GetTaskType();
    }
    eTaskType rev = task.GetTaskType();
    EXPECT_EQ(orig, rev);
    EXPECT_EQ(orig, TASK_SIMPLE_CLEAR_LOOK_AT);
}

GAME_DIFF_TEST(CTaskSimpleDuckToggle, GetTaskType) {
    SuspendOtherThreads freeze;
    CTaskSimpleDuckToggle task(CTaskSimpleDuckToggle::eMode::AUTO);

    eTaskType orig;
    {
        HookDisableGuard guard("Tasks/TaskTypes/CTaskSimpleDuckToggle/GetTaskType");
        orig = task.GetTaskType();
    }
    eTaskType rev = task.GetTaskType();
    EXPECT_EQ(orig, rev);
    EXPECT_EQ(orig, TASK_SIMPLE_DUCK_TOGGLE);
}

GAME_DIFF_TEST(CTaskSimpleFinishBrain, GetTaskType) {
    SuspendOtherThreads freeze;
    CTaskSimpleFinishBrain task;

    eTaskType orig;
    {
        HookDisableGuard guard("Tasks/TaskTypes/CTaskSimpleFinishBrain/GetTaskType");
        orig = task.GetTaskType();
    }
    eTaskType rev = task.GetTaskType();
    EXPECT_EQ(orig, rev);
    EXPECT_EQ(orig, TASK_SIMPLE_FINISH_BRAIN);
}

GAME_DIFF_TEST(CTaskSimpleIKManager, GetTaskType) {
    SuspendOtherThreads freeze;
    CTaskSimpleIKManager task;

    eTaskType orig;
    {
        HookDisableGuard guard("Tasks/TaskTypes/CTaskSimpleIKManager/GetTaskType");
        orig = task.GetTaskType();
    }
    eTaskType rev = task.GetTaskType();
    EXPECT_EQ(orig, rev);
    EXPECT_EQ(orig, TASK_SIMPLE_IK_MANAGER);
}

GAME_DIFF_TEST(CTaskSimpleFacial, GetTaskType) {
    SuspendOtherThreads freeze;
    CTaskSimpleFacial task(eFacialExpression::HAPPINESS, 2000);

    eTaskType orig;
    {
        HookDisableGuard guard("Tasks/TaskTypes/CTaskSimpleFacial/GetTaskType");
        orig = task.GetTaskType();
    }
    eTaskType rev = task.GetTaskType();
    EXPECT_EQ(orig, rev);
    EXPECT_EQ(orig, TASK_SIMPLE_FACIAL);
}

// ---------------------------------------------------------------------------
// Differential tests for Clone() — for tasks with hooked Clone
// ---------------------------------------------------------------------------

GAME_DIFF_TEST(CTaskSimplePlayerOnFire, Clone) {
    SuspendOtherThreads freeze;
    CTaskSimplePlayerOnFire task;

    CTask* origClone;
    {
        HookDisableGuard guard("Tasks/TaskTypes/CTaskSimplePlayerOnFire/Clone");
        origClone = task.Clone();
    }
    CTask* revClone = task.Clone();

    EXPECT_EQ(origClone->GetTaskType(), revClone->GetTaskType());
    EXPECT_EQ(origClone->GetTaskType(), TASK_SIMPLE_PLAYER_ON_FIRE);

    delete origClone;
    delete revClone;
}

GAME_DIFF_TEST(CTaskSimpleTurn180, Clone) {
    SuspendOtherThreads freeze;
    CTaskSimpleTurn180 task;

    CTask* origClone;
    {
        HookDisableGuard guard("Tasks/TaskTypes/CTaskSimpleTurn180/Clone");
        origClone = task.Clone();
    }
    CTask* revClone = task.Clone();

    EXPECT_EQ(origClone->GetTaskType(), revClone->GetTaskType());
    EXPECT_EQ(origClone->GetTaskType(), TASK_SIMPLE_TURN_180);

    delete origClone;
    delete revClone;
}

GAME_DIFF_TEST(CTaskSimpleFacial, Clone) {
    SuspendOtherThreads freeze;
    CTaskSimpleFacial task(eFacialExpression::SADNESS, 3000);

    CTask* origClone;
    {
        HookDisableGuard guard("Tasks/TaskTypes/CTaskComplexFacial/Clone");  // Note: CTaskSimpleFacial doesn't have its own Clone hook
        origClone = task.Clone();
    }
    CTask* revClone = task.Clone();

    // Both should produce a CTaskSimpleFacial
    EXPECT_EQ(origClone->GetTaskType(), TASK_SIMPLE_FACIAL);
    EXPECT_EQ(revClone->GetTaskType(), TASK_SIMPLE_FACIAL);

    delete origClone;
    delete revClone;
}
