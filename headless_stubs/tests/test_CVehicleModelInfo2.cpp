// test_CVehicleModelInfo2.cpp — Extended differential tests for CVehicleModelInfo.
// Hook paths: Models/CVehicleModelInfo/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "Models/VehicleModelInfo.h"
#include "ModelInfo.h"

// Helper: get a valid CVehicleModelInfo for a loaded vehicle
static CVehicleModelInfo* GetLoadedVehicleModelInfo() {
    // Model 596 (police car) is always loaded at state 9
    auto* info = CModelInfo::GetModelInfo(596);
    if (info && info->GetModelType() == MODEL_INFO_VEHICLE)
        return static_cast<CVehicleModelInfo*>(info);
    return nullptr;
}

GAME_DIFF_TEST(CVehicleModelInfo2, GetNumDoors) {
    auto* mi = GetLoadedVehicleModelInfo();
    if (!mi) return;
    int32 orig, rev;
    { HookDisableGuard guard("Models/CVehicleModelInfo/GetNumDoors");
      orig = mi->GetNumDoors(); }
    rev = mi->GetNumDoors();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CVehicleModelInfo2, GetNumRemaps) {
    auto* mi = GetLoadedVehicleModelInfo();
    if (!mi) return;
    int32 orig, rev;
    { HookDisableGuard guard("Models/CVehicleModelInfo/GetNumRemaps");
      orig = mi->GetNumRemaps(); }
    rev = mi->GetNumRemaps();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CVehicleModelInfo2, GetAnimFileIndex) {
    auto* mi = GetLoadedVehicleModelInfo();
    if (!mi) return;
    int32 orig, rev;
    { HookDisableGuard guard("Models/CVehicleModelInfo/GetAnimFileIndex");
      orig = mi->GetAnimFileIndex(); }
    rev = mi->GetAnimFileIndex();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CVehicleModelInfo2, GetCustomCarPlateText) {
    auto* mi = GetLoadedVehicleModelInfo();
    if (!mi) return;
    const char* orig;
    const char* rev;
    { HookDisableGuard guard("Models/CVehicleModelInfo/GetCustomCarPlateText");
      orig = mi->GetCustomCarPlateText(); }
    rev = mi->GetCustomCarPlateText();
    // Both null or both same string
    if (orig && rev)
        EXPECT_TRUE(strcmp(orig, rev) == 0);
    else
        EXPECT_EQ(orig, rev);
}

// Test across multiple loaded vehicle models
GAME_DIFF_TEST(CVehicleModelInfo2, GetNumDoors_MultipleModels) {
    int32 models[] = { 400, 401, 402, 410, 420, 440, 480, 500, 550, 596 };
    for (int32 id : models) {
        auto* info = CModelInfo::GetModelInfo(id);
        if (!info || info->GetModelType() != MODEL_INFO_VEHICLE) continue;
        auto* mi = static_cast<CVehicleModelInfo*>(info);
        int32 orig, rev;
        { HookDisableGuard guard("Models/CVehicleModelInfo/GetNumDoors");
          orig = mi->GetNumDoors(); }
        rev = mi->GetNumDoors();
        EXPECT_EQ(orig, rev);
    }
}

// IsValidCompRule not a member of CVehicleModelInfo — skipped.
