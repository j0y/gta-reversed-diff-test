// test_CPedStats3.cpp — Differential tests for CPedStats.
// Hook paths: Global/CPedStats/<fn>
//
// Tests ped stat type lookups across known stat names.

#include "StdInc.h"
#include "TestFramework.h"
#include "PedStats.h"

// --- GetPedStatType ---

GAME_DIFF_TEST(CPedStats3, GetPedStatType_KnownNames) {
    const char* names[] = {
        "PLAYER", "COP", "MEDIC", "FIREMAN", "GANG1", "GANG2", "GANG3",
        "GANG4", "GANG5", "GANG6", "GANG7", "GANG8", "GANG9", "GANG10",
        "DEALER", "TRAMP", "CRIMINAL", "TOUGH_GUY", "PROSTITUTE",
        "CIVMALE", "CIVFEMALE", "FATMALE", "FATFEMALE", "OLDFEMALE",
        "OLDMALE", "SKATER",
    };
    for (auto* name : names) {
        int32 orig, rev;
        { HookDisableGuard guard("Global/CPedStats/GetPedStatType");
          orig = (int32)CPedStats::GetPedStatType(name); }
        rev = (int32)CPedStats::GetPedStatType(name);
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(CPedStats3, GetPedStatType_Invalid) {
    int32 orig, rev;
    { HookDisableGuard guard("Global/CPedStats/GetPedStatType");
      orig = (int32)CPedStats::GetPedStatType("NONEXISTENT_TYPE"); }
    rev = (int32)CPedStats::GetPedStatType("NONEXISTENT_TYPE");
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CPedStats3, GetPedStatType_Empty) {
    int32 orig, rev;
    { HookDisableGuard guard("Global/CPedStats/GetPedStatType");
      orig = (int32)CPedStats::GetPedStatType(""); }
    rev = (int32)CPedStats::GetPedStatType("");
    EXPECT_EQ(orig, rev);
}
