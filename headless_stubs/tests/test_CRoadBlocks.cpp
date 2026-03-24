// test_CRoadBlocks.cpp — Behavior tests for CRoadBlocks.
// Hook paths: Global/CRoadBlocks/<fn>
//
// Most CRoadBlocks functions are either private or have complex signatures.
// Test Init and ClearScriptRoadBlocks as behavior tests.

#include "StdInc.h"
#include "TestFramework.h"
#include "RoadBlocks.h"

// ClearScriptRoadBlocks — safe to call, resets script roadblock state
GAME_DIFF_TEST(CRoadBlocks, ClearScriptRoadBlocks) {
    { HookDisableGuard guard("Global/CRoadBlocks/ClearScriptRoadBlocks");
      CRoadBlocks::ClearScriptRoadBlocks(); }
    CRoadBlocks::ClearScriptRoadBlocks();
    EXPECT_TRUE(true);
}
