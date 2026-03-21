// test_CTagManager.cpp — Differential tests for CTagManager.
// Hook paths: Global/CTagManager/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "TagManager.h"

GAME_DIFF_TEST(CTagManager, GetPercentageTagged) {
    int32 orig, rev;
    { HookDisableGuard guard("Global/CTagManager/GetPercentageTagged"); orig = CTagManager::GetPercentageTagged(); }
    rev = CTagManager::GetPercentageTagged();
    EXPECT_EQ(orig, rev);
}
