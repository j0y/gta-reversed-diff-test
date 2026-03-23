// test_CTaskComplexTurnToFaceEntityOrCoord.cpp

#include "StdInc.h"
#include "TestFramework.h"
#include "Tasks/TaskTypes/TaskComplexTurnToFaceEntityOrCoord.h"
#include "Entity/Ped/Ped.h"

GAME_TEST(CTaskComplexTurnToFaceEntityOrCoord, GetTaskType_Coords) {
    CVector coords(100.0f, 200.0f, 10.0f);
    CTaskComplexTurnToFaceEntityOrCoord task(coords);
    EXPECT_EQ((int32)task.GetTaskType(), (int32)TASK_COMPLEX_TURN_TO_FACE_ENTITY);
}

GAME_TEST(CTaskComplexTurnToFaceEntityOrCoord, GetTaskType_Entity) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    CTaskComplexTurnToFaceEntityOrCoord task(player);
    EXPECT_EQ((int32)task.GetTaskType(), (int32)TASK_COMPLEX_TURN_TO_FACE_ENTITY);
}

GAME_TEST(CTaskComplexTurnToFaceEntityOrCoord, IsComplex) {
    CVector coords(0, 0, 0);
    CTaskComplexTurnToFaceEntityOrCoord task(coords);
    EXPECT_TRUE(task.IsComplex());
}

GAME_TEST(CTaskComplexTurnToFaceEntityOrCoord, Clone_Coords) {
    CVector coords(50.0f, 60.0f, 70.0f);
    CTaskComplexTurnToFaceEntityOrCoord original(coords, 0.8f, 1.5f);
    CTask* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;
    EXPECT_EQ((int32)cloned->GetTaskType(), (int32)TASK_COMPLEX_TURN_TO_FACE_ENTITY);
    delete cloned;
}

GAME_TEST(CTaskComplexTurnToFaceEntityOrCoord, Clone_Entity) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    CTaskComplexTurnToFaceEntityOrCoord original(player, 0.5f, 1.0f);
    CTask* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;
    EXPECT_EQ((int32)cloned->GetTaskType(), (int32)TASK_COMPLEX_TURN_TO_FACE_ENTITY);
    delete cloned;
}
