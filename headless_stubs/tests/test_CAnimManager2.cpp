// test_CAnimManager2.cpp — Differential tests for CAnimManager query functions.
// Hook paths: Animation/CAnimManager/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "Animation/AnimManager.h"

// --- GetNumRefsToAnimBlock ---

GAME_DIFF_TEST(CAnimManager2, GetNumRefsToAnimBlock_Range) {
    for (int32 blockIdx = 0; blockIdx < 10; blockIdx++) {
        int32 orig, rev;
        { HookDisableGuard guard("Animation/CAnimManager/GetNumRefsToAnimBlock");
          orig = CAnimManager::GetNumRefsToAnimBlock(blockIdx); }
        rev = CAnimManager::GetNumRefsToAnimBlock(blockIdx);
        EXPECT_EQ(orig, rev);
    }
}
