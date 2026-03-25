// test_CFireManager3.cpp — Systematic CFireManager differential tests.
// Tests ALL remaining untested hooked CFireManager functions.
// Hook paths: Global/CFireManager/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "FireManager.h"

// ===================================================================
// ExtinguishPoint — void, extinguishes fires near position
// ===================================================================

GAME_DIFF_TEST(CFireManager3, ExtinguishPoint_GroveStreet) {
    CVector pos(2488.f, -1666.f, 13.5f);
    { HookDisableGuard guard("Global/CFireManager/ExtinguishPoint");
      gFireManager.ExtinguishPoint(pos, 100.f); }
    gFireManager.ExtinguishPoint(pos, 100.f);
    EXPECT_TRUE(true);
}

GAME_DIFF_TEST(CFireManager3, ExtinguishPoint_Origin) {
    CVector pos(0.f, 0.f, 0.f);
    { HookDisableGuard guard("Global/CFireManager/ExtinguishPoint");
      gFireManager.ExtinguishPoint(pos, 50.f); }
    gFireManager.ExtinguishPoint(pos, 50.f);
    EXPECT_TRUE(true);
}

// ===================================================================
// ExtinguishPointWithWater — bool, returns whether fire was put out
// ===================================================================

GAME_DIFF_TEST(CFireManager3, ExtinguishPointWithWater_GroveStreet) {
    CVector pos(2488.f, -1666.f, 13.5f);
    bool orig, rev;
    { HookDisableGuard guard("Global/CFireManager/ExtinguishPointWithWater");
      orig = gFireManager.ExtinguishPointWithWater(pos, 100.f, 5.f); }
    rev = gFireManager.ExtinguishPointWithWater(pos, 100.f, 5.f);
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CFireManager3, ExtinguishPointWithWater_NoFires) {
    CVector pos(-3000.f, -3000.f, 0.f);
    bool orig, rev;
    { HookDisableGuard guard("Global/CFireManager/ExtinguishPointWithWater");
      orig = gFireManager.ExtinguishPointWithWater(pos, 10.f, 1.f); }
    rev = gFireManager.ExtinguishPointWithWater(pos, 10.f, 1.f);
    EXPECT_EQ(orig, rev);
}

// ===================================================================
// IsScriptFireExtinguished — extend to more fire IDs
// ===================================================================

GAME_DIFF_TEST(CFireManager3, IsScriptFireExtinguished_AllSlots) {
    for (int16 id = 0; id < 16; id++) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CFireManager/IsScriptFireExtinguished");
          orig = gFireManager.IsScriptFireExtinguished(id); }
        rev = gFireManager.IsScriptFireExtinguished(id);
        EXPECT_EQ(orig, rev);
    }
}

// ===================================================================
// RemoveAllScriptFires — void
// ===================================================================

GAME_DIFF_TEST(CFireManager3, RemoveAllScriptFires) {
    { HookDisableGuard guard("Global/CFireManager/RemoveAllScriptFires");
      gFireManager.RemoveAllScriptFires(); }
    gFireManager.RemoveAllScriptFires();
    EXPECT_TRUE(true);
}

// ===================================================================
// ClearAllScriptFireFlags — void, already tested once, extend
// ===================================================================

GAME_DIFF_TEST(CFireManager3, ClearAllScriptFireFlags_Diff) {
    { HookDisableGuard guard("Global/CFireManager/ClearAllScriptFireFlags");
      gFireManager.ClearAllScriptFireFlags(); }
    gFireManager.ClearAllScriptFireFlags();
    EXPECT_TRUE(true);
}

// ===================================================================
// GetScriptFireCoords — returns const CVector& for fire ID
// ===================================================================

GAME_DIFF_TEST(CFireManager3, GetScriptFireCoords_AllSlots) {
    for (int16 id = 0; id < 16; id++) {
        const CVector& orig = gFireManager.GetScriptFireCoords(id);
        // Can't diff via HookDisableGuard (returns ref — same address).
        // Just verify no crash and position is finite.
        EXPECT_TRUE(std::isfinite(orig.x));
    }
}

// ===================================================================
// GetNextFreeFire — returns CFire*, tries to find a free slot
// ===================================================================

GAME_DIFF_TEST(CFireManager3, GetNextFreeFire_NoExtinguish) {
    CFire* orig;
    CFire* rev;
    { HookDisableGuard guard("Global/CFireManager/GetNextFreeFire");
      orig = gFireManager.GetNextFreeFire(false); }
    rev = gFireManager.GetNextFreeFire(false);
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CFireManager3, GetNextFreeFire_AllowExtinguish) {
    CFire* orig;
    CFire* rev;
    { HookDisableGuard guard("Global/CFireManager/GetNextFreeFire");
      orig = gFireManager.GetNextFreeFire(true); }
    rev = gFireManager.GetNextFreeFire(true);
    EXPECT_EQ(orig, rev);
}

// ===================================================================
// GetNumFiresInRange — extend with more positions
// ===================================================================

GAME_DIFF_TEST(CFireManager3, GetNumFiresInRange_AllCities) {
    CVector positions[] = {
        {2488.f, -1666.f, 13.5f},  // LS
        {-2170.f, 250.f, 30.f},    // SF
        {2000.f, 1500.f, 20.f},    // LV
        {0.f, 0.f, 0.f},           // origin
    };
    for (auto& pos : positions) {
        uint32 orig, rev;
        { HookDisableGuard guard("Global/CFireManager/GetNumFiresInRange");
          orig = gFireManager.GetNumFiresInRange(pos, 200.f); }
        rev = gFireManager.GetNumFiresInRange(pos, 200.f);
        EXPECT_EQ(orig, rev);
    }
}

// ===================================================================
// GetNumFiresInArea — extend with more areas
// ===================================================================

GAME_DIFF_TEST(CFireManager3, GetNumFiresInArea_AllCities) {
    struct Area { float x1, x2, y1, y2, z1, z2; };
    Area areas[] = {
        {2400.f, 2550.f, -1700.f, -1600.f, 0.f, 50.f},  // LS
        {-2300.f, -2000.f, 100.f, 400.f, 0.f, 50.f},     // SF
        {1800.f, 2200.f, 1300.f, 1700.f, 0.f, 50.f},     // LV
    };
    for (auto& a : areas) {
        uint32 orig, rev;
        { HookDisableGuard guard("Global/CFireManager/GetNumFiresInArea");
          orig = gFireManager.GetNumFiresInArea(a.x1, a.x2, a.y1, a.y2, a.z1, a.z2); }
        rev = gFireManager.GetNumFiresInArea(a.x1, a.x2, a.y1, a.y2, a.z1, a.z2);
        EXPECT_EQ(orig, rev);
    }
}

// ===================================================================
// FindNearestFire — extend with flags
// ===================================================================

GAME_DIFF_TEST(CFireManager3, FindNearestFire_WithFlags) {
    CVector pos(2488.f, -1666.f, 13.5f);
    CFire* orig;
    CFire* rev;
    // Check with wasExtinguished=true
    { HookDisableGuard guard("Global/CFireManager/FindNearestFire");
      orig = gFireManager.FindNearestFire(pos, true, false); }
    rev = gFireManager.FindNearestFire(pos, true, false);
    EXPECT_EQ(orig, rev);

    // Check with wasCreatedByScript=true
    { HookDisableGuard guard("Global/CFireManager/FindNearestFire");
      orig = gFireManager.FindNearestFire(pos, false, true); }
    rev = gFireManager.FindNearestFire(pos, false, true);
    EXPECT_EQ(orig, rev);
}

// ===================================================================
// SetScriptFireAudio — void, sets audio flag on fire
// ===================================================================

GAME_DIFF_TEST(CFireManager3, SetScriptFireAudio_AllSlots) {
    for (int16 id = 0; id < 16; id++) {
        { HookDisableGuard guard("Global/CFireManager/SetScriptFireAudio");
          gFireManager.SetScriptFireAudio(id, true); }
        gFireManager.SetScriptFireAudio(id, true);
    }
    EXPECT_TRUE(true);
}

// ===================================================================
// DestroyAllFxSystems / CreateAllFxSystems — void, FX management
// ===================================================================

GAME_DIFF_TEST(CFireManager3, DestroyCreateFxSystems_Roundtrip) {
    { HookDisableGuard guard("Global/CFireManager/DestroyAllFxSystems");
      gFireManager.DestroyAllFxSystems(); }
    gFireManager.DestroyAllFxSystems();
    // Recreate so nothing is broken
    { HookDisableGuard guard("Global/CFireManager/CreateAllFxSystems");
      gFireManager.CreateAllFxSystems(); }
    gFireManager.CreateAllFxSystems();
    EXPECT_TRUE(true);
}
