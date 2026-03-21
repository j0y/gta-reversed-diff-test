// test_CTheScripts.cpp — Differential tests for CTheScripts.
// Hook paths: Scripts/CTheScripts/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "Scripts/TheScripts.h"

GAME_DIFF_TEST(CTheScripts, IsPlayerOnAMission) {
    bool orig, rev;
    { HookDisableGuard guard("Scripts/CTheScripts/IsPlayerOnAMission"); orig = CTheScripts::IsPlayerOnAMission(); }
    rev = CTheScripts::IsPlayerOnAMission();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CTheScripts, HasCarModelBeenSuppressed) {
    // Test a range of vehicle model IDs
    int ids[] = { 400, 410, 420, 440, 480, 500, 560, 596, 611 };
    for (int id : ids) {
        bool orig, rev;
        { HookDisableGuard guard("Scripts/CTheScripts/HasCarModelBeenSuppressed");
          orig = CTheScripts::HasCarModelBeenSuppressed(static_cast<eModelID>(id)); }
        rev = CTheScripts::HasCarModelBeenSuppressed(static_cast<eModelID>(id));
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(CTheScripts, HasVehicleModelBeenBlockedByScript) {
    int ids[] = { 400, 410, 420, 440, 480, 500, 560, 596, 611 };
    for (int id : ids) {
        bool orig, rev;
        { HookDisableGuard guard("Scripts/CTheScripts/HasVehicleModelBeenBlockedByScript");
          orig = CTheScripts::HasVehicleModelBeenBlockedByScript(static_cast<eModelID>(id)); }
        rev = CTheScripts::HasVehicleModelBeenBlockedByScript(static_cast<eModelID>(id));
        EXPECT_EQ(orig, rev);
    }
}
