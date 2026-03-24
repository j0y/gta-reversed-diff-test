// test_CAnimBlendAssocGroup.cpp — Differential tests for CAnimBlendAssocGroup.
// Hook paths: Animation/CAnimBlendAssocGroup/<fn>
// GetAnimAssocGroups not available — access via CAnimManager::GetAnimation instead.
// Keep as placeholder for when API is clarified.

#include "StdInc.h"
#include "TestFramework.h"
#include "Animation/AnimManager.h"

// Test GetAnimationBlockIndex — available on CAnimManager
GAME_DIFF_TEST(CAnimBlendAssocGroup, GetAnimationBlockIndex_Names) {
    const char* names[] = { "ped", "default", "fatman", "muscular" };
    for (auto* name : names) {
        int32 orig, rev;
        { HookDisableGuard guard("Animation/CAnimManager/GetAnimationBlockIndex-by-name");
          orig = CAnimManager::GetAnimationBlockIndex(name); }
        rev = CAnimManager::GetAnimationBlockIndex(name);
        EXPECT_EQ(orig, rev);
    }
}
