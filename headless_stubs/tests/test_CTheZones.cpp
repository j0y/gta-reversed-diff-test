// test_CTheZones.cpp — Differential tests for CTheZones.
// Hook paths: Global/CTheZones/<fn>

#include "StdInc.h"
#include "TestFramework.h"

GAME_DIFF_TEST(CTheZones, GetLevelFromPosition) {
    CVector positions[] = {
        {2488, -1666, 13},   // Los Santos (Grove Street area)
        {-2200, 2300, 30},   // San Fierro
        {1500, 2500, 20},    // Las Venturas
        {-500, -500, 10},    // Countryside
        {0, 0, 0},           // Origin
    };
    for (auto& pos : positions) {
        eLevelName orig, rev;
        { HookDisableGuard guard("Global/CTheZones/GetLevelFromPosition"); orig = CTheZones::GetLevelFromPosition(pos); }
        rev = CTheZones::GetLevelFromPosition(pos);
        EXPECT_EQ((int)orig, (int)rev);
    }
}
