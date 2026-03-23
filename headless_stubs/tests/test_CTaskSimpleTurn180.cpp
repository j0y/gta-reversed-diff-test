// test_CTaskSimpleTurn180.cpp

#include "StdInc.h"
#include "TestFramework.h"
#include "Tasks/TaskTypes/TaskSimpleTurn180.h"

GAME_TEST(CTaskSimpleTurn180, GetTaskType) {
    CTaskSimpleTurn180 task;
    EXPECT_EQ((int32)task.GetTaskType(), (int32)TASK_SIMPLE_TURN_180);
}

GAME_TEST(CTaskSimpleTurn180, IsSimple) {
    CTaskSimpleTurn180 task;
    EXPECT_TRUE(task.IsSimple());
}

GAME_TEST(CTaskSimpleTurn180, MakeAbortable_AlwaysFalse) {
    CTaskSimpleTurn180 task;
    EXPECT_FALSE(task.MakeAbortable(nullptr));
}

GAME_TEST(CTaskSimpleTurn180, Clone) {
    CTaskSimpleTurn180 original;
    CTask* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;
    EXPECT_EQ((int32)cloned->GetTaskType(), (int32)TASK_SIMPLE_TURN_180);
    delete cloned;
}

GAME_TEST(CTaskSimpleTurn180, InitialState) {
    CTaskSimpleTurn180 task;
    EXPECT_FALSE(task.m_animHasFinished);
}
