// test_CRopes.cpp — Differential tests for CRopes.
// Hook paths: Global/CRopes/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "Ropes.h"

GAME_DIFF_TEST(CRopes, FindRope) {
    // No ropes exist at state 9 — should return -1 for all IDs
    uint32 ids[] = { 0, 1, 2, 100, 1000 };
    for (auto id : ids) {
        int32 orig, rev;
        { HookDisableGuard guard("Global/CRopes/FindRope"); orig = CRopes::FindRope(id); }
        rev = CRopes::FindRope(id);
        EXPECT_EQ(orig, rev);
    }
}
