// test_CStreaming.cpp — Differential tests for CStreaming.
// Hook paths: Global/CStreaming/<fn>
// All functions are static — safe to call at state 9 with models loaded.

#include "StdInc.h"
#include "TestFramework.h"
#include "Streaming.h"

// --- Simple bool queries ---

GAME_DIFF_TEST(CStreaming, IsVeryBusy) {
    bool orig, rev;
    { HookDisableGuard guard("Global/CStreaming/IsVeryBusy"); orig = CStreaming::IsVeryBusy(); }
    rev = CStreaming::IsVeryBusy();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CStreaming, HasVehicleUpgradeLoaded) {
    // Test a range of model IDs in the vehicle upgrade range (1000-1193)
    int ids[] = { 1000, 1001, 1010, 1050, 1080, 1100, 1130, 1193, 999, 0, 400 };
    for (int id : ids) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CStreaming/HasVehicleUpgradeLoaded"); orig = CStreaming::HasVehicleUpgradeLoaded(id); }
        rev = CStreaming::HasVehicleUpgradeLoaded(id);
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(CStreaming, HasSpecialCharLoaded) {
    // Special char slots 0-9
    for (int slot = 0; slot < 10; slot++) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CStreaming/HasSpecialCharLoaded"); orig = CStreaming::HasSpecialCharLoaded(slot); }
        rev = CStreaming::HasSpecialCharLoaded(slot);
        EXPECT_EQ(orig, rev);
    }
}

// --- Default model getters ---

GAME_DIFF_TEST(CStreaming, GetDefaultCopModel) {
    auto orig = CallOriginal("Global/CStreaming/GetDefaultCopModel", CStreaming::GetDefaultCopModel);
    auto rev = CStreaming::GetDefaultCopModel();
    EXPECT_EQ((int)orig, (int)rev);
}

GAME_DIFF_TEST(CStreaming, GetDefaultCopCarModel) {
    // Test both with and without ignoreLvpd1
    for (bool ignore : { true, false }) {
        auto orig = CallOriginal("Global/CStreaming/GetDefaultCopCarModel",
            CStreaming::GetDefaultCopCarModel, ignore);
        auto rev = CStreaming::GetDefaultCopCarModel(ignore);
        EXPECT_EQ((int)orig, (int)rev);
    }
}

GAME_DIFF_TEST(CStreaming, GetDefaultFiremanModel) {
    auto orig = CallOriginal("Global/CStreaming/GetDefaultFiremanModel", CStreaming::GetDefaultFiremanModel);
    auto rev = CStreaming::GetDefaultFiremanModel();
    EXPECT_EQ((int)orig, (int)rev);
}

GAME_DIFF_TEST(CStreaming, GetDefaultMedicModel) {
    auto orig = CallOriginal("Global/CStreaming/GetDefaultMedicModel", CStreaming::GetDefaultMedicModel);
    auto rev = CStreaming::GetDefaultMedicModel();
    EXPECT_EQ((int)orig, (int)rev);
}

// GetDefaultCabDriverModel uses random selection — not deterministic between calls
// Skip differential test, just verify it returns a valid model ID
GAME_TEST(CStreaming, GetDefaultCabDriverModel_Valid) {
    int model = CStreaming::GetDefaultCabDriverModel();
    EXPECT_TRUE(model >= 0);
}

// --- Zone/model queries ---

GAME_DIFF_TEST(CStreaming, IsCarModelNeededInCurrentZone) {
    // Test common vehicle model IDs (400-611)
    int ids[] = { 400, 401, 410, 420, 440, 480, 500, 520, 560, 580, 596, 600, 611 };
    for (int id : ids) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CStreaming/IsCarModelNeededInCurrentZone"); orig = CStreaming::IsCarModelNeededInCurrentZone(id); }
        rev = CStreaming::IsCarModelNeededInCurrentZone(id);
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(CStreaming, WeAreTryingToPhaseVehicleOut) {
    int ids[] = { 400, 401, 410, 420, 440, 480, 500, 520, 560, 580, 596, 600, 611 };
    for (int id : ids) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CStreaming/WeAreTryingToPhaseVehicleOut"); orig = CStreaming::WeAreTryingToPhaseVehicleOut(id); }
        rev = CStreaming::WeAreTryingToPhaseVehicleOut(id);
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(CStreaming, FindMIPedSlotForInterior) {
    int factors[] = { 0, 1, 2, 5, 10, 50, 100 };
    for (int f : factors) {
        int orig, rev;
        { HookDisableGuard guard("Global/CStreaming/FindMIPedSlotForInterior"); orig = CStreaming::FindMIPedSlotForInterior(f); }
        rev = CStreaming::FindMIPedSlotForInterior(f);
        EXPECT_EQ(orig, rev);
    }
}
