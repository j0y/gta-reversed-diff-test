// test_CWaterCreatures.cpp — Differential + behavior tests for WaterCreatureManager_c.
// Hook paths: Global/WaterCreatureManager_c/<fn>
//
// Fish/jellyfish/turtle/dolphin spawning system. 128 creature slots.
// Init, GetRandomWaterCreatureId, CanAddWaterCreatureAtPos are safe to test.
// Update/TryToFreeUp/TryToExitGroup need active creatures with CObject pointers.

#include "StdInc.h"
#include "TestFramework.h"
#include "WaterCreatureManager_c.h"

// --- Differential tests ---

// GetRandomWaterCreatureId is private after upstream refactor (#1266).
// Call original via direct function pointer, reversed via same.
GAME_DIFF_TEST(WaterCreatureManager, GetRandomWaterCreatureId) {
    // Seed both exe and DLL CRTs — original inlines rand() to exe's CRT at 0x821B1E
    SeedBothRng(42);

    // Call reversed 20 times
    using GetIdFn = int32(__thiscall*)(WaterCreatureManager_c*);
    auto getIdFn = reinterpret_cast<GetIdFn>(0x6E4040);

    int32 revResults[20];
    for (int i = 0; i < 20; i++) {
        revResults[i] = getIdFn(&g_waterCreatureMan);
    }

    SeedBothRng(42);
    for (int i = 0; i < 20; i++) {
        int32 orig;
        { HookDisableGuard guard("Global/WaterCreatureManager_c/GetRandomWaterCreatureId");
          orig = getIdFn(&g_waterCreatureMan); }
        EXPECT_EQ(revResults[i], orig);
    }
}

// --- Behavior tests ---

GAME_TEST(WaterCreatureManager, CreatureInfoTable) {
    // Verify the static creature info table has reasonable values
    for (int i = 0; i < WaterCreatureManager_c::NUM_WATER_CREATURE_INFOS; i++) {
        auto& info = WaterCreatureManager_c::GetCreatureInfo(i);
        EXPECT_TRUE(info.m_nMinCreated <= info.m_nMaxCreated);
        EXPECT_TRUE(info.m_fMinSpeed <= info.m_fMaxSpeed);
        EXPECT_TRUE(info.m_fMinSpawnDepth >= 0.0f);
        EXPECT_TRUE(info.m_fMinDistFromSameCreature >= 0.0f);
    }
}

GAME_TEST(WaterCreatureManager, RandomCreatureIdRange) {
    // GetRandomWaterCreatureId should return 0-6 (NUM_WATER_CREATURE_INFOS-1)
    SeedBothRng(123);
    using GetIdFn = int32(__thiscall*)(WaterCreatureManager_c*);
    auto getIdFn = reinterpret_cast<GetIdFn>(0x6E4040);
    for (int i = 0; i < 100; i++) {
        int32 id = getIdFn(&g_waterCreatureMan);
        EXPECT_TRUE(id >= 0 && id < WaterCreatureManager_c::NUM_WATER_CREATURE_INFOS);
    }
}

GAME_TEST(WaterCreatureManager, RandomCreatureDistribution) {
    // Verify the distribution: ~80% small fish (0-2), ~10% jellyfish (3-4),
    // ~7% turtle (5), ~3% dolphin (6)
    SeedBothRng(42);
    using GetIdFn = int32(__thiscall*)(WaterCreatureManager_c*);
    auto getIdFn = reinterpret_cast<GetIdFn>(0x6E4040);
    int counts[7] = {};
    for (int i = 0; i < 1000; i++) {
        int32 id = getIdFn(&g_waterCreatureMan);
        if (id >= 0 && id < 7) counts[id]++;
    }
    int smallFish = counts[0] + counts[1] + counts[2];
    int jellyfish = counts[3] + counts[4];
    int turtles = counts[5];
    int dolphins = counts[6];

    // Loose bounds — just verify the distribution is roughly correct
    EXPECT_TRUE(smallFish > 600);   // ~80% of 1000
    EXPECT_TRUE(jellyfish > 50);    // ~10%
    EXPECT_TRUE(turtles > 10);      // ~7%
    EXPECT_TRUE(dolphins > 5);      // ~3%
    EXPECT_TRUE(dolphins < turtles); // dolphins rarer than turtles
}

GAME_TEST(WaterCreatureManager, CreatureTypeEnum) {
    EXPECT_EQ((int)FISH1, 0);
    EXPECT_EQ((int)DOLPHIN, 6);
    EXPECT_EQ(WaterCreatureManager_c::NUM_WATER_CREATURE_INFOS, 7);
}
