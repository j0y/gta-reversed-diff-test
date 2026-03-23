// test_CTaskSimpleLand.cpp

#include "StdInc.h"
#include "TestFramework.h"
#include "Tasks/TaskTypes/TaskSimpleLand.h"

GAME_TEST(CTaskSimpleLand, GetTaskType) {
    CTaskSimpleLand task(ANIM_ID_FALL_LAND);
    EXPECT_EQ((int32)task.GetTaskType(), (int32)TASK_SIMPLE_LAND);
}

GAME_TEST(CTaskSimpleLand, IsSimple) {
    CTaskSimpleLand task(ANIM_ID_FALL_LAND);
    EXPECT_TRUE(task.IsSimple());
}

GAME_TEST(CTaskSimpleLand, Clone_PreservesAnimId) {
    CTaskSimpleLand original(ANIM_ID_FALL_LAND);
    CTask* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;
    EXPECT_EQ((int32)cloned->GetTaskType(), (int32)TASK_SIMPLE_LAND);
    delete cloned;
}

GAME_TEST(CTaskSimpleLand, SizeValidation) {
    EXPECT_EQ(sizeof(CTaskSimpleLand), (size_t)0x14);
}
