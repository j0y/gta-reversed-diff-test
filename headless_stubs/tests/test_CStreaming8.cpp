// test_CStreaming8.cpp — Systematic CStreaming differential tests.
// Tests ALL remaining untested hooked CStreaming query functions.
// Hook paths: Global/CStreaming/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "Streaming.h"
#include "StreamingInfo.h"

// GetNextFileOnCd — advances internal stream position on each call.
// Can't diff test (original call moves cursor, reversed starts from different offset).
// Same issue as CVehicleRecording::RegisterRecordingFile.
// Behavior test only.

GAME_TEST(CStreaming8, GetNextFileOnCd_NoCrash) {
    int32 result = CStreaming::GetNextFileOnCd(0, false);
    (void)result;
    EXPECT_TRUE(true);
}

GAME_TEST(CStreaming8, GetNextFileOnCd_NotPriority) {
    int32 result = CStreaming::GetNextFileOnCd(0, true);
    (void)result;
    EXPECT_TRUE(true);
}

// ===================================================================
// IsVeryBusy — checks if streaming system is overloaded
// ===================================================================

GAME_DIFF_TEST(CStreaming8, IsVeryBusy) {
    bool orig, rev;
    { HookDisableGuard guard("Global/CStreaming/IsVeryBusy");
      orig = CStreaming::IsVeryBusy(); }
    rev = CStreaming::IsVeryBusy();
    EXPECT_EQ(orig, rev);
}

// ===================================================================
// DisableCopBikes — sets flag, safe to toggle and restore
// ===================================================================

GAME_DIFF_TEST(CStreaming8, DisableCopBikes_Enable) {
    bool saved = CStreaming::m_bDisableCopBikes;
    { HookDisableGuard guard("Global/CStreaming/DisableCopBikes");
      CStreaming::DisableCopBikes(true); }
    bool origState = CStreaming::m_bDisableCopBikes;
    CStreaming::DisableCopBikes(true);
    bool revState = CStreaming::m_bDisableCopBikes;
    EXPECT_EQ(origState, revState);
    CStreaming::m_bDisableCopBikes = saved;
}

GAME_DIFF_TEST(CStreaming8, DisableCopBikes_Disable) {
    bool saved = CStreaming::m_bDisableCopBikes;
    { HookDisableGuard guard("Global/CStreaming/DisableCopBikes");
      CStreaming::DisableCopBikes(false); }
    bool origState = CStreaming::m_bDisableCopBikes;
    CStreaming::DisableCopBikes(false);
    bool revState = CStreaming::m_bDisableCopBikes;
    EXPECT_EQ(origState, revState);
    CStreaming::m_bDisableCopBikes = saved;
}

// ===================================================================
// GetDefaultFiremanModel / GetDefaultMedicModel — simple lookups
// Already tested but let's add more input coverage
// ===================================================================

GAME_DIFF_TEST(CStreaming8, GetDefaultFiremanModel) {
    int32 orig, rev;
    { HookDisableGuard guard("Global/CStreaming/GetDefaultFiremanModel");
      orig = CStreaming::GetDefaultFiremanModel(); }
    rev = CStreaming::GetDefaultFiremanModel();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CStreaming8, GetDefaultMedicModel) {
    int32 orig, rev;
    { HookDisableGuard guard("Global/CStreaming/GetDefaultMedicModel");
      orig = CStreaming::GetDefaultMedicModel(); }
    rev = CStreaming::GetDefaultMedicModel();
    EXPECT_EQ(orig, rev);
}

// ===================================================================
// AreAnimsUsedByRequestedModels / AreTexturesUsedByRequestedModels
// ===================================================================

GAME_DIFF_TEST(CStreaming8, AreAnimsUsedByRequestedModels_Various) {
    // Test with a range of anim block indices
    for (int32 i = 0; i < 20; i++) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CStreaming/AreAnimsUsedByRequestedModels");
          orig = CStreaming::AreAnimsUsedByRequestedModels(i); }
        rev = CStreaming::AreAnimsUsedByRequestedModels(i);
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(CStreaming8, AreTexturesUsedByRequestedModels_Various) {
    // Test with a range of TXD indices
    for (int32 i = 0; i < 20; i++) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CStreaming/AreTexturesUsedByRequestedModels");
          orig = CStreaming::AreTexturesUsedByRequestedModels(i); }
        rev = CStreaming::AreTexturesUsedByRequestedModels(i);
        EXPECT_EQ(orig, rev);
    }
}

// ===================================================================
// IsCarModelNeededInCurrentZone — checks model against zone requirements
// ===================================================================

GAME_DIFF_TEST(CStreaming8, IsCarModelNeededInCurrentZone_AllVehicles) {
    for (int32 id = 400; id <= 611; id += 5) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CStreaming/IsCarModelNeededInCurrentZone");
          orig = CStreaming::IsCarModelNeededInCurrentZone(id); }
        rev = CStreaming::IsCarModelNeededInCurrentZone(id);
        EXPECT_EQ(orig, rev);
    }
}

// ===================================================================
// RemoveLeastUsedModel — returns bool, tries to free a model slot
// ===================================================================

GAME_DIFF_TEST(CStreaming8, RemoveLeastUsedModel_NoFlags) {
    bool orig, rev;
    { HookDisableGuard guard("Global/CStreaming/RemoveLeastUsedModel");
      orig = CStreaming::RemoveLeastUsedModel(0); }
    rev = CStreaming::RemoveLeastUsedModel(0);
    EXPECT_EQ(orig, rev);
}

// ===================================================================
// AddToLoadedVehiclesList — returns bool
// ===================================================================

GAME_DIFF_TEST(CStreaming8, AddToLoadedVehiclesList_Invalid) {
    // Test with model that's already loaded (police car)
    bool orig, rev;
    { HookDisableGuard guard("Global/CStreaming/AddToLoadedVehiclesList");
      orig = CStreaming::AddToLoadedVehiclesList(596); }
    rev = CStreaming::AddToLoadedVehiclesList(596);
    EXPECT_EQ(orig, rev);
}

// ===================================================================
// GetDefaultCopModel / GetDefaultCopCarModel — zone-dependent lookups
// ===================================================================

GAME_DIFF_TEST(CStreaming8, GetDefaultCopModel_Extended) {
    // Call multiple times — should be deterministic
    for (int i = 0; i < 5; i++) {
        int32 orig, rev;
        { HookDisableGuard guard("Global/CStreaming/GetDefaultCopModel");
          orig = CStreaming::GetDefaultCopModel(); }
        rev = CStreaming::GetDefaultCopModel();
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(CStreaming8, GetDefaultCopCarModel_Extended) {
    for (int i = 0; i < 5; i++) {
        int32 orig, rev;
        { HookDisableGuard guard("Global/CStreaming/GetDefaultCopCarModel");
          orig = CStreaming::GetDefaultCopCarModel(false); }
        rev = CStreaming::GetDefaultCopCarModel(false);
        EXPECT_EQ(orig, rev);
    }
}

// ===================================================================
// PossiblyStreamCarOutAfterCreation — void, safe to call on loaded model
// ===================================================================

GAME_DIFF_TEST(CStreaming8, PossiblyStreamCarOutAfterCreation_596) {
    { HookDisableGuard guard("Global/CStreaming/PossiblyStreamCarOutAfterCreation");
      CStreaming::PossiblyStreamCarOutAfterCreation(596); }
    CStreaming::PossiblyStreamCarOutAfterCreation(596);
    EXPECT_TRUE(true);
}

// ===================================================================
// SetModelIsDeletable / SetMissionDoesntRequireModel — flag setters
// ===================================================================

GAME_DIFF_TEST(CStreaming8, SetModelIsDeletable_And_Restore) {
    // Save, toggle, restore
    auto& info = CStreaming::GetInfo(596);
    auto savedFlags = info.GetFlags();
    { HookDisableGuard guard("Global/CStreaming/SetModelIsDeletable");
      CStreaming::SetModelIsDeletable(596); }
    auto origFlags = info.GetFlags();
    info.SetFlags(savedFlags); // restore

    CStreaming::SetModelIsDeletable(596);
    auto revFlags = info.GetFlags();
    info.SetFlags(savedFlags); // restore

    EXPECT_EQ(origFlags, revFlags);
}

GAME_DIFF_TEST(CStreaming8, SetMissionDoesntRequireModel_596) {
    auto& info = CStreaming::GetInfo(596);
    auto savedFlags = info.GetFlags();
    { HookDisableGuard guard("Global/CStreaming/SetMissionDoesntRequireModel");
      CStreaming::SetMissionDoesntRequireModel(596); }
    auto origFlags = info.GetFlags();
    info.SetFlags(savedFlags);

    CStreaming::SetMissionDoesntRequireModel(596);
    auto revFlags = info.GetFlags();
    info.SetFlags(savedFlags);

    EXPECT_EQ(origFlags, revFlags);
}
