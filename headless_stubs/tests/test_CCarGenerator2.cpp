// test_CCarGenerator2.cpp — Differential tests for CCarGenerator.
// Hook paths: Global/CCarGenerator/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "CarGenerator.h"
#include "TheCarGenerators.h"

GAME_DIFF_TEST(CCarGenerator2, CheckIfWithinRangeOfAnyPlayers) {
    uint32 numGens = CTheCarGenerators::NumOfCarGenerators;
    auto& generators = CTheCarGenerators::CarGeneratorArray;
    for (uint32 i = 0; i < 10 && i < numGens; i++) {
        auto& gen = generators[i];
        bool orig, rev;
        { HookDisableGuard guard("Global/CCarGenerator/CheckIfWithinRangeOfAnyPlayers"); orig = gen.CheckIfWithinRangeOfAnyPlayers(); }
        rev = gen.CheckIfWithinRangeOfAnyPlayers();
        EXPECT_EQ(orig, rev);
    }
}
