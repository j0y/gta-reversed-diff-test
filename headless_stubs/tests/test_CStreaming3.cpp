// test_CStreaming3.cpp — Deep differential tests for CStreaming query functions.
// Hook paths: Global/CStreaming/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "Streaming.h"

// --- GetDefaultCopModel ---

GAME_DIFF_TEST(CStreaming3, GetDefaultCopModel) {
    int32 orig, rev;
    { HookDisableGuard guard("Global/CStreaming/GetDefaultCopModel"); orig = (int32)CStreaming::GetDefaultCopModel(); }
    rev = (int32)CStreaming::GetDefaultCopModel();
    EXPECT_EQ(orig, rev);
}

// --- GetDefaultCopCarModel ---

GAME_DIFF_TEST(CStreaming3, GetDefaultCopCarModel_IgnoreLvpd) {
    int32 orig, rev;
    { HookDisableGuard guard("Global/CStreaming/GetDefaultCopCarModel"); orig = (int32)CStreaming::GetDefaultCopCarModel(true); }
    rev = (int32)CStreaming::GetDefaultCopCarModel(true);
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CStreaming3, GetDefaultCopCarModel_IncludeLvpd) {
    int32 orig, rev;
    { HookDisableGuard guard("Global/CStreaming/GetDefaultCopCarModel"); orig = (int32)CStreaming::GetDefaultCopCarModel(false); }
    rev = (int32)CStreaming::GetDefaultCopCarModel(false);
    EXPECT_EQ(orig, rev);
}

// --- GetDefaultFiremanModel ---

GAME_DIFF_TEST(CStreaming3, GetDefaultFiremanModel) {
    int32 orig, rev;
    { HookDisableGuard guard("Global/CStreaming/GetDefaultFiremanModel"); orig = (int32)CStreaming::GetDefaultFiremanModel(); }
    rev = (int32)CStreaming::GetDefaultFiremanModel();
    EXPECT_EQ(orig, rev);
}

// --- GetDefaultMedicModel ---

GAME_DIFF_TEST(CStreaming3, GetDefaultMedicModel) {
    int32 orig, rev;
    { HookDisableGuard guard("Global/CStreaming/GetDefaultMedicModel"); orig = (int32)CStreaming::GetDefaultMedicModel(); }
    rev = (int32)CStreaming::GetDefaultMedicModel();
    EXPECT_EQ(orig, rev);
}

// --- HasSpecialCharLoaded ---

GAME_DIFF_TEST(CStreaming3, HasSpecialCharLoaded_Various) {
    int32 slots[] = { 0, 1, 2, 3, 4, 5, 6, 7 };
    for (int32 s : slots) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CStreaming/HasSpecialCharLoaded"); orig = CStreaming::HasSpecialCharLoaded(s); }
        rev = CStreaming::HasSpecialCharLoaded(s);
        EXPECT_EQ(orig, rev);
    }
}

// --- HasVehicleUpgradeLoaded ---

GAME_DIFF_TEST(CStreaming3, HasVehicleUpgradeLoaded_Various) {
    int32 models[] = { 1000, 1001, 1010, 1050, 1080, 1100, 1130, 1190 };
    for (int32 m : models) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CStreaming/HasVehicleUpgradeLoaded"); orig = CStreaming::HasVehicleUpgradeLoaded(m); }
        rev = CStreaming::HasVehicleUpgradeLoaded(m);
        EXPECT_EQ(orig, rev);
    }
}

// --- IsVeryBusy ---

GAME_DIFF_TEST(CStreaming3, IsVeryBusy) {
    bool orig, rev;
    { HookDisableGuard guard("Global/CStreaming/IsVeryBusy"); orig = CStreaming::IsVeryBusy(); }
    rev = CStreaming::IsVeryBusy();
    EXPECT_EQ(orig, rev);
}

// --- CarIsCandidateForRemoval ---

// CarIsCandidateForRemoval hook path not found at runtime — skip

// --- IsCarModelNeededInCurrentZone ---

GAME_DIFF_TEST(CStreaming3, IsCarModelNeededInCurrentZone_Various) {
    int32 models[] = { 400, 410, 420, 500, 560, 596 };
    for (int32 m : models) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CStreaming/IsCarModelNeededInCurrentZone"); orig = CStreaming::IsCarModelNeededInCurrentZone(m); }
        rev = CStreaming::IsCarModelNeededInCurrentZone(m);
        EXPECT_EQ(orig, rev);
    }
}
