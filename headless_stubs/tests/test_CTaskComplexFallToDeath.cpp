// test_CTaskComplexFallToDeath.cpp

#include "StdInc.h"
#include "TestFramework.h"
#include "Tasks/TaskTypes/TaskComplexFallToDeath.h"

GAME_TEST(CTaskComplexFallToDeath, GetTaskType) {
    CVector pos(0, 0, 50.0f);
    CTaskComplexFallToDeath task((int32)eDirection::FORWARD, pos, false, false);
    EXPECT_EQ((int32)task.GetTaskType(), (int32)TASK_COMPLEX_FALL_TO_DEATH);
}

GAME_TEST(CTaskComplexFallToDeath, IsComplex) {
    CVector pos(0, 0, 50.0f);
    CTaskComplexFallToDeath task((int32)eDirection::FORWARD, pos, false, false);
    EXPECT_TRUE(task.IsComplex());
}

GAME_TEST(CTaskComplexFallToDeath, Constructor_Fields) {
    CVector pos(10.0f, 20.0f, 100.0f);
    CTaskComplexFallToDeath task((int32)eDirection::LEFT, pos, true, false);
    EXPECT_EQ((int32)task.m_nFallToDeathDir, (int32)eDirection::LEFT);
    EXPECT_NEAR(task.m_Posn.x, 10.0f, 1e-5f);
    EXPECT_NEAR(task.m_Posn.z, 100.0f, 1e-5f);
}

GAME_TEST(CTaskComplexFallToDeath, Clone_PreservesType) {
    CVector pos(0, 0, 50.0f);
    CTaskComplexFallToDeath original((int32)eDirection::RIGHT, pos, false, true);
    CTask* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;
    EXPECT_EQ((int32)cloned->GetTaskType(), (int32)TASK_COMPLEX_FALL_TO_DEATH);
    delete cloned;
}

GAME_TEST(CTaskComplexFallToDeath, DifferentDirections) {
    CVector pos(0, 0, 0);
    int32 dirs[] = { (int32)eDirection::FORWARD, (int32)eDirection::LEFT, (int32)eDirection::BACKWARD, (int32)eDirection::RIGHT };
    for (int32 d : dirs) {
        CTaskComplexFallToDeath task(d, pos, false, false);
        EXPECT_EQ((int32)task.m_nFallToDeathDir, d);
    }
}
