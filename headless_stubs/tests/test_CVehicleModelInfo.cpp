// test_CVehicleModelInfo.cpp — Differential tests for CVehicleModelInfo.
// Hook paths: Models/CVehicleModelInfo/<fn>
// These operate on model info pointers retrieved via CModelInfo at state 9.

#include "StdInc.h"
#include "TestFramework.h"
#include "ModelInfo.h"

// Helper: get a valid CVehicleModelInfo* for a model ID, or nullptr
static CVehicleModelInfo* GetVehInfo(int32 id) {
    auto* base = CModelInfo::GetModelInfo(id);
    if (!base) return nullptr;
    return base->AsVehicleModelInfoPtr();
}

// GetMaximumNumberOfPassengersFromNumberOfDoors crashes in original code —
// internally calls GetModelInfo which may return null for some IDs,
// and original code dereferences without null check. Needs ScenarioHelpers
// to ensure models are fully loaded before testing.

GAME_DIFF_TEST(CVehicleModelInfo, GetNumRemaps) {
    int ids[] = { 400, 410, 420, 440, 480, 500, 560, 596, 600 };
    for (int id : ids) {
        auto* info = GetVehInfo(id);
        if (!info) continue;
        int orig, rev;
        { HookDisableGuard guard("Models/CVehicleModelInfo/GetNumRemaps"); orig = info->GetNumRemaps(); }
        rev = info->GetNumRemaps();
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(CVehicleModelInfo, GetNumDoors) {
    int ids[] = { 400, 410, 420, 440, 445, 480, 500, 560, 596, 600 };
    for (int id : ids) {
        auto* info = GetVehInfo(id);
        if (!info) continue;
        int orig, rev;
        { HookDisableGuard guard("Models/CVehicleModelInfo/GetNumDoors"); orig = info->GetNumDoors(); }
        rev = info->GetNumDoors();
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(CVehicleModelInfo, GetCustomCarPlateText) {
    int ids[] = { 400, 420, 480, 500, 596 };
    for (int id : ids) {
        auto* info = GetVehInfo(id);
        if (!info) continue;
        char* orig;
        char* rev;
        { HookDisableGuard guard("Models/CVehicleModelInfo/GetCustomCarPlateText"); orig = info->GetCustomCarPlateText(); }
        rev = info->GetCustomCarPlateText();
        // Both should be same pointer or both null
        EXPECT_EQ(orig, rev);
    }
}

// IsUpgradeAvailable crashes in original code for some vehicles —
// likely accesses upgrade data not loaded in headless mode.
// Needs model streaming to be complete before testing.

GAME_DIFF_TEST(CVehicleModelInfo, GetModelType) {
    int ids[] = { 400, 410, 420, 440, 480, 500, 560, 596, 600, 611 };
    for (int id : ids) {
        auto* info = GetVehInfo(id);
        if (!info) continue;
        int orig, rev;
        { HookDisableGuard guard("Models/CVehicleModelInfo/GetModelType"); orig = (int)info->GetModelType(); }
        rev = (int)info->GetModelType();
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(CVehicleModelInfo, GetAnimFileIndex) {
    int ids[] = { 400, 410, 420, 440, 480, 500, 560, 596, 600 };
    for (int id : ids) {
        auto* info = GetVehInfo(id);
        if (!info) continue;
        int orig, rev;
        { HookDisableGuard guard("Models/CVehicleModelInfo/GetAnimFileIndex"); orig = info->GetAnimFileIndex(); }
        rev = info->GetAnimFileIndex();
        EXPECT_EQ(orig, rev);
    }
}

// Static wheel upgrade queries
GAME_DIFF_TEST(CVehicleModelInfo, GetNumWheelUpgrades) {
    for (int set = 0; set < 4; set++) {
        int orig, rev;
        { HookDisableGuard guard("Models/CVehicleModelInfo/GetNumWheelUpgrades");
          orig = CVehicleModelInfo::GetNumWheelUpgrades(set); }
        rev = CVehicleModelInfo::GetNumWheelUpgrades(set);
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(CVehicleModelInfo, GetWheelUpgrade) {
    // Use reversed code to get count, then compare individual lookups
    for (int set = 0; set < 4; set++) {
        int numUpgrades = CVehicleModelInfo::GetNumWheelUpgrades(set);
        for (int i = 0; i < numUpgrades && i < 3; i++) {
            int orig, rev;
            { HookDisableGuard guard("Models/CVehicleModelInfo/GetWheelUpgrade");
              orig = CVehicleModelInfo::GetWheelUpgrade(set, i); }
            rev = CVehicleModelInfo::GetWheelUpgrade(set, i);
            if (orig != rev) {
                char buf[128];
                snprintf(buf, sizeof(buf), "set=%d i=%d: orig=%d rev=%d", set, i, orig, rev);
                GetTestContext().RecordFailure("test_CVehicleModelInfo.cpp", __LINE__, buf);
                return;
            }
            EXPECT_EQ(orig, rev);
        }
    }
}
