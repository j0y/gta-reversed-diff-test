// test_CWaterCreatures2.cpp — Additional differential tests for WaterCreatureManager_c.
// Hook paths: Global/WaterCreatureManager_c/<fn>
//
// Extends test_CWaterCreatures.cpp with tests for:
// - CanAddWaterCreatureAtPos at various positions (ocean, inland, edge cases)

#include "StdInc.h"
#include "TestFramework.h"
#include "WaterCreatureManager_c.h"

// --- CanAddWaterCreatureAtPos with ocean positions ---

GAME_DIFF_TEST(WaterCreatureManager, CanAddWaterCreatureAtPos_OceanDeep) {
    // Deep ocean position — far offshore, should be valid for water creatures
    CVector oceanPos(-2000.0f, -2000.0f, -30.0f);
    for (int type = 0; type < WaterCreatureManager_c::NUM_WATER_CREATURE_INFOS; type++) {
        bool orig, rev;
        { HookDisableGuard guard("Global/WaterCreatureManager_c/CanAddWaterCreatureAtPos");
          orig = g_waterCreatureMan.CanAddWaterCreatureAtPos(type, oceanPos); }
        rev = g_waterCreatureMan.CanAddWaterCreatureAtPos(type, oceanPos);
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(WaterCreatureManager, CanAddWaterCreatureAtPos_InlandDry) {
    // Inland position — no water, should generally return false
    CVector inlandPos(2488.0f, -1666.0f, 50.0f);
    for (int type = 0; type < WaterCreatureManager_c::NUM_WATER_CREATURE_INFOS; type++) {
        bool orig, rev;
        { HookDisableGuard guard("Global/WaterCreatureManager_c/CanAddWaterCreatureAtPos");
          orig = g_waterCreatureMan.CanAddWaterCreatureAtPos(type, inlandPos); }
        rev = g_waterCreatureMan.CanAddWaterCreatureAtPos(type, inlandPos);
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(WaterCreatureManager, CanAddWaterCreatureAtPos_SantaMariaBeach) {
    // Near Santa Maria Beach — shallow coastal water
    CVector beachPos(300.0f, -1850.0f, -5.0f);
    for (int type = 0; type < WaterCreatureManager_c::NUM_WATER_CREATURE_INFOS; type++) {
        bool orig, rev;
        { HookDisableGuard guard("Global/WaterCreatureManager_c/CanAddWaterCreatureAtPos");
          orig = g_waterCreatureMan.CanAddWaterCreatureAtPos(type, beachPos); }
        rev = g_waterCreatureMan.CanAddWaterCreatureAtPos(type, beachPos);
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(WaterCreatureManager, CanAddWaterCreatureAtPos_Origin) {
    // World origin — edge case
    CVector originPos(0.0f, 0.0f, 0.0f);
    for (int type = 0; type < WaterCreatureManager_c::NUM_WATER_CREATURE_INFOS; type++) {
        bool orig, rev;
        { HookDisableGuard guard("Global/WaterCreatureManager_c/CanAddWaterCreatureAtPos");
          orig = g_waterCreatureMan.CanAddWaterCreatureAtPos(type, originPos); }
        rev = g_waterCreatureMan.CanAddWaterCreatureAtPos(type, originPos);
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(WaterCreatureManager, CanAddWaterCreatureAtPos_MapEdge) {
    // Far map edge — extreme coordinates
    CVector edgePos(3000.0f, 3000.0f, -20.0f);
    for (int type = 0; type < WaterCreatureManager_c::NUM_WATER_CREATURE_INFOS; type++) {
        bool orig, rev;
        { HookDisableGuard guard("Global/WaterCreatureManager_c/CanAddWaterCreatureAtPos");
          orig = g_waterCreatureMan.CanAddWaterCreatureAtPos(type, edgePos); }
        rev = g_waterCreatureMan.CanAddWaterCreatureAtPos(type, edgePos);
        EXPECT_EQ(orig, rev);
    }
}
