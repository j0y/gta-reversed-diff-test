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

GAME_DIFF_TEST(WaterCreatureManager, GetRandomWaterCreatureId) {
    // Seed RNG for deterministic results, call N times and compare sequences
    srand(42);
    int32 revResults[20];
    for (int i = 0; i < 20; i++) {
        revResults[i] = g_waterCreatureMan.GetRandomWaterCreatureId();
    }

    srand(42);
    for (int i = 0; i < 20; i++) {
        int32 orig;
        { HookDisableGuard guard("Global/WaterCreatureManager_c/GetRandomWaterCreatureId");
          orig = g_waterCreatureMan.GetRandomWaterCreatureId(); }
        EXPECT_EQ(revResults[i], orig);
    }
}

GAME_DIFF_TEST(WaterCreatureManager, CanAddWaterCreatureAtPos_EmptyList) {
    // With no creatures created, any position should be valid for any type
    // Save and restore creature list state
    auto savedCreationTime = g_waterCreatureMan.m_nLastCreationCheckTime;

    for (int type = 0; type < WaterCreatureManager_c::NUM_WATER_CREATURE_INFOS; type++) {
        CVector testPos(100.0f, 100.0f, -10.0f);
        bool rev = g_waterCreatureMan.CanAddWaterCreatureAtPos(type, testPos);
        bool orig;
        { HookDisableGuard guard("Global/WaterCreatureManager_c/CanAddWaterCreatureAtPos");
          orig = g_waterCreatureMan.CanAddWaterCreatureAtPos(type, testPos); }
        EXPECT_EQ(rev, orig);
    }

    g_waterCreatureMan.m_nLastCreationCheckTime = savedCreationTime;
}

GAME_DIFF_TEST(WaterCreatureManager, Init) {
    // Save state
    auto savedCreationTime = g_waterCreatureMan.m_nLastCreationCheckTime;
    auto savedFreeCount = g_waterCreatureMan.m_freeList.GetNumItems();
    auto savedCreatedCount = g_waterCreatureMan.m_createdList.GetNumItems();

    // Only test Init if no creatures are currently active (safe to reinit)
    if (savedCreatedCount > 0) return;

    g_waterCreatureMan.m_nLastCreationCheckTime = 12345;
    g_waterCreatureMan.Init();
    int32 revTime = g_waterCreatureMan.m_nLastCreationCheckTime;
    uint32 revFreeCount = g_waterCreatureMan.m_freeList.GetNumItems();

    // Reset and do original
    g_waterCreatureMan.m_freeList.RemoveAll();
    g_waterCreatureMan.m_nLastCreationCheckTime = 12345;
    { HookDisableGuard guard("Global/WaterCreatureManager_c/Init");
      g_waterCreatureMan.Init(); }

    EXPECT_EQ(revTime, g_waterCreatureMan.m_nLastCreationCheckTime);
    EXPECT_EQ(revFreeCount, g_waterCreatureMan.m_freeList.GetNumItems());
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
    srand(123);
    for (int i = 0; i < 100; i++) {
        int32 id = g_waterCreatureMan.GetRandomWaterCreatureId();
        EXPECT_TRUE(id >= 0 && id < WaterCreatureManager_c::NUM_WATER_CREATURE_INFOS);
    }
}

GAME_TEST(WaterCreatureManager, RandomCreatureDistribution) {
    // Verify the distribution: ~80% small fish (0-2), ~10% jellyfish (3-4),
    // ~7% turtle (5), ~3% dolphin (6)
    srand(42);
    int counts[7] = {};
    for (int i = 0; i < 1000; i++) {
        int32 id = g_waterCreatureMan.GetRandomWaterCreatureId();
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

GAME_TEST(WaterCreatureManager, FreeListCount) {
    // At state 9, free list + created list should total NUM_WATER_CREATURES
    uint32 freeCount = g_waterCreatureMan.m_freeList.GetNumItems();
    uint32 createdCount = g_waterCreatureMan.m_createdList.GetNumItems();
    EXPECT_EQ(freeCount + createdCount, (uint32)WaterCreatureManager_c::NUM_WATER_CREATURES);
}

GAME_TEST(WaterCreatureManager, CreatureTypeEnum) {
    EXPECT_EQ((int)FISH1, 0);
    EXPECT_EQ((int)DOLPHIN, 6);
    EXPECT_EQ(WaterCreatureManager_c::NUM_WATER_CREATURE_INFOS, 7);
}
