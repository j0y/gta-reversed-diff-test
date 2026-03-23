// test_CTaskSimpleDead.cpp — Tests for CTaskSimpleDead.

#include "StdInc.h"
#include "TestFramework.h"
#include "Tasks/TaskTypes/TaskSimpleDead.h"

GAME_TEST(CTaskSimpleDead, GetTaskType) {
    CTaskSimpleDead task(0, false);
    EXPECT_EQ((int32)task.GetTaskType(), (int32)TASK_SIMPLE_DEAD);
}

GAME_TEST(CTaskSimpleDead, IsSimple) {
    CTaskSimpleDead task(0, false);
    EXPECT_TRUE(task.IsSimple());
}

GAME_TEST(CTaskSimpleDead, Constructor_NotDrowned) {
    CTaskSimpleDead task(1000, false);
    EXPECT_EQ(task.m_nDeathTimeMS, (uint32)1000);
    EXPECT_FALSE(task.m_bHasDrowned);
}

GAME_TEST(CTaskSimpleDead, Constructor_Drowned) {
    CTaskSimpleDead task(5000, true);
    EXPECT_EQ(task.m_nDeathTimeMS, (uint32)5000);
}

GAME_TEST(CTaskSimpleDead, MakeAbortable_AlwaysTrue) {
    CTaskSimpleDead task(0, false);
    EXPECT_TRUE(task.MakeAbortable(nullptr));
}

GAME_TEST(CTaskSimpleDead, Clone_PreservesFields) {
    CTaskSimpleDead original(3000, false);
    CTask* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;
    EXPECT_EQ((int32)cloned->GetTaskType(), (int32)TASK_SIMPLE_DEAD);
    auto* c = static_cast<CTaskSimpleDead*>(cloned);
    EXPECT_EQ(c->m_nDeathTimeMS, (uint32)3000);
    delete cloned;
}
