// test_CCarGenerator3.cpp — More CCarGenerator differential tests.
// Hook paths: Global/CCarGenerator/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "CarGenerator.h"
#include "TheCarGenerators.h"

// --- CheckForBlockage: bool, test on first 10 generators with their own model IDs ---

// CheckForBlockage crashes on generators with invalid/unloaded models.
// Only test generators with loaded models.
GAME_DIFF_TEST(CCarGenerator3, CheckForBlockage_Safe) {
    uint32 numGens = CTheCarGenerators::NumOfCarGenerators;
    auto& generators = CTheCarGenerators::CarGeneratorArray;
    for (uint32 i = 0; i < numGens && i < 50; i++) {
        auto& gen = generators[i];
        int32 modelId = gen.m_nModelId;
        if (modelId <= 0 || !CStreaming::IsModelLoaded(modelId)) continue;
        bool orig, rev;
        { HookDisableGuard guard("Global/CCarGenerator/CheckForBlockage"); orig = gen.CheckForBlockage(modelId); }
        rev = gen.CheckForBlockage(modelId);
        EXPECT_EQ(orig, rev);
    }
}

// --- CalcNextGen: static uint32, call and compare ---

GAME_DIFF_TEST(CCarGenerator3, CalcNextGen) {
    uint32 orig, rev;
    { HookDisableGuard guard("Global/CCarGenerator/CalcNextGen"); orig = CCarGenerator::CalcNextGen(); }
    rev = CCarGenerator::CalcNextGen();
    EXPECT_EQ(orig, rev);
}
