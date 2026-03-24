// test_CEscalators.cpp — Behavior tests for CEscalators.
// Hook paths: Global/CEscalators/<fn>
//
// Escalator state management functions. Tests verify no crash and
// consistent behavior at state 9.

#include "StdInc.h"
#include "TestFramework.h"
#include "Escalators.h"

// AddOne creates an escalator — verify no crash with valid geometry
GAME_TEST(CEscalators, AddOne_NoCrash) {
    CVector start(2488.f, -1666.f, 13.f);
    CVector bottom(2488.f, -1666.f, 13.f);
    CVector top(2490.f, -1666.f, 16.f);
    CVector end(2492.f, -1666.f, 16.f);
    // Add one escalator — should not crash even with no rendering
    CEscalators::AddOne(start, bottom, top, end, true, nullptr);
    EXPECT_TRUE(true);
}
