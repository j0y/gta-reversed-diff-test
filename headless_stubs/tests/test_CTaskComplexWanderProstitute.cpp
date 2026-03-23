// test_CTaskComplexWanderProstitute.cpp

#include "StdInc.h"
#include "TestFramework.h"
#include "Tasks/TaskTypes/TaskComplexWanderProstitute.h"

GAME_TEST(CTaskComplexWanderProstitute, GetTaskType) {
    CTaskComplexWanderProstitute task(PEDMOVE_WALK, 0);
    EXPECT_EQ((int32)task.GetTaskType(), (int32)TASK_COMPLEX_WANDER);
}

GAME_TEST(CTaskComplexWanderProstitute, GetWanderType) {
    CTaskComplexWanderProstitute task(PEDMOVE_WALK, 0);
    EXPECT_EQ((int32)task.GetWanderType(), (int32)WANDER_TYPE_PROSTITUTE);
}

GAME_TEST(CTaskComplexWanderProstitute, Clone_PreservesFields) {
    CTaskComplexWanderProstitute original(PEDMOVE_RUN, 5, true);
    CTask* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;
    auto* c = static_cast<CTaskComplexWanderProstitute*>(cloned);
    EXPECT_EQ((int32)c->GetWanderType(), (int32)WANDER_TYPE_PROSTITUTE);
    EXPECT_EQ((int32)c->m_nMoveState, (int32)PEDMOVE_RUN);
    EXPECT_EQ(c->m_nDir, (uint8)5);
    delete cloned;
}

GAME_TEST(CTaskComplexWanderProstitute, IsComplex) {
    CTaskComplexWanderProstitute task(PEDMOVE_WALK, 0);
    EXPECT_TRUE(task.IsComplex());
}
