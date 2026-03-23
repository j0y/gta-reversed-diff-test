// test_CStreaming4.cpp — Extended differential tests for CStreaming.
// Hook paths: Global/CStreaming/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "Streaming.h"

// --- GetDiscInDrive ---

// GetDiscInDrive hook not found at runtime — likely inline or different path

// --- FindMIPedSlotForInterior ---

GAME_DIFF_TEST(CStreaming4, FindMIPedSlotForInterior_Various) {
    int32 factors[] = { 0, 1, 5, 10, 42, 100, 255 };
    for (int32 f : factors) {
        int32 orig, rev;
        { HookDisableGuard guard("Global/CStreaming/FindMIPedSlotForInterior"); orig = CStreaming::FindMIPedSlotForInterior(f); }
        rev = CStreaming::FindMIPedSlotForInterior(f);
        EXPECT_EQ(orig, rev);
    }
}

// --- IsObjectInCdImage ---

// IsObjectInCdImage hook not found at runtime — likely inline or different path
