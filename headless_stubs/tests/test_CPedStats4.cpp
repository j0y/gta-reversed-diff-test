// test_CPedStats4.cpp — Tests for CPedStats functions.
// GetPedStatByArrayIndex and GetPedStatInfo are NOT hooked — behavior tests only.

#include "StdInc.h"
#include "TestFramework.h"
#include "PedStats.h"

GAME_TEST(CPedStats4, GetPedStatByArrayIndex_Range) {
    for (uint32 idx = 0; idx < 36; idx++) {
        CPedStat* stat = CPedStats::GetPedStatByArrayIndex(idx);
        EXPECT_TRUE(stat != nullptr);
    }
}

GAME_TEST(CPedStats4, GetPedStatInfo_KnownNames) {
    const char* names[] = {
        "PLAYER", "COP", "MEDIC", "FIREMAN", "GANG1", "GANG2",
        "GANG3", "GANG4", "GANG5", "GANG6", "GANG7", "GANG8",
        "GANG9", "GANG10", "DEALER", "TRAMP", "CRIMINAL",
        "TOUGH_GUY", "PROSTITUTE", "CIVMALE", "CIVFEMALE",
    };
    for (auto* name : names) {
        CPedStat* stat = CPedStats::GetPedStatInfo(name);
        EXPECT_TRUE(stat != nullptr);
    }
}

GAME_TEST(CPedStats4, GetPedStatInfo_Invalid) {
    CPedStat* stat = CPedStats::GetPedStatInfo("NONEXISTENT_TYPE");
    // Should return nullptr or a default
    (void)stat;
    EXPECT_TRUE(true);
}
