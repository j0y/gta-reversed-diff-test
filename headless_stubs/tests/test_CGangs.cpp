// test_CGangs.cpp — Differential tests for CGangs.
// Hook paths: Global/CGangs/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "Gangs.h"
#include "PedType.h"

GAME_DIFF_TEST(CGangs, GetWillAttackPlayerWithCops) {
    // Test one gang type at a time to identify which one diverges
    for (int t = 7; t <= 16; t++) {
        auto pt = static_cast<ePedType>(t);
        bool orig, rev;
        { HookDisableGuard guard("Global/CGangs/GetWillAttackPlayerWithCops");
          orig = CGangs::GetWillAttackPlayerWithCops(pt); }
        rev = CGangs::GetWillAttackPlayerWithCops(pt);
        // Report which ped type fails
        if (orig != rev) {
            char buf[128];
            snprintf(buf, sizeof(buf), "Mismatch at pedType=%d: orig=%d rev=%d", t, (int)orig, (int)rev);
            GetTestContext().RecordFailure("test_CGangs.cpp", __LINE__, buf);
            return;
        }
        EXPECT_EQ(orig, rev);
    }
}
