// test_CTxdStore.cpp — Deep differential tests for CTxdStore.
// Hook paths: Global/CTxdStore/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "TxdStore.h"

// --- FindTxdSlot by name ---

// FindTxdSlot hook not found at runtime

// --- GetNumRefs ---

GAME_DIFF_TEST(CTxdStore, GetNumRefs_Various) {
    // Test with indices for known txd slots
    int32 genericSlot = CTxdStore::FindTxdSlot("generic");
    if (genericSlot >= 0) {
        int32 orig, rev;
        { HookDisableGuard guard("Global/CTxdStore/GetNumRefs"); orig = CTxdStore::GetNumRefs(genericSlot); }
        rev = CTxdStore::GetNumRefs(genericSlot);
        EXPECT_EQ(orig, rev);
    }

    int32 particleSlot = CTxdStore::FindTxdSlot("particle");
    if (particleSlot >= 0) {
        int32 orig, rev;
        { HookDisableGuard guard("Global/CTxdStore/GetNumRefs"); orig = CTxdStore::GetNumRefs(particleSlot); }
        rev = CTxdStore::GetNumRefs(particleSlot);
        EXPECT_EQ(orig, rev);
    }
}

// --- GetParentTxdSlot ---

GAME_DIFF_TEST(CTxdStore, GetParentTxdSlot_Various) {
    int32 slots[] = { 0, 1, 5, 10 };
    for (int32 s : slots) {
        int32 orig, rev;
        { HookDisableGuard guard("Global/CTxdStore/GetParentTxdSlot"); orig = CTxdStore::GetParentTxdSlot(s); }
        rev = CTxdStore::GetParentTxdSlot(s);
        EXPECT_EQ(orig, rev);
    }
}
