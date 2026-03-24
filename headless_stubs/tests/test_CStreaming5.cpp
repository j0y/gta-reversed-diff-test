// test_CStreaming5.cpp — Extended differential tests for CStreaming.
// Hook paths: Global/CStreaming/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "Streaming.h"

// --- IsVeryBusy ---

GAME_DIFF_TEST(CStreaming5, IsVeryBusy) {
    bool orig, rev;
    { HookDisableGuard guard("Global/CStreaming/IsVeryBusy");
      orig = CStreaming::IsVeryBusy(); }
    rev = CStreaming::IsVeryBusy();
    EXPECT_EQ(orig, rev);
}

// --- AreAnimsUsedByRequestedModels ---

GAME_DIFF_TEST(CStreaming5, AreAnimsUsedByRequestedModels) {
    // Test with a few anim block IDs
    for (int32 blockId = 0; blockId < 5; blockId++) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CStreaming/AreAnimsUsedByRequestedModels");
          orig = CStreaming::AreAnimsUsedByRequestedModels(blockId); }
        rev = CStreaming::AreAnimsUsedByRequestedModels(blockId);
        EXPECT_EQ(orig, rev);
    }
}

// --- AreTexturesUsedByRequestedModels ---

GAME_DIFF_TEST(CStreaming5, AreTexturesUsedByRequestedModels) {
    for (int32 txdId = 0; txdId < 5; txdId++) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CStreaming/AreTexturesUsedByRequestedModels");
          orig = CStreaming::AreTexturesUsedByRequestedModels(txdId); }
        rev = CStreaming::AreTexturesUsedByRequestedModels(txdId);
        EXPECT_EQ(orig, rev);
    }
}

// --- HasSpecialCharLoaded ---

GAME_DIFF_TEST(CStreaming5, HasSpecialCharLoaded_Range) {
    for (int32 slot = 0; slot < 10; slot++) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CStreaming/HasSpecialCharLoaded");
          orig = CStreaming::HasSpecialCharLoaded(slot); }
        rev = CStreaming::HasSpecialCharLoaded(slot);
        EXPECT_EQ(orig, rev);
    }
}

// --- HasVehicleUpgradeLoaded ---

GAME_DIFF_TEST(CStreaming5, HasVehicleUpgradeLoaded_Range) {
    // Vehicle upgrades are in model ID range ~1000-1193
    int32 upgrades[] = { 1000, 1001, 1010, 1050, 1100, 1190 };
    for (int32 id : upgrades) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CStreaming/HasVehicleUpgradeLoaded");
          orig = CStreaming::HasVehicleUpgradeLoaded(id); }
        rev = CStreaming::HasVehicleUpgradeLoaded(id);
        EXPECT_EQ(orig, rev);
    }
}

// --- IsCarModelNeededInCurrentZone ---

GAME_DIFF_TEST(CStreaming5, IsCarModelNeededInCurrentZone_Range) {
    int32 models[] = { 400, 420, 450, 500, 550, 596, 600 };
    for (int32 m : models) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CStreaming/IsCarModelNeededInCurrentZone");
          orig = CStreaming::IsCarModelNeededInCurrentZone(m); }
        rev = CStreaming::IsCarModelNeededInCurrentZone(m);
        EXPECT_EQ(orig, rev);
    }
}

// --- GetDefaultCopModel / GetDefaultCopCarModel ---

GAME_DIFF_TEST(CStreaming5, GetDefaultCopModel) {
    int32 orig, rev;
    { HookDisableGuard guard("Global/CStreaming/GetDefaultCopModel");
      orig = CStreaming::GetDefaultCopModel(); }
    rev = CStreaming::GetDefaultCopModel();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CStreaming5, GetDefaultCopCarModel) {
    int32 orig, rev;
    { HookDisableGuard guard("Global/CStreaming/GetDefaultCopCarModel");
      orig = CStreaming::GetDefaultCopCarModel(); }
    rev = CStreaming::GetDefaultCopCarModel();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CStreaming5, GetDefaultFiremanModel) {
    int32 orig, rev;
    { HookDisableGuard guard("Global/CStreaming/GetDefaultFiremanModel");
      orig = CStreaming::GetDefaultFiremanModel(); }
    rev = CStreaming::GetDefaultFiremanModel();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CStreaming5, GetDefaultMedicModel) {
    int32 orig, rev;
    { HookDisableGuard guard("Global/CStreaming/GetDefaultMedicModel");
      orig = CStreaming::GetDefaultMedicModel(); }
    rev = CStreaming::GetDefaultMedicModel();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CStreaming5, GetDefaultCabDriverModel) {
    int32 orig, rev;
    { HookDisableGuard guard("Global/CStreaming/GetDefaultCabDriverModel");
      orig = CStreaming::GetDefaultCabDriverModel(); }
    rev = CStreaming::GetDefaultCabDriverModel();
    EXPECT_EQ(orig, rev);
}
