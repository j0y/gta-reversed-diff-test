// test_CPlaceable.cpp — Differential tests for CPlaceable base class methods.
// Hook paths: Entity/CPlaceable/<fn>
// Tests on player ped (which inherits from CPlaceable) and spawned vehicle.
//
// CPlaceable is the base for all positioned entities — fundamental to
// correct entity orientation and area checks.

#include "StdInc.h"
#include "TestFramework.h"
#include "ScenarioHelpers.h"
#include "Entity/Placeable.h"

// --- Player ped tests ---

GAME_DIFF_TEST(CPlaceable, GetHeading_Player) {
    auto* ped = FindPlayerPed(0);
    if (!ped) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No player ped"); return; }
    float orig, rev;
    { HookDisableGuard guard("Entity/CPlaceable/GetHeading"); orig = ped->GetHeading(); }
    rev = ped->GetHeading();
    EXPECT_NEAR(orig, rev, 1e-5f);
}

GAME_DIFF_TEST(CPlaceable, GetRoll_Player) {
    auto* ped = FindPlayerPed(0);
    if (!ped) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No player ped"); return; }
    float orig, rev;
    { HookDisableGuard guard("Entity/CPlaceable/GetRoll"); orig = ped->GetRoll(); }
    rev = ped->GetRoll();
    EXPECT_NEAR(orig, rev, 1e-5f);
}

// IsWithinArea — test player against known areas
GAME_DIFF_TEST(CPlaceable, IsWithinArea_Player_2D) {
    auto* ped = FindPlayerPed(0);
    if (!ped) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No player ped"); return; }

    // Grove Street bounding box (player spawns here)
    struct { float x1, y1, x2, y2; } areas[] = {
        { 2400.f, -1750.f, 2550.f, -1600.f }, // Grove Street — should be inside
        { 0.f, 0.f, 100.f, 100.f },            // Far away — should be outside
        { -3000.f, -3000.f, 3000.f, 3000.f },  // Entire map — should be inside
    };
    for (auto& a : areas) {
        bool orig, rev;
        { HookDisableGuard guard("Entity/CPlaceable/IsWithinArea-xy");
          orig = ped->IsWithinArea(a.x1, a.y1, a.x2, a.y2); }
        rev = ped->IsWithinArea(a.x1, a.y1, a.x2, a.y2);
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(CPlaceable, IsWithinArea_Player_3D) {
    auto* ped = FindPlayerPed(0);
    if (!ped) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No player ped"); return; }

    struct { float x1, y1, z1, x2, y2, z2; } areas[] = {
        { 2400.f, -1750.f, 0.f, 2550.f, -1600.f, 50.f },    // Grove Street 3D
        { 0.f, 0.f, 0.f, 100.f, 100.f, 100.f },              // Far away
        { -3000.f, -3000.f, -100.f, 3000.f, 3000.f, 500.f },  // Entire map
    };
    for (auto& a : areas) {
        bool orig, rev;
        { HookDisableGuard guard("Entity/CPlaceable/IsWithinArea-xyz");
          orig = ped->IsWithinArea(a.x1, a.y1, a.z1, a.x2, a.y2, a.z2); }
        rev = ped->IsWithinArea(a.x1, a.y1, a.z1, a.x2, a.y2, a.z2);
        EXPECT_EQ(orig, rev);
    }
}

// --- Spawned vehicle tests ---

static Scenario::TestVehicle* s_placeVeh = nullptr;
static bool s_triedPlaceVeh = false;

static CVehicle* GetPlaceableVehicle() {
    if (!s_triedPlaceVeh) {
        s_triedPlaceVeh = true;
        int32 model = Scenario::FindLoadedVehicleModel();
        if (model >= 0) {
            s_placeVeh = new Scenario::TestVehicle(model, CVector(2498.0f, -1665.0f, 13.5f));
        }
    }
    return s_placeVeh ? s_placeVeh->AsVehicle() : nullptr;
}

GAME_DIFF_TEST(CPlaceable, GetHeading_Vehicle) {
    auto* veh = GetPlaceableVehicle();
    if (!veh) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No vehicle"); return; }
    float orig, rev;
    { HookDisableGuard guard("Entity/CPlaceable/GetHeading"); orig = veh->GetHeading(); }
    rev = veh->GetHeading();
    EXPECT_NEAR(orig, rev, 1e-5f);
}

GAME_DIFF_TEST(CPlaceable, GetRoll_Vehicle) {
    auto* veh = GetPlaceableVehicle();
    if (!veh) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No vehicle"); return; }
    float orig, rev;
    { HookDisableGuard guard("Entity/CPlaceable/GetRoll"); orig = veh->GetRoll(); }
    rev = veh->GetRoll();
    EXPECT_NEAR(orig, rev, 1e-5f);
}

GAME_DIFF_TEST(CPlaceable, IsWithinArea_Vehicle) {
    auto* veh = GetPlaceableVehicle();
    if (!veh) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No vehicle"); return; }
    // Vehicle placed near (2498, -1665, 13.5)
    bool orig, rev;
    { HookDisableGuard guard("Entity/CPlaceable/IsWithinArea-xy");
      orig = veh->IsWithinArea(2490.f, -1670.f, 2505.f, -1660.f); }
    rev = veh->IsWithinArea(2490.f, -1670.f, 2505.f, -1660.f);
    EXPECT_EQ(orig, rev);
}
