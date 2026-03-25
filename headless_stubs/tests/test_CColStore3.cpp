// test_CColStore3.cpp — Differential tests for CColStore (part 3).
// Hook paths: Collision/CColStore/<fn>
//
// Tests: FindColSlot, AddCollisionNeededAtPosn, AddRef/RemoveRef,
//        EnsureCollisionIsInMemory, SetCollisionRequired, RequestCollision.

#include "StdInc.h"
#include "TestFramework.h"
#include "Collision/ColStore.h"

// --- FindColSlot by name ---

// FindColSlot is NOT hooked — behavior test only
GAME_TEST(CColStore, FindColSlot_Generic) {
    int32 slot = CColStore::FindColSlot("generic");
    EXPECT_EQ(slot, 0); // "generic" is always slot 0
}

GAME_TEST(CColStore, FindColSlot_NonExistent) {
    int32 slot = CColStore::FindColSlot("zzzz_does_not_exist_12345");
    EXPECT_EQ(slot, -1);
}

// --- AddCollisionNeededAtPosn ---

GAME_DIFF_TEST(CColStore, AddCollisionNeededAtPosn_Various) {
    CVector positions[] = {
        { 2450.f, -1665.f, 13.f },   // LS area
        { 0.f, 0.f, 0.f },           // Origin
        { -2170.f, 250.f, 30.f },    // SF area
    };
    for (auto& pos : positions) {
        // AddCollisionNeededAtPosn is void; just verify no crash in both paths
        { HookDisableGuard guard("Collision/CColStore/AddCollisionNeededAtPosn");
          CColStore::AddCollisionNeededAtPosn(pos); }
        CColStore::AddCollisionNeededAtPosn(pos);
    }
}

// --- AddRef / RemoveRef on slot 0 (generic) ---

GAME_DIFF_TEST(CColStore, AddRef_RemoveRef_Slot0) {
    // AddRef and RemoveRef are void — test they don't crash differentially
    { HookDisableGuard guard("Collision/CColStore/AddRef");
      CColStore::AddRef(0); }
    CColStore::AddRef(0);

    // Balance the refs
    { HookDisableGuard guard("Collision/CColStore/RemoveRef");
      CColStore::RemoveRef(0); }
    CColStore::RemoveRef(0);
}

// --- EnsureCollisionIsInMemory ---

GAME_DIFF_TEST(CColStore, EnsureCollisionIsInMemory_LS) {
    CVector pos{ 2450.f, -1665.f, 13.f };
    { HookDisableGuard guard("Collision/CColStore/EnsureCollisionIsInMemory");
      CColStore::EnsureCollisionIsInMemory(pos); }
    CColStore::EnsureCollisionIsInMemory(pos);
}

// --- SetCollisionRequired ---

GAME_DIFF_TEST(CColStore, SetCollisionRequired_Various) {
    struct TestCase { CVector pos; int32 area; };
    TestCase cases[] = {
        { { 2455.f, -1665.f, 13.f }, 0 },
        { { -2170.f, 250.f, 30.f }, 0 },
    };
    for (auto& tc : cases) {
        { HookDisableGuard guard("Collision/CColStore/SetCollisionRequired");
          CColStore::SetCollisionRequired(tc.pos, tc.area); }
        CColStore::SetCollisionRequired(tc.pos, tc.area);
    }
}

// --- RequestCollision ---

GAME_DIFF_TEST(CColStore, RequestCollision_LS) {
    CVector pos{ 2450.f, -1665.f, 13.f };
    { HookDisableGuard guard("Collision/CColStore/RequestCollision");
      CColStore::RequestCollision(pos, 0); }
    CColStore::RequestCollision(pos, 0);
}

// --- IncludeModelIndex (slot 0, model 0) ---

GAME_DIFF_TEST(CColStore, IncludeModelIndex_Slot0) {
    // Should be safe for slot 0 with a valid low model id
    { HookDisableGuard guard("Collision/CColStore/IncludeModelIndex");
      CColStore::IncludeModelIndex(0, 0); }
    CColStore::IncludeModelIndex(0, 0);
}
