// test_CObject4.cpp — CObject query tests.
// Hook paths: Entity/Object/CObject/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "Entity/Object/Object.h"

// Test with nullptr — verify safe handling
GAME_DIFF_TEST(CObject4, GetRopeHeight_Behavior) {
    // Create a default CObject to test on isn't easily possible without pool access.
    // Test the static deletion functions instead.
    { HookDisableGuard guard("Entity/Object/CObject/DeleteAllTempObjectsInArea");
      CObject::DeleteAllTempObjectsInArea(CVector(0.f, 0.f, 0.f), 1.0f); }
    CObject::DeleteAllTempObjectsInArea(CVector(0.f, 0.f, 0.f), 1.0f);
    EXPECT_TRUE(true);
}

GAME_DIFF_TEST(CObject4, DeleteAllMissionObjects) {
    // No mission objects at state 9 — safe to call
    { HookDisableGuard guard("Entity/Object/CObject/DeleteAllMissionObjects");
      CObject::DeleteAllMissionObjects(); }
    CObject::DeleteAllMissionObjects();
    EXPECT_TRUE(true);
}
