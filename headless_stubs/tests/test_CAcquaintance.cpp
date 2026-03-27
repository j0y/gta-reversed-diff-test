// test_CAcquaintance.cpp — Differential tests for CAcquaintance.
// Hook paths: Global/CAcquaintance/<fn>
//
// Bitfield-based acquaintance tracking for peds.
// 5 slots (respect, like, dislike, hate, NUM), each a uint32 bitmask.

#include "StdInc.h"
#include "TestFramework.h"
#include "Acquaintance.h"

// --- GetAcquaintances / SetAcquaintances ---

GAME_DIFF_TEST(CAcquaintance, GetSetAcquaintances) {
    CAcquaintance acq{};
    uint32 values[] = { 0, 1, 0xFF, 0xDEADBEEF, 0xFFFFFFFF };
    for (uint32 id = 0; id < 5; id++) {
        for (uint32 val : values) {
            // Set via reversed
            acq.SetAcquaintances(id, val);
            uint32 rev = acq.GetAcquaintances(id);

            // Set via original and read back
            { HookDisableGuard guard("Global/CAcquaintance/SetAcquaintances");
              acq.SetAcquaintances(id, val); }
            uint32 orig;
            { HookDisableGuard guard("Global/CAcquaintance/GetAcquaintances");
              orig = acq.GetAcquaintances(id); }

            EXPECT_EQ(orig, rev);
        }
    }
}

// --- ClearAcquaintances ---

GAME_DIFF_TEST(CAcquaintance, ClearAcquaintances) {
    for (uint32 id = 0; id < 5; id++) {
        CAcquaintance orig{}, rev{};
        // Fill with non-zero
        orig.SetAcquaintances(id, 0xFFFFFFFF);
        rev.SetAcquaintances(id, 0xFFFFFFFF);

        { HookDisableGuard guard("Global/CAcquaintance/ClearAcquaintances");
          orig.ClearAcquaintances(id); }
        rev.ClearAcquaintances(id);

        EXPECT_EQ(orig.GetAcquaintances(id), rev.GetAcquaintances(id));
        EXPECT_EQ(orig.GetAcquaintances(id), (uint32)0);
    }
}

// --- SetAsAcquaintance ---

GAME_DIFF_TEST(CAcquaintance, SetAsAcquaintance) {
    // SetAsAcquaintance(id, bitMask) should set bit in slot id
    // and clear it from all other slots
    uint32 bits[] = { 1, 2, 4, 0x80, 0x100 };
    for (uint32 id = 0; id < 5; id++) {
        for (uint32 bit : bits) {
            CAcquaintance orig{}, rev{};
            // Pre-fill all slots with the bit
            for (uint32 i = 0; i < 5; i++) {
                orig.SetAcquaintances(i, bit);
                rev.SetAcquaintances(i, bit);
            }

            { HookDisableGuard guard("Global/CAcquaintance/SetAsAcquaintance");
              orig.SetAsAcquaintance(id, bit); }
            rev.SetAsAcquaintance(id, bit);

            // Compare all slots
            for (uint32 i = 0; i < 5; i++) {
                EXPECT_EQ(orig.GetAcquaintances(i), rev.GetAcquaintances(i));
            }
        }
    }
}

// --- ClearAsAcquaintance ---

GAME_DIFF_TEST(CAcquaintance, ClearAsAcquaintance) {
    uint32 bits[] = { 1, 2, 0xFF, 0xFFFFFFFF };
    for (uint32 id = 0; id < 5; id++) {
        for (uint32 bit : bits) {
            CAcquaintance orig{}, rev{};
            orig.SetAcquaintances(id, 0xFFFFFFFF);
            rev.SetAcquaintances(id, 0xFFFFFFFF);

            { HookDisableGuard guard("Global/CAcquaintance/ClearAsAcquaintance");
              orig.ClearAsAcquaintance(id, bit); }
            rev.ClearAsAcquaintance(id, bit);

            EXPECT_EQ(orig.GetAcquaintances(id), rev.GetAcquaintances(id));
        }
    }
}
