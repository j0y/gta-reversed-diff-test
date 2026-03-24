// test_CShotInfo2.cpp — Extended differential tests for CShotInfo.
// Hook paths: Global/CShotInfo/<fn>
//
// Tests shot info queries including edge cases.

#include "StdInc.h"
#include "TestFramework.h"
#include "ShotInfo.h"

// --- GetFlameThrowerShotPosn extended ---

GAME_DIFF_TEST(CShotInfo, GetFlameThrowerShotPosn_AllSlots_Extended) {
    // Test all 100 shot slots (NUMSHOTINFOS)
    for (uint8 id = 0; id < 100; id++) {
        CVector origPos{}, revPos{};
        bool orig, rev;
        { HookDisableGuard guard("Global/CShotInfo/GetFlameThrowerShotPosn");
          orig = CShotInfo::GetFlameThrowerShotPosn(id, origPos); }
        rev = CShotInfo::GetFlameThrowerShotPosn(id, revPos);
        EXPECT_EQ(orig, rev);
        if (orig && rev) {
            EXPECT_NEAR(origPos.x, revPos.x, 1e-3f);
            EXPECT_NEAR(origPos.y, revPos.y, 1e-3f);
            EXPECT_NEAR(origPos.z, revPos.z, 1e-3f);
        }
    }
}
