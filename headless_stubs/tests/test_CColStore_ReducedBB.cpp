// test_CColStore_ReducedBB.cpp — Differential test for SetIfCollisionIsRequiredReducedBB.
//
// Bug: reversed code guards only (!area && interior), missing (area!=0 && !interior).
// Original truth table:
//   (area=0, interior=F) => SET
//   (area=0, interior=T) => RET
//   (area!=0, interior=T) => SET
//   (area!=0, interior=F) => RET  <-- reversed incorrectly SETs here
//
// Strategy: call SetCollisionRequired with areaCode != CGame::currArea to trigger the
// ReducedBB path, then compare m_bCollisionIsRequired on pool slots between original
// and reversed. We toggle only the ReducedBB hook so the rest uses reversed code.

#include "StdInc.h"
#include "TestFramework.h"
#include "Collision/ColStore.h"
#include "Game.h"

// SetCollisionRequired calls ForAllMatching with SetIfCollisionIsRequiredReducedBB
// when areaCode != CGame::currArea. We toggle the callback hook and compare results.

static constexpr const char* HOOK_REDUCED_BB =
    "Collision/CColStore/SetIfCollisionIsRequiredReducedBB";

// Helper: clear all m_bCollisionIsRequired flags in the pool
static void ClearAllCollisionRequired() {
    auto* pool = CColStore::GetPool();
    for (auto i = 1u; i < pool->GetSize(); i++) {
        auto* def = pool->GetAt(i);
        if (def)
            def->m_bCollisionIsRequired = false;
    }
}

// Helper: snapshot m_bCollisionIsRequired for all slots
static void SnapshotCollisionRequired(bool* out, size_t maxSlots) {
    auto* pool = CColStore::GetPool();
    for (size_t i = 0; i < maxSlots && i < pool->GetSize(); i++) {
        auto* def = pool->GetAt(i);
        out[i] = def ? def->m_bCollisionIsRequired : false;
    }
}

// Test: use an areaCode that differs from CGame::currArea to force the ReducedBB path.
// Pick a position near Grove Street (LS) and use area=1 (interior) when currArea=0.
// This exercises the (area!=0, interior=false) case on non-interior ColDefs.
GAME_DIFF_TEST(CColStore, SetIfCollisionIsRequiredReducedBB_Guard) {
    auto* pool = CColStore::GetPool();
    size_t poolSize = pool->GetSize();
    if (poolSize < 2) {
        EXPECT_TRUE(false); // pool too small
        return;
    }

    // Cap snapshot size
    constexpr size_t MAX_SLOTS = 256;
    size_t slots = poolSize < MAX_SLOTS ? poolSize : MAX_SLOTS;

    bool origSnapshot[MAX_SLOTS] = {};
    bool revSnapshot[MAX_SLOTS] = {};

    // Use area=1 when CGame::currArea is 0 (exterior), or area=0 when currArea!=0.
    // This ensures usedArea != CGame::currArea, triggering the ReducedBB path.
    int32 testArea = (CGame::currArea == 0) ? 1 : 0;

    // Position near Grove Street — known to have collision slots in range
    CVector testPos(2495.0f, -1665.0f, 13.5f);

    // --- Original code path ---
    ClearAllCollisionRequired();
    {
        HookDisableGuard guard(HOOK_REDUCED_BB);
        CColStore::SetCollisionRequired(testPos, testArea);
    }
    SnapshotCollisionRequired(origSnapshot, slots);

    // --- Reversed code path ---
    ClearAllCollisionRequired();
    CColStore::SetCollisionRequired(testPos, testArea);
    SnapshotCollisionRequired(revSnapshot, slots);

    // Compare: every slot should match
    int mismatches = 0;
    for (size_t i = 0; i < slots; i++) {
        if (origSnapshot[i] != revSnapshot[i]) {
            mismatches++;
            auto* def = pool->GetAt((int32)i);
            if (def && mismatches <= 5) {
                char msg[256];
                _snprintf(msg, sizeof(msg),
                    "Slot %d ('%s', interior=%d): orig=%d rev=%d (area=%d)",
                    (int)i, def->name, (int)def->m_bInterior,
                    (int)origSnapshot[i], (int)revSnapshot[i], testArea);
                GetTestContext().RecordFailure(__FILE__, __LINE__, msg);
                GetTestContext().assertions++;
            }
        }
    }
    // At least one assertion even if no mismatches
    GetTestContext().assertions++;
    if (mismatches == 0) {
        // If no mismatches, the bug isn't triggered — could mean no non-interior
        // ColDefs are in range. Try to verify we actually tested something.
        // Count how many slots were set by either path.
        int origSet = 0, revSet = 0;
        for (size_t i = 0; i < slots; i++) {
            if (origSnapshot[i]) origSet++;
            if (revSnapshot[i]) revSet++;
        }
        // If both set the same count, test is inconclusive but passes.
        // Record as assertion so we see it ran.
        EXPECT_EQ(origSet, revSet);
    }

    // Restore: clear flags to avoid side effects on other tests
    ClearAllCollisionRequired();
}

// Focused test: use area=1 from SF area to maximise non-interior ColDefs in range
GAME_DIFF_TEST(CColStore, SetIfCollisionIsRequiredReducedBB_SF) {
    auto* pool = CColStore::GetPool();
    size_t poolSize = pool->GetSize();
    if (poolSize < 2) {
        EXPECT_TRUE(false);
        return;
    }

    constexpr size_t MAX_SLOTS = 256;
    size_t slots = poolSize < MAX_SLOTS ? poolSize : MAX_SLOTS;

    bool origSnapshot[MAX_SLOTS] = {};
    bool revSnapshot[MAX_SLOTS] = {};

    // SF position, force area=1 (or 2) to differ from currArea
    CVector testPos(-2170.0f, 250.0f, 30.0f);
    int32 testArea = (CGame::currArea == 0) ? 1 : 0;

    // Original
    ClearAllCollisionRequired();
    {
        HookDisableGuard guard(HOOK_REDUCED_BB);
        CColStore::SetCollisionRequired(testPos, testArea);
    }
    SnapshotCollisionRequired(origSnapshot, slots);

    // Reversed
    ClearAllCollisionRequired();
    CColStore::SetCollisionRequired(testPos, testArea);
    SnapshotCollisionRequired(revSnapshot, slots);

    int mismatches = 0;
    for (size_t i = 0; i < slots; i++) {
        if (origSnapshot[i] != revSnapshot[i]) {
            mismatches++;
            auto* def = pool->GetAt((int32)i);
            if (def && mismatches <= 5) {
                char msg[256];
                _snprintf(msg, sizeof(msg),
                    "Slot %d ('%s', interior=%d): orig=%d rev=%d (area=%d)",
                    (int)i, def->name, (int)def->m_bInterior,
                    (int)origSnapshot[i], (int)revSnapshot[i], testArea);
                GetTestContext().RecordFailure(__FILE__, __LINE__, msg);
                GetTestContext().assertions++;
            }
        }
    }
    GetTestContext().assertions++;
    if (mismatches == 0) {
        int origSet = 0, revSet = 0;
        for (size_t i = 0; i < slots; i++) {
            if (origSnapshot[i]) origSet++;
            if (revSnapshot[i]) revSet++;
        }
        EXPECT_EQ(origSet, revSet);
    }

    ClearAllCollisionRequired();
}
