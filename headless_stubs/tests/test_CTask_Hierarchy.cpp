// test_CTask_Hierarchy.cpp — Tests for CTask base class hierarchy behavior.
// No specific hook paths — tests the virtual dispatch and type system.
//
// Verifies that the task type system works correctly: IsSimple/IsComplex,
// GetSubTask returns correct values, and the polymorphic dispatch chain
// is intact across different task types.

#include "StdInc.h"
#include "TestFramework.h"
#include "Tasks/Task.h"
#include "Tasks/TaskSimple.h"
#include "Tasks/TaskComplex.h"
#include "Tasks/TaskTypes/TaskSimpleStandStill.h"
#include "Tasks/TaskTypes/TaskSimpleDuck.h"
#include "Tasks/TaskTypes/TaskSimpleGoToPoint.h"
#include "Tasks/TaskTypes/TaskComplexDie.h"
#include "Tasks/TaskTypes/TaskComplexWanderStandard.h"

// --- IsSimple / IsComplex across types ---

GAME_TEST(CTask_Hierarchy, SimpleTasksAreSimple) {
    CTaskSimpleStandStill standStill;
    CTaskSimpleDuck duck(DUCK_STANDALONE, 2000);
    CVector target(0, 0, 0);
    CTaskSimpleGoToPoint goTo(PEDMOVE_WALK, target);

    EXPECT_TRUE(standStill.IsSimple());
    EXPECT_FALSE(standStill.IsComplex());

    EXPECT_TRUE(duck.IsSimple());
    EXPECT_FALSE(duck.IsComplex());

    EXPECT_TRUE(goTo.IsSimple());
    EXPECT_FALSE(goTo.IsComplex());
}

GAME_TEST(CTask_Hierarchy, ComplexTasksAreComplex) {
    CTaskComplexDie die(WEAPON_UNARMED, ANIM_GROUP_DEFAULT, ANIM_ID_KO_SHOT_FRONT_0);
    CTaskComplexWanderStandard wander(PEDMOVE_WALK, 0);

    EXPECT_FALSE(die.IsSimple());
    EXPECT_TRUE(die.IsComplex());

    EXPECT_FALSE(wander.IsSimple());
    EXPECT_TRUE(wander.IsComplex());
}

// --- GetSubTask ---

GAME_TEST(CTask_Hierarchy, SimpleTask_GetSubTask_Null) {
    CTaskSimpleStandStill task;
    EXPECT_EQ((uintptr_t)task.GetSubTask(), (uintptr_t)nullptr);
}

GAME_TEST(CTask_Hierarchy, ComplexTask_GetSubTask_InitiallyNull) {
    CTaskComplexDie task(WEAPON_UNARMED, ANIM_GROUP_DEFAULT, ANIM_ID_KO_SHOT_FRONT_0);
    EXPECT_EQ((uintptr_t)task.GetSubTask(), (uintptr_t)nullptr);
}

// --- GetTaskType uniqueness ---

GAME_TEST(CTask_Hierarchy, TaskTypes_AreDistinct) {
    CTaskSimpleStandStill standStill;
    CTaskSimpleDuck duck(DUCK_STANDALONE, 2000);
    CVector target(0, 0, 0);
    CTaskSimpleGoToPoint goTo(PEDMOVE_WALK, target);
    CTaskComplexDie die(WEAPON_UNARMED, ANIM_GROUP_DEFAULT, ANIM_ID_KO_SHOT_FRONT_0);
    CTaskComplexWanderStandard wander(PEDMOVE_WALK, 0);

    int32 types[] = {
        (int32)standStill.GetTaskType(),
        (int32)duck.GetTaskType(),
        (int32)goTo.GetTaskType(),
        (int32)die.GetTaskType(),
        (int32)wander.GetTaskType()
    };

    // All types should be distinct
    for (int i = 0; i < 5; i++) {
        for (int j = i + 1; j < 5; j++) {
            EXPECT_NE(types[i], types[j]);
        }
    }
}

// --- Clone preserves type through base pointer ---

GAME_TEST(CTask_Hierarchy, Clone_PreservesType_ViaBasePtr) {
    CTaskSimpleStandStill standStill(1000, true, false, 4.0f);
    CTask* base = &standStill;

    CTask* cloned = base->Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;

    EXPECT_EQ((int32)cloned->GetTaskType(), (int32)base->GetTaskType());
    EXPECT_EQ(cloned->IsSimple(), base->IsSimple());

    delete cloned;
}

GAME_TEST(CTask_Hierarchy, Clone_PreservesType_Complex) {
    CTaskComplexDie die(WEAPON_AK47, ANIM_GROUP_DEFAULT, ANIM_ID_KO_SHOT_FRONT_0, 8.0f, 1.0f, true);
    CTask* base = &die;

    CTask* cloned = base->Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;

    EXPECT_EQ((int32)cloned->GetTaskType(), (int32)TASK_COMPLEX_DIE);
    EXPECT_TRUE(cloned->IsComplex());

    delete cloned;
}

// --- Parent tracking ---

GAME_TEST(CTask_Hierarchy, FreshTask_NoParent) {
    CTaskSimpleStandStill task;
    EXPECT_EQ((uintptr_t)task.GetParent(), (uintptr_t)nullptr);
}

// --- IsGoToTask static helper ---

GAME_TEST(CTask_Hierarchy, IsGoToTask_GoToPoint) {
    CVector target(10.0f, 20.0f, 5.0f);
    CTaskSimpleGoToPoint task(PEDMOVE_WALK, target);
    EXPECT_TRUE(CTask::IsGoToTask(&task));
}

GAME_TEST(CTask_Hierarchy, IsGoToTask_NonGoToTask) {
    CTaskSimpleStandStill task;
    EXPECT_FALSE(CTask::IsGoToTask(&task));
}

// IsGoToTask(nullptr) dereferences the pointer — skip, not safe to call with null
// GAME_TEST(CTask_Hierarchy, IsGoToTask_Null) {
//     EXPECT_FALSE(CTask::IsGoToTask(nullptr));
// }
