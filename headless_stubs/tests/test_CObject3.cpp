// test_CObject3.cpp — Differential tests for CObject functions.
// Hook paths: Entity/Object/CObject/<fn>
// Note: IsObjectPointerValid may be a free function, not a member.

#include "StdInc.h"
#include "TestFramework.h"
#include "Entity/Object/Object.h"

// Test DeleteAllTempObjects (no area restriction) — verify no crash

GAME_DIFF_TEST(CObject3, DeleteAllTempObjects_NoCrash) {
    // At state 9, there may be temp objects spawned by the world population system.
    // Calling this far from player shouldn't affect gameplay.
    { HookDisableGuard guard("Entity/Object/CObject/DeleteAllTempObjects");
      CObject::DeleteAllTempObjects(); }
    CObject::DeleteAllTempObjects();
    EXPECT_TRUE(true);
}
