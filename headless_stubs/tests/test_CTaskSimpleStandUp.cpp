// test_CTaskSimpleStandUp.cpp

#include "StdInc.h"
#include "TestFramework.h"
#include "Tasks/TaskTypes/TaskSimpleStandUp.h"

GAME_TEST(CTaskSimpleStandUp, GetTaskType) {
    CTaskSimpleStandUp task(false);
    EXPECT_EQ((int32)task.GetTaskType(), (int32)TASK_SIMPLE_STAND_UP);
}

GAME_TEST(CTaskSimpleStandUp, IsSimple) {
    CTaskSimpleStandUp task(false);
    EXPECT_TRUE(task.IsSimple());
}

GAME_TEST(CTaskSimpleStandUp, Constructor_NoSitAfterStep) {
    CTaskSimpleStandUp task(false);
    EXPECT_FALSE(task.m_sitAfterStep);
    EXPECT_FALSE(task.m_hasAnimFinished);
}

GAME_TEST(CTaskSimpleStandUp, Constructor_SitAfterStep) {
    CTaskSimpleStandUp task(true);
    EXPECT_TRUE(task.m_sitAfterStep);
}

GAME_TEST(CTaskSimpleStandUp, Clone) {
    CTaskSimpleStandUp original(true);
    CTask* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;
    EXPECT_EQ((int32)cloned->GetTaskType(), (int32)TASK_SIMPLE_STAND_UP);
    auto* c = static_cast<CTaskSimpleStandUp*>(cloned);
    EXPECT_TRUE(c->m_sitAfterStep);
    delete cloned;
}
