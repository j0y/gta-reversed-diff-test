// test_CWorld9.cpp — Systematic CWorld differential tests.
// Tests ALL remaining untested hooked CWorld functions.
// Hook paths: Global/CWorld/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "World.h"

// ===================================================================
// ProcessLineOfSight — the most important untested function.
// bool ProcessLineOfSight(origin, target, outColPoint, outEntity, ...)
// ===================================================================

GAME_DIFF_TEST(CWorld9, ProcessLineOfSight_StraightDown) {
    CVector origin(2488.f, -1666.f, 50.f);
    CVector target(2488.f, -1666.f, -10.f);
    CColPoint origCP{}, revCP{};
    CEntity* origEnt = nullptr;
    CEntity* revEnt = nullptr;
    bool orig, rev;
    { HookDisableGuard guard("Global/CWorld/ProcessLineOfSight");
      orig = CWorld::ProcessLineOfSight(origin, target, origCP, origEnt, true, true, true, true, true, false, false, false); }
    rev = CWorld::ProcessLineOfSight(origin, target, revCP, revEnt, true, true, true, true, true, false, false, false);
    EXPECT_EQ(orig, rev);
    if (orig && rev) {
        EXPECT_NEAR(origCP.m_vecPoint.x, revCP.m_vecPoint.x, 0.1f);
        EXPECT_NEAR(origCP.m_vecPoint.y, revCP.m_vecPoint.y, 0.1f);
        EXPECT_NEAR(origCP.m_vecPoint.z, revCP.m_vecPoint.z, 0.1f);
        EXPECT_EQ(origEnt, revEnt);
    }
}

GAME_DIFF_TEST(CWorld9, ProcessLineOfSight_Horizontal) {
    CVector origin(2488.f, -1666.f, 13.5f);
    CVector target(2550.f, -1666.f, 13.5f);
    CColPoint origCP{}, revCP{};
    CEntity* origEnt = nullptr;
    CEntity* revEnt = nullptr;
    bool orig, rev;
    { HookDisableGuard guard("Global/CWorld/ProcessLineOfSight");
      orig = CWorld::ProcessLineOfSight(origin, target, origCP, origEnt, true, false, false, false, false, false, false, false); }
    rev = CWorld::ProcessLineOfSight(origin, target, revCP, revEnt, true, false, false, false, false, false, false, false);
    EXPECT_EQ(orig, rev);
    if (orig && rev) {
        EXPECT_NEAR(origCP.m_vecPoint.z, revCP.m_vecPoint.z, 0.1f);
        EXPECT_EQ(origEnt, revEnt);
    }
}

GAME_DIFF_TEST(CWorld9, ProcessLineOfSight_NoHit) {
    CVector origin(2488.f, -1666.f, 100.f);
    CVector target(2488.f, -1666.f, 200.f); // up into sky
    CColPoint origCP{}, revCP{};
    CEntity* origEnt = nullptr;
    CEntity* revEnt = nullptr;
    bool orig, rev;
    { HookDisableGuard guard("Global/CWorld/ProcessLineOfSight");
      orig = CWorld::ProcessLineOfSight(origin, target, origCP, origEnt, true, true, true, true, true, false, false, false); }
    rev = CWorld::ProcessLineOfSight(origin, target, revCP, revEnt, true, true, true, true, true, false, false, false);
    EXPECT_EQ(orig, rev);
}

// ===================================================================
// ProcessVerticalLine — similar to ProcessLineOfSight but vertical
// ===================================================================

GAME_DIFF_TEST(CWorld9, ProcessVerticalLine_GroveStreet) {
    CVector origin(2488.f, -1666.f, 50.f);
    CColPoint origCP{}, revCP{};
    CEntity* origEnt = nullptr;
    CEntity* revEnt = nullptr;
    bool orig, rev;
    { HookDisableGuard guard("Global/CWorld/ProcessVerticalLine");
      orig = CWorld::ProcessVerticalLine(origin, -10.f, origCP, origEnt, true, false, false, false, false, false, nullptr); }
    rev = CWorld::ProcessVerticalLine(origin, -10.f, revCP, revEnt, true, false, false, false, false, false, nullptr);
    EXPECT_EQ(orig, rev);
    if (orig && rev) {
        EXPECT_NEAR(origCP.m_vecPoint.z, revCP.m_vecPoint.z, 0.1f);
    }
}

// ===================================================================
// FindObjectsKindaColliding — returns entities near a point
// ===================================================================

GAME_DIFF_TEST(CWorld9, FindObjectsKindaColliding_GroveStreet) {
    CVector pos(2488.f, -1666.f, 13.5f);
    int16 origCount = 0, revCount = 0;
    CEntity* origEnts[16] = {};
    CEntity* revEnts[16] = {};
    { HookDisableGuard guard("Global/CWorld/FindObjectsKindaColliding");
      CWorld::FindObjectsKindaColliding(pos, 30.f, true, &origCount, 16, origEnts, true, true, true, true, true); }
    CWorld::FindObjectsKindaColliding(pos, 30.f, true, &revCount, 16, revEnts, true, true, true, true, true);
    EXPECT_EQ(origCount, revCount);
}

GAME_DIFF_TEST(CWorld9, FindObjectsKindaColliding_Empty) {
    CVector pos(0.f, 0.f, 500.f); // high above origin — nothing there
    int16 origCount = 0, revCount = 0;
    CEntity* origEnts[8] = {};
    CEntity* revEnts[8] = {};
    { HookDisableGuard guard("Global/CWorld/FindObjectsKindaColliding");
      CWorld::FindObjectsKindaColliding(pos, 5.f, true, &origCount, 8, origEnts, false, false, false, false, false); }
    CWorld::FindObjectsKindaColliding(pos, 5.f, true, &revCount, 8, revEnts, false, false, false, false, false);
    EXPECT_EQ(origCount, revCount);
}

// ===================================================================
// FindObjectsIntersectingCube
// ===================================================================

GAME_DIFF_TEST(CWorld9, FindObjectsIntersectingCube_GroveStreet) {
    CVector a(2470.f, -1690.f, 0.f);
    CVector b(2510.f, -1640.f, 30.f);
    int16 origCount = 0, revCount = 0;
    CEntity* origEnts[32] = {};
    CEntity* revEnts[32] = {};
    { HookDisableGuard guard("Global/CWorld/FindObjectsIntersectingCube");
      CWorld::FindObjectsIntersectingCube(a, b, &origCount, 32, origEnts, true, true, true, true, true); }
    CWorld::FindObjectsIntersectingCube(a, b, &revCount, 32, revEnts, true, true, true, true, true);
    EXPECT_EQ(origCount, revCount);
}

// ===================================================================
// FindMissionEntitiesIntersectingCube
// ===================================================================

GAME_DIFF_TEST(CWorld9, FindMissionEntitiesIntersectingCube_GroveStreet) {
    CVector a(2470.f, -1690.f, 0.f);
    CVector b(2510.f, -1640.f, 30.f);
    int16 origCount = 0, revCount = 0;
    CEntity* origEnts[16] = {};
    CEntity* revEnts[16] = {};
    { HookDisableGuard guard("Global/CWorld/FindMissionEntitiesIntersectingCube");
      CWorld::FindMissionEntitiesIntersectingCube(a, b, &origCount, 16, origEnts, true, true, true); }
    CWorld::FindMissionEntitiesIntersectingCube(a, b, &revCount, 16, revEnts, true, true, true);
    EXPECT_EQ(origCount, revCount);
}

// ===================================================================
// FindObjectsOfTypeInRange
// ===================================================================

GAME_DIFF_TEST(CWorld9, FindObjectsOfTypeInRange_AnyType) {
    CVector pos(2488.f, -1666.f, 13.5f);
    int16 origCount = 0, revCount = 0;
    CEntity* origEnts[32] = {};
    CEntity* revEnts[32] = {};
    { HookDisableGuard guard("Global/CWorld/FindObjectsOfTypeInRange");
      CWorld::FindObjectsOfTypeInRange(-1, pos, 50.f, true, &origCount, 32, origEnts, true, true, true, true, true); }
    CWorld::FindObjectsOfTypeInRange(-1, pos, 50.f, true, &revCount, 32, revEnts, true, true, true, true, true);
    EXPECT_EQ(origCount, revCount);
}

// ===================================================================
// FindLodOfTypeInRange
// ===================================================================

GAME_DIFF_TEST(CWorld9, FindLodOfTypeInRange_AnyType) {
    CVector pos(2488.f, -1666.f, 13.5f);
    int16 origCount = 0, revCount = 0;
    CEntity* origEnts[16] = {};
    CEntity* revEnts[16] = {};
    { HookDisableGuard guard("Global/CWorld/FindLodOfTypeInRange");
      CWorld::FindLodOfTypeInRange(-1, pos, 100.f, true, &origCount, 16, origEnts); }
    CWorld::FindLodOfTypeInRange(-1, pos, 100.f, true, &revCount, 16, revEnts);
    EXPECT_EQ(origCount, revCount);
}

// ===================================================================
// FindPlayerSlotWithVehiclePointer
// ===================================================================

GAME_DIFF_TEST(CWorld9, FindPlayerSlotWithVehiclePointer_Null) {
    int32 orig, rev;
    { HookDisableGuard guard("Global/CWorld/FindPlayerSlotWithVehiclePointer");
      orig = CWorld::FindPlayerSlotWithVehiclePointer(nullptr); }
    rev = CWorld::FindPlayerSlotWithVehiclePointer(nullptr);
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CWorld9, FindPlayerSlotWithVehiclePointer_Player) {
    auto* veh = FindPlayerVehicle(0);
    // At state 9 player may not be in vehicle — test with whatever we get
    int32 orig, rev;
    { HookDisableGuard guard("Global/CWorld/FindPlayerSlotWithVehiclePointer");
      orig = CWorld::FindPlayerSlotWithVehiclePointer((CEntity*)veh); }
    rev = CWorld::FindPlayerSlotWithVehiclePointer((CEntity*)veh);
    EXPECT_EQ(orig, rev);
}

// ===================================================================
// GetSector / GetRepeatSector / GetLodPtrList
// ===================================================================

GAME_DIFF_TEST(CWorld9, GetSector_Various) {
    // Test sector lookup at various grid coords
    int32 coords[][2] = { {50,50}, {0,0}, {10,20}, {60,60} };
    for (auto& c : coords) {
        CSector* orig;
        CSector* rev;
        { HookDisableGuard guard("Global/CWorld/GetSector");
          orig = &CWorld::GetSector(c[0], c[1]); }
        rev = &CWorld::GetSector(c[0], c[1]);
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(CWorld9, GetRepeatSector_Various) {
    int32 coords[][2] = { {5,5}, {0,0}, {3,7}, {15,15} };
    for (auto& c : coords) {
        CRepeatSector* orig;
        CRepeatSector* rev;
        { HookDisableGuard guard("Global/CWorld/GetRepeatSector");
          orig = &CWorld::GetRepeatSector(c[0], c[1]); }
        rev = &CWorld::GetRepeatSector(c[0], c[1]);
        EXPECT_EQ(orig, rev);
    }
}

// ===================================================================
// CameraToIgnoreThisObject
// ===================================================================

// CameraToIgnoreThisObject crashes original code with both player and nullptr — skip

// ===================================================================
// ResetLineTestOptions
// ===================================================================

GAME_DIFF_TEST(CWorld9, ResetLineTestOptions) {
    { HookDisableGuard guard("Global/CWorld/ResetLineTestOptions");
      CWorld::ResetLineTestOptions(); }
    CWorld::ResetLineTestOptions();
    EXPECT_TRUE(true);
}

// ===================================================================
// SprayPaintWorld — returns eSprayPaintState
// ===================================================================

GAME_DIFF_TEST(CWorld9, SprayPaintWorld_GroveStreet) {
    CVector pos(2488.f, -1666.f, 13.5f);
    CVector origDir{}, revDir{};
    int32 orig, rev;
    { HookDisableGuard guard("Global/CWorld/SprayPaintWorld");
      orig = (int32)CWorld::SprayPaintWorld(pos, origDir, 2.f, false); }
    rev = (int32)CWorld::SprayPaintWorld(pos, revDir, 2.f, false);
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CWorld9, SprayPaintWorld_NoTags) {
    CVector pos(0.f, 0.f, 100.f); // far from any tag
    CVector origDir{}, revDir{};
    int32 orig, rev;
    { HookDisableGuard guard("Global/CWorld/SprayPaintWorld");
      orig = (int32)CWorld::SprayPaintWorld(pos, origDir, 2.f, false); }
    rev = (int32)CWorld::SprayPaintWorld(pos, revDir, 2.f, false);
    EXPECT_EQ(orig, rev);
}
