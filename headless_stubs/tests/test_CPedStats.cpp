// test_CPedStats.cpp — Differential tests for CPedStats.
// Hook paths: Global/CPedStats/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "PedStats.h"

GAME_DIFF_TEST(CPedStats, GetPedStatType) {
    const char* names[] = {
        "PLAYER", "COP", "MEDIC", "FIREMAN", "GANG1", "GANG2",
        "NORMAL", "TOUGH_GUY", "TRAMP", "INVALID_STAT_XYZ"
    };
    for (auto name : names) {
        int orig, rev;
        { HookDisableGuard guard("Global/CPedStats/GetPedStatType");
          orig = (int)CPedStats::GetPedStatType(name); }
        rev = (int)CPedStats::GetPedStatType(name);
        EXPECT_EQ(orig, rev);
    }
}
