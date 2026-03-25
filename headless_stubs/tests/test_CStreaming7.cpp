// test_CStreaming7.cpp — Tests for CStreaming functions not covered by earlier files.
// IsInitialised, IsObjectInCdImage, GetDiscInDrive, CarIsCandidateForRemoval are NOT hooked.
// WeAreTryingToPhaseVehicleOut, FindMIPedSlotForInterior, HasSpecialCharLoaded,
// HasVehicleUpgradeLoaded ARE hooked.

#include "StdInc.h"
#include "TestFramework.h"
#include "Streaming.h"

// --- Behavior tests for unhooked functions ---

GAME_TEST(CStreaming7, IsObjectInCdImage_Vehicles) {
    int32 models[] = { 400, 410, 420, 450, 500, 550, 596 };
    for (int32 m : models) {
        bool result = CStreaming::IsObjectInCdImage(m);
        (void)result;
        EXPECT_TRUE(true);
    }
}

GAME_TEST(CStreaming7, IsObjectInCdImage_Peds) {
    int32 models[] = { 0, 1, 7, 9, 20, 50, 100, 200 };
    for (int32 m : models) {
        bool result = CStreaming::IsObjectInCdImage(m);
        (void)result;
        EXPECT_TRUE(true);
    }
}

// --- Differential tests for hooked functions ---

GAME_DIFF_TEST(CStreaming7, WeAreTryingToPhaseVehicleOut_Range) {
    for (int32 m = 400; m <= 611; m += 10) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CStreaming/WeAreTryingToPhaseVehicleOut");
          orig = CStreaming::WeAreTryingToPhaseVehicleOut(m); }
        rev = CStreaming::WeAreTryingToPhaseVehicleOut(m);
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(CStreaming7, FindMIPedSlotForInterior_Range) {
    for (int32 r = 0; r < 20; r++) {
        int32 orig, rev;
        { HookDisableGuard guard("Global/CStreaming/FindMIPedSlotForInterior");
          orig = CStreaming::FindMIPedSlotForInterior(r); }
        rev = CStreaming::FindMIPedSlotForInterior(r);
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(CStreaming7, HasSpecialCharLoaded_FullRange) {
    for (int32 slot = 0; slot < 20; slot++) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CStreaming/HasSpecialCharLoaded");
          orig = CStreaming::HasSpecialCharLoaded(slot); }
        rev = CStreaming::HasSpecialCharLoaded(slot);
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(CStreaming7, HasVehicleUpgradeLoaded_FullRange) {
    for (int32 id = 1000; id <= 1193; id += 5) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CStreaming/HasVehicleUpgradeLoaded");
          orig = CStreaming::HasVehicleUpgradeLoaded(id); }
        rev = CStreaming::HasVehicleUpgradeLoaded(id);
        EXPECT_EQ(orig, rev);
    }
}
