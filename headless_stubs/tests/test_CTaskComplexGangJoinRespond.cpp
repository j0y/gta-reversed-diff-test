// test_CTaskComplexGangJoinRespond.cpp

#include "StdInc.h"
#include "TestFramework.h"
#include "Tasks/TaskTypes/TaskComplexGangJoinRespond.h"

GAME_TEST(CTaskComplexGangJoinRespond, GetTaskType) {
    CTaskComplexGangJoinRespond task(true);
    EXPECT_EQ((int32)task.GetTaskType(), (int32)TASK_COMPLEX_GANG_JOIN_RESPOND);
}

GAME_TEST(CTaskComplexGangJoinRespond, IsComplex) {
    CTaskComplexGangJoinRespond task(false);
    EXPECT_TRUE(task.IsComplex());
}

GAME_TEST(CTaskComplexGangJoinRespond, Constructor_Accept) {
    CTaskComplexGangJoinRespond task(true);
    EXPECT_TRUE(task.m_response);
}

GAME_TEST(CTaskComplexGangJoinRespond, Constructor_Reject) {
    CTaskComplexGangJoinRespond task(false);
    EXPECT_FALSE(task.m_response);
}

GAME_TEST(CTaskComplexGangJoinRespond, MakeAbortable_AlwaysTrue) {
    CTaskComplexGangJoinRespond task(true);
    EXPECT_TRUE(task.MakeAbortable(nullptr));
}

GAME_TEST(CTaskComplexGangJoinRespond, Clone_PreservesResponse) {
    CTaskComplexGangJoinRespond original(true);
    CTask* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;
    EXPECT_EQ((int32)cloned->GetTaskType(), (int32)TASK_COMPLEX_GANG_JOIN_RESPOND);
    auto* c = static_cast<CTaskComplexGangJoinRespond*>(cloned);
    EXPECT_TRUE(c->m_response);
    delete cloned;
}
