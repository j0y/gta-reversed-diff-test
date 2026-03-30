// test_CLoadedCarGroup2.cpp — Differential tests for CLoadedCarGroup.
// Hook paths: Global/CLoadedCarGroup/<fn>
//
// Previously tested: Clear, AddMember, CountMembers, GetMember, RemoveMember
//
// New targets:
//   PickLeastUsedModel — finds model with lowest refcount/usage
//   SortBasedOnUsage   — sorts models by m_nTimesUsed descending
//
// Note: PickRandomCar uses RNG — not differentially testable.
//
// PickLeastUsedModel has a suspected bug: the comparator uses
// GetMI(modelA) for both miA and miB (copy-paste error).
// This means it always returns the first element regardless of usage.

#include "StdInc.h"
#include "TestFramework.h"
#include "LoadedCarGroup.h"
#include "Streaming.h"

// ── SortBasedOnUsage ──
// Sorts the model array by m_nTimesUsed descending.
// Test on the global CStreaming::ms_vehiclesLoaded which has real data.

GAME_DIFF_TEST(CLoadedCarGroup2, Diff_SortBasedOnUsage) {
    // Make a copy of the real loaded vehicle group
    auto& global = CStreaming::ms_vehiclesLoaded;
    CLoadedCarGroup origCopy, revCopy;
    memcpy(&origCopy, &global, sizeof(CLoadedCarGroup));
    memcpy(&revCopy, &global, sizeof(CLoadedCarGroup));

    if (origCopy.CountMembers() == 0) return;

    { HookDisableGuard guard("Global/CLoadedCarGroup/SortBasedOnUsage");
      origCopy.SortBasedOnUsage(); }
    revCopy.SortBasedOnUsage();

    // Compare all members after sort
    uint32 count = origCopy.CountMembers();
    EXPECT_EQ(count, revCopy.CountMembers());
    for (uint32 i = 0; i < count; i++) {
        EXPECT_EQ(origCopy.GetMember(i), revCopy.GetMember(i));
    }
}

// ── PickLeastUsedModel ──
// Returns the model with the lowest refcount (primary) and usage (secondary).
// Suspected bug: comparator uses GetMI(modelA) for both miA and miB.

GAME_DIFF_TEST(CLoadedCarGroup2, Diff_PickLeastUsedModel) {
    auto& global = CStreaming::ms_vehiclesLoaded;
    if (global.CountMembers() == 0) return;

    // Test with a high maxTimesUsed (should find something)
    eModelID orig, rev;
    { HookDisableGuard guard("Global/CLoadedCarGroup/PickLeastUsedModel");
      orig = global.PickLeastUsedModel(9999); }
    rev = global.PickLeastUsedModel(9999);
    EXPECT_EQ((int32)orig, (int32)rev);
}

GAME_DIFF_TEST(CLoadedCarGroup2, Diff_PickLeastUsedModel_ZeroMax) {
    auto& global = CStreaming::ms_vehiclesLoaded;
    if (global.CountMembers() == 0) return;

    // With maxTimesUsed=0, only models never used qualify
    eModelID orig, rev;
    { HookDisableGuard guard("Global/CLoadedCarGroup/PickLeastUsedModel");
      orig = global.PickLeastUsedModel(0); }
    rev = global.PickLeastUsedModel(0);
    EXPECT_EQ((int32)orig, (int32)rev);
}

GAME_DIFF_TEST(CLoadedCarGroup2, Diff_PickLeastUsedModel_AppropriateLoaded) {
    auto& group = CPopulation::m_AppropriateLoadedCars;
    if (group.CountMembers() == 0) return;

    eModelID orig, rev;
    { HookDisableGuard guard("Global/CLoadedCarGroup/PickLeastUsedModel");
      orig = group.PickLeastUsedModel(9999); }
    rev = group.PickLeastUsedModel(9999);
    EXPECT_EQ((int32)orig, (int32)rev);
}

// ── PickLeastUsedModel on empty group ──

GAME_DIFF_TEST(CLoadedCarGroup2, Diff_PickLeastUsedModel_Empty) {
    CLoadedCarGroup empty;
    empty.Clear();

    eModelID orig, rev;
    { HookDisableGuard guard("Global/CLoadedCarGroup/PickLeastUsedModel");
      orig = empty.PickLeastUsedModel(9999); }
    rev = empty.PickLeastUsedModel(9999);
    EXPECT_EQ((int32)orig, (int32)rev);
    EXPECT_EQ((int32)rev, (int32)MODEL_INVALID);
}

// ── PickLeastUsedModel with crafted multi-model group ──
// Vehicle model info entries (400-611) exist in CModelInfo::ms_modelInfoPtrs
// even when not streamed. We build a group with known IDs and manipulate
// their m_nRefCount/m_nTimesUsed to force the comparator to exercise all branches.
//
// Bug #58: GetMI(modelA) was used for both miA and miB (copy-paste error).
// The comparator compared a model against itself, always returning false,
// so rng::min returned the first element regardless of actual usage.

GAME_DIFF_TEST(CLoadedCarGroup2, Diff_PickLeastUsedModel_TimesUsedDiffers) {
    // Use 3 known vehicle model IDs (Landstalker=400, Bravura=401, Buffalo=402)
    eModelID models[] = { (eModelID)400, (eModelID)401, (eModelID)402 };
    auto* mi0 = CModelInfo::GetVehicleModelInfo(400);
    auto* mi1 = CModelInfo::GetVehicleModelInfo(401);
    auto* mi2 = CModelInfo::GetVehicleModelInfo(402);
    if (!mi0 || !mi1 || !mi2) return;

    // Save original stats
    uint16 savedRef0 = mi0->m_nRefCount, savedRef1 = mi1->m_nRefCount, savedRef2 = mi2->m_nRefCount;
    uint8  savedUse0 = mi0->m_nTimesUsed, savedUse1 = mi1->m_nTimesUsed, savedUse2 = mi2->m_nTimesUsed;

    // Set same refcount, but model 402 has lowest timesUsed
    mi0->m_nRefCount = 1; mi0->m_nTimesUsed = 50;
    mi1->m_nRefCount = 1; mi1->m_nTimesUsed = 30;
    mi2->m_nRefCount = 1; mi2->m_nTimesUsed = 10;  // winner: lowest usage

    CLoadedCarGroup group;
    group.Clear();
    for (auto m : models) group.AddMember(m);

    eModelID orig, rev;
    { HookDisableGuard guard("Global/CLoadedCarGroup/PickLeastUsedModel");
      orig = group.PickLeastUsedModel(9999); }
    rev = group.PickLeastUsedModel(9999);

    // Restore
    mi0->m_nRefCount = savedRef0; mi0->m_nTimesUsed = savedUse0;
    mi1->m_nRefCount = savedRef1; mi1->m_nTimesUsed = savedUse1;
    mi2->m_nRefCount = savedRef2; mi2->m_nTimesUsed = savedUse2;

    // Both should pick model 402 (lowest timesUsed).
    // Without the fix, reversed would pick 400 (first element).
    EXPECT_EQ((int32)orig, (int32)rev);
}

GAME_DIFF_TEST(CLoadedCarGroup2, Diff_PickLeastUsedModel_RefCountDiffers) {
    eModelID models[] = { (eModelID)400, (eModelID)401, (eModelID)402 };
    auto* mi0 = CModelInfo::GetVehicleModelInfo(400);
    auto* mi1 = CModelInfo::GetVehicleModelInfo(401);
    auto* mi2 = CModelInfo::GetVehicleModelInfo(402);
    if (!mi0 || !mi1 || !mi2) return;

    uint16 savedRef0 = mi0->m_nRefCount, savedRef1 = mi1->m_nRefCount, savedRef2 = mi2->m_nRefCount;
    uint8  savedUse0 = mi0->m_nTimesUsed, savedUse1 = mi1->m_nTimesUsed, savedUse2 = mi2->m_nTimesUsed;

    // Model 401 has lowest refcount — should win regardless of timesUsed
    mi0->m_nRefCount = 5; mi0->m_nTimesUsed = 0;
    mi1->m_nRefCount = 1; mi1->m_nTimesUsed = 99;  // winner: lowest refcount
    mi2->m_nRefCount = 3; mi2->m_nTimesUsed = 0;

    CLoadedCarGroup group;
    group.Clear();
    for (auto m : models) group.AddMember(m);

    eModelID orig, rev;
    { HookDisableGuard guard("Global/CLoadedCarGroup/PickLeastUsedModel");
      orig = group.PickLeastUsedModel(9999); }
    rev = group.PickLeastUsedModel(9999);

    mi0->m_nRefCount = savedRef0; mi0->m_nTimesUsed = savedUse0;
    mi1->m_nRefCount = savedRef1; mi1->m_nTimesUsed = savedUse1;
    mi2->m_nRefCount = savedRef2; mi2->m_nTimesUsed = savedUse2;

    // Both should pick model 401 (lowest refcount).
    // Without the fix, reversed would pick 400 (first element).
    EXPECT_EQ((int32)orig, (int32)rev);
}

GAME_DIFF_TEST(CLoadedCarGroup2, Diff_PickLeastUsedModel_LastElementWins) {
    eModelID models[] = { (eModelID)400, (eModelID)401, (eModelID)402 };
    auto* mi0 = CModelInfo::GetVehicleModelInfo(400);
    auto* mi1 = CModelInfo::GetVehicleModelInfo(401);
    auto* mi2 = CModelInfo::GetVehicleModelInfo(402);
    if (!mi0 || !mi1 || !mi2) return;

    uint16 savedRef0 = mi0->m_nRefCount, savedRef1 = mi1->m_nRefCount, savedRef2 = mi2->m_nRefCount;
    uint8  savedUse0 = mi0->m_nTimesUsed, savedUse1 = mi1->m_nTimesUsed, savedUse2 = mi2->m_nTimesUsed;

    // First two have high stats, last one is best — maximally exercises the bug
    mi0->m_nRefCount = 10; mi0->m_nTimesUsed = 100;
    mi1->m_nRefCount = 10; mi1->m_nTimesUsed = 80;
    mi2->m_nRefCount = 0;  mi2->m_nTimesUsed = 0;   // winner

    CLoadedCarGroup group;
    group.Clear();
    for (auto m : models) group.AddMember(m);

    eModelID orig, rev;
    { HookDisableGuard guard("Global/CLoadedCarGroup/PickLeastUsedModel");
      orig = group.PickLeastUsedModel(9999); }
    rev = group.PickLeastUsedModel(9999);

    mi0->m_nRefCount = savedRef0; mi0->m_nTimesUsed = savedUse0;
    mi1->m_nRefCount = savedRef1; mi1->m_nTimesUsed = savedUse1;
    mi2->m_nRefCount = savedRef2; mi2->m_nTimesUsed = savedUse2;

    // Both should pick model 402 (lowest everything).
    EXPECT_EQ((int32)orig, (int32)rev);
}
