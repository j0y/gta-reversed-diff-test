// test_CWorld6.cpp — CWorld sector and cleanup queries.
// Hook paths: Global/CWorld/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "World.h"

// --- GetCurrentScanCode ---

GAME_DIFF_TEST(CWorld6, GetCurrentScanCode) {
    uint16 orig, rev;
    { HookDisableGuard guard("Global/CWorld/GetCurrentScanCode"); orig = CWorld::GetCurrentScanCode(); }
    rev = CWorld::GetCurrentScanCode();
    EXPECT_EQ(orig, rev);
}

// --- RemoveFallenPeds ---

GAME_DIFF_TEST(CWorld6, RemoveFallenPeds_NoCrash) {
    { HookDisableGuard guard("Global/CWorld/RemoveFallenPeds"); CWorld::RemoveFallenPeds(); }
    CWorld::RemoveFallenPeds();
    EXPECT_TRUE(true);
}

// --- RemoveFallenCars ---

GAME_DIFF_TEST(CWorld6, RemoveFallenCars_NoCrash) {
    { HookDisableGuard guard("Global/CWorld/RemoveFallenCars"); CWorld::RemoveFallenCars(); }
    CWorld::RemoveFallenCars();
    EXPECT_TRUE(true);
}

// RemoveStaticObjects — original code crashes (accesses rendering state). Skipped.

// ClearCarsFromArea / ClearPedsFromArea take different arg counts. Skipped.
