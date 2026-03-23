// test_CPedStats2.cpp — Deep differential tests for CPedStats.
// Hook paths: Global/CPedStats/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "PedStats.h"

// --- GetPedStatType ---

GAME_DIFF_TEST(CPedStats, GetPedStatType_KnownNames) {
    const char* names[] = {
        "PLAYER", "COP", "MEDIC", "FIREMAN",
        "GANG1", "GANG2", "GANG3", "GANG4",
        "TRIAD", "MAFIA", "BALLAS", "GROVE",
        "DEALER", "PROSTITUTE", "CRIMINAL",
        "FATMALE", "FATFEMALE", "THINMALE", "THINFEMALE",
        "INVALID_STAT_NAME_XYZ"
    };
    for (auto* name : names) {
        int32 orig, rev;
        { HookDisableGuard guard("Global/CPedStats/GetPedStatType");
          orig = (int32)CPedStats::GetPedStatType(name); }
        rev = (int32)CPedStats::GetPedStatType(name);
        EXPECT_EQ(orig, rev);
    }
}
