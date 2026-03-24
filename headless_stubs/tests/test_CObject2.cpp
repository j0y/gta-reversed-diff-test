// test_CObject2.cpp — Behavior tests for CObject.
// CObject hook path "Global/CObject/DeleteAllTempObjectsInArea" not found at runtime.
// Test as behavior only.

#include "StdInc.h"
#include "TestFramework.h"
#include "Entity/Object/Object.h"

GAME_TEST(CObject2, DeleteAllTempObjectsInArea_NoCrash) {
    CObject::DeleteAllTempObjectsInArea(CVector(0.f, 0.f, 0.f), 1.0f);
    EXPECT_TRUE(true);
}
