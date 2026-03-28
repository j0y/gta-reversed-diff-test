// test_CWaterCreatures2.cpp — Additional differential tests for WaterCreatureManager_c.
// Hook paths: Global/WaterCreatureManager_c/<fn>
//
// Extends test_CWaterCreatures.cpp with tests for:
// - CanAddWaterCreatureAtPos at various positions (ocean, inland, edge cases)

#include "StdInc.h"
#include "TestFramework.h"
#include "WaterCreatureManager_c.h"

// CanAddWaterCreatureAtPos is private after upstream refactor (#1266).
// Call via direct function pointer at original address.
using CanAddFn = bool(__thiscall*)(WaterCreatureManager_c*, int32, CVector);
static auto canAddFn = reinterpret_cast<CanAddFn>(0x6E3F40);

// --- CanAddWaterCreatureAtPos with ocean positions ---

GAME_DIFF_TEST(WaterCreatureManager, CanAddWaterCreatureAtPos_OceanDeep) {
    // Deep ocean position — far offshore, should be valid for water creatures
    CVector oceanPos(-2000.0f, -2000.0f, -30.0f);
    for (int type = 0; type < WaterCreatureManager_c::NUM_WATER_CREATURE_INFOS; type++) {
        bool orig, rev;
        { HookDisableGuard guard("Global/WaterCreatureManager_c/CanAddWaterCreatureAtPos");
          orig = canAddFn(&g_waterCreatureMan, type, oceanPos); }
        rev = canAddFn(&g_waterCreatureMan, type, oceanPos);
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(WaterCreatureManager, CanAddWaterCreatureAtPos_InlandDry) {
    CVector inlandPos(2488.0f, -1666.0f, 50.0f);
    for (int type = 0; type < WaterCreatureManager_c::NUM_WATER_CREATURE_INFOS; type++) {
        bool orig, rev;
        { HookDisableGuard guard("Global/WaterCreatureManager_c/CanAddWaterCreatureAtPos");
          orig = canAddFn(&g_waterCreatureMan, type, inlandPos); }
        rev = canAddFn(&g_waterCreatureMan, type, inlandPos);
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(WaterCreatureManager, CanAddWaterCreatureAtPos_SantaMariaBeach) {
    CVector beachPos(300.0f, -1850.0f, -5.0f);
    for (int type = 0; type < WaterCreatureManager_c::NUM_WATER_CREATURE_INFOS; type++) {
        bool orig, rev;
        { HookDisableGuard guard("Global/WaterCreatureManager_c/CanAddWaterCreatureAtPos");
          orig = canAddFn(&g_waterCreatureMan, type, beachPos); }
        rev = canAddFn(&g_waterCreatureMan, type, beachPos);
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(WaterCreatureManager, CanAddWaterCreatureAtPos_Origin) {
    CVector originPos(0.0f, 0.0f, 0.0f);
    for (int type = 0; type < WaterCreatureManager_c::NUM_WATER_CREATURE_INFOS; type++) {
        bool orig, rev;
        { HookDisableGuard guard("Global/WaterCreatureManager_c/CanAddWaterCreatureAtPos");
          orig = canAddFn(&g_waterCreatureMan, type, originPos); }
        rev = canAddFn(&g_waterCreatureMan, type, originPos);
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(WaterCreatureManager, CanAddWaterCreatureAtPos_MapEdge) {
    CVector edgePos(3000.0f, 3000.0f, -20.0f);
    for (int type = 0; type < WaterCreatureManager_c::NUM_WATER_CREATURE_INFOS; type++) {
        bool orig, rev;
        { HookDisableGuard guard("Global/WaterCreatureManager_c/CanAddWaterCreatureAtPos");
          orig = canAddFn(&g_waterCreatureMan, type, edgePos); }
        rev = canAddFn(&g_waterCreatureMan, type, edgePos);
        EXPECT_EQ(orig, rev);
    }
}
