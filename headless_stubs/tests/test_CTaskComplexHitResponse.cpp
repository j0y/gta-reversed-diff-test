// test_CTaskComplexHitResponse.cpp

#include "StdInc.h"
#include "TestFramework.h"
#include "Tasks/TaskTypes/TaskComplexHitResponse.h"

GAME_TEST(CTaskComplexHitResponse, GetTaskType) {
    CTaskComplexHitResponse task(eDirection::FORWARD);
    EXPECT_EQ((int32)task.GetTaskType(), (int32)TASK_COMPLEX_HIT_RESPONSE);
}

GAME_TEST(CTaskComplexHitResponse, IsComplex) {
    CTaskComplexHitResponse task(eDirection::FORWARD);
    EXPECT_TRUE(task.IsComplex());
}

GAME_TEST(CTaskComplexHitResponse, Constructor_Direction) {
    CTaskComplexHitResponse task(eDirection::LEFT);
    EXPECT_EQ((int32)(eDirection)task.m_HitSide, (int32)eDirection::LEFT);
}

GAME_TEST(CTaskComplexHitResponse, Clone_PreservesDirection) {
    CTaskComplexHitResponse original(eDirection::RIGHT);
    CTask* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;
    EXPECT_EQ((int32)cloned->GetTaskType(), (int32)TASK_COMPLEX_HIT_RESPONSE);
    auto* c = static_cast<CTaskComplexHitResponse*>(cloned);
    EXPECT_EQ((int32)(eDirection)c->m_HitSide, (int32)eDirection::RIGHT);
    delete cloned;
}

GAME_TEST(CTaskComplexHitResponse, CreateNextSubTask_ReturnsNull) {
    CTaskComplexHitResponse task(eDirection::FORWARD);
    EXPECT_EQ((uintptr_t)task.CreateNextSubTask(nullptr), (uintptr_t)nullptr);
}

GAME_TEST(CTaskComplexHitResponse, DifferentDirections) {
    eDirection dirs[] = { eDirection::FORWARD, eDirection::LEFT, eDirection::BACKWARD, eDirection::RIGHT };
    for (auto d : dirs) {
        CTaskComplexHitResponse task(d);
        EXPECT_EQ((int32)(eDirection)task.m_HitSide, (int32)d);
    }
}
