// test_CTaskComplexHitByGunResponse.cpp

#include "StdInc.h"
#include "TestFramework.h"
#include "Tasks/TaskTypes/TaskComplexHitByGunResponse.h"

GAME_TEST(CTaskComplexHitByGunResponse, GetTaskType) {
    CTaskComplexHitByGunResponse task(eDirection::FORWARD);
    EXPECT_EQ((int32)task.GetTaskType(), (int32)TASK_COMPLEX_HIT_BY_GUN_RESPONSE);
}

GAME_TEST(CTaskComplexHitByGunResponse, IsComplex) {
    CTaskComplexHitByGunResponse task(eDirection::FORWARD);
    EXPECT_TRUE(task.IsComplex());
}

GAME_TEST(CTaskComplexHitByGunResponse, Constructor_Direction) {
    CTaskComplexHitByGunResponse task(eDirection::BACKWARD);
    EXPECT_EQ((int32)(eDirection)task.m_HitSide, (int32)eDirection::BACKWARD);
}

GAME_TEST(CTaskComplexHitByGunResponse, Clone_PreservesDirection) {
    CTaskComplexHitByGunResponse original(eDirection::LEFT);
    CTask* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;
    EXPECT_EQ((int32)cloned->GetTaskType(), (int32)TASK_COMPLEX_HIT_BY_GUN_RESPONSE);
    auto* c = static_cast<CTaskComplexHitByGunResponse*>(cloned);
    EXPECT_EQ((int32)(eDirection)c->m_HitSide, (int32)eDirection::LEFT);
    delete cloned;
}

GAME_TEST(CTaskComplexHitByGunResponse, CreateNextSubTask_ReturnsNull) {
    CTaskComplexHitByGunResponse task(eDirection::FORWARD);
    EXPECT_EQ((uintptr_t)task.CreateNextSubTask(nullptr), (uintptr_t)nullptr);
}
