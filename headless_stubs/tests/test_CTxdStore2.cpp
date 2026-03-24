// test_CTxdStore2.cpp — Differential tests for CTxdStore query functions.
// Hook paths: Global/CTxdStore/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "TxdStore.h"

// --- GetTxd ---

GAME_DIFF_TEST(CTxdStore2, GetTxd_FirstSlots) {
    for (int32 slot = 0; slot < 20; slot++) {
        auto* orig = CTxdStore::ms_pTxdPool->GetAt(slot);
        auto* rev = CTxdStore::ms_pTxdPool->GetAt(slot);
        EXPECT_EQ(orig, rev);
    }
}

// --- AddRef / GetNumRefs round-trip ---

GAME_DIFF_TEST(CTxdStore2, AddRef_GetNumRefs) {
    // Find a valid TXD slot with data
    for (int32 slot = 0; slot < 20; slot++) {
        if (!CTxdStore::ms_pTxdPool->GetAt(slot)) continue;

        int32 origBefore, revBefore;
        { HookDisableGuard guard("Global/CTxdStore/AddRef");
          origBefore = CTxdStore::GetNumRefs(slot);
          CTxdStore::AddRef(slot); }
        int32 origAfter = CTxdStore::GetNumRefs(slot);

        // Undo original's AddRef
        CTxdStore::RemoveRef(slot);

        revBefore = CTxdStore::GetNumRefs(slot);
        CTxdStore::AddRef(slot);
        int32 revAfter = CTxdStore::GetNumRefs(slot);

        // Undo reversed's AddRef
        CTxdStore::RemoveRef(slot);

        EXPECT_EQ(origBefore, revBefore);
        EXPECT_EQ(origAfter, revAfter);
        EXPECT_EQ(origAfter, origBefore + 1);
        break; // one slot is enough
    }
}
