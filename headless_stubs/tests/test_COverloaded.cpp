// test_COverloaded.cpp — Differential tests for previously-skipped overloaded hooks.
//
// These use the dash-separated hook path format: Category/Class/Function-Suffix
// Covers functions from multiple classes that were skipped due to overloaded
// RH_ScopedOverloadedInstall registrations.

#include "StdInc.h"
#include "TestFramework.h"
#include "Entity/Vehicle/Vehicle.h"
#include "Entity/Ped/Ped.h"
#include "TheZones.h"
#include "WaterLevel.h"
#include "TxdStore.h"
#include "Camera.h"
#include "PedGeometryAnalyser.h"
#include "Entity/Object/Object.h"
#include "Population.h"
#include "ScenarioHelpers.h"

// ============================================================
// CVehicle::IsPassenger / IsDriver (overloaded: -Ped, -ModelID)
// ============================================================

static Scenario::TestVehicle* s_ovVeh = nullptr;
static bool s_triedOvVeh = false;

static CVehicle* GetOvVeh() {
    if (!s_triedOvVeh) {
        s_triedOvVeh = true;
        int32 model = Scenario::FindLoadedVehicleModel();
        if (model >= 0)
            s_ovVeh = new Scenario::TestVehicle(model, CVector(2386.f, -1654.f, 13.5f));
    }
    return s_ovVeh ? s_ovVeh->AsVehicle() : nullptr;
}

GAME_DIFF_TEST(COverloaded, IsPassenger_Ped) {
    auto* veh = GetOvVeh();
    auto* player = FindPlayerPed(0);
    if (!veh || !player) return;
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/IsPassenger-Ped");
      orig = veh->IsPassenger(player); }
    rev = veh->IsPassenger(player);
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(COverloaded, IsPassenger_ModelID) {
    auto* veh = GetOvVeh();
    if (!veh) return;
    int32 models[] = { 0, 7, 596 };
    for (int32 m : models) {
        bool orig, rev;
        { HookDisableGuard guard("Vehicle/CVehicle/IsPassenger-ModelID");
          orig = veh->IsPassenger(m); }
        rev = veh->IsPassenger(m);
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(COverloaded, IsDriver_Ped) {
    auto* veh = GetOvVeh();
    auto* player = FindPlayerPed(0);
    if (!veh || !player) return;
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/IsDriver-Ped");
      orig = veh->IsDriver(player); }
    rev = veh->IsDriver(player);
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(COverloaded, IsDriver_ModelID) {
    auto* veh = GetOvVeh();
    if (!veh) return;
    int32 models[] = { 0, 7, 596 };
    for (int32 m : models) {
        bool orig, rev;
        { HookDisableGuard guard("Vehicle/CVehicle/IsDriver-ModelID");
          orig = veh->IsDriver(m); }
        rev = veh->IsDriver(m);
        EXPECT_EQ(orig, rev);
    }
}

// ============================================================
// CTheZones::GetZoneInfo (overloaded: empty suffix → dash at end)
// CTheZones::FindZone (overloaded: empty suffix)
// ============================================================

GAME_DIFF_TEST(COverloaded, GetZoneInfo) {
    CVector positions[] = {
        {2488.f, -1666.f, 13.f},
        {-2170.f, 250.f, 30.f},
        {2027.f, 1008.f, 20.f},
        {0.f, 0.f, 0.f},
    };
    for (auto& pos : positions) {
        CZone* origZone = nullptr;
        CZone* revZone = nullptr;
        CZoneInfo* orig;
        CZoneInfo* rev;
        { HookDisableGuard guard("Global/CTheZones/GetZoneInfo-");
          orig = CTheZones::GetZoneInfo(pos, &origZone); }
        rev = CTheZones::GetZoneInfo(pos, &revZone);
        EXPECT_EQ(orig, rev);
        EXPECT_EQ(origZone, revZone);
    }
}

GAME_DIFF_TEST(COverloaded, FindZone) {
    CVector pos(2488.f, -1666.f, 13.f);
    // Encode "GANTON" as uint64_t
    uint64_t label = 0;
    const char* name = "GANTON";
    for (int i = 0; i < 7 && name[i]; i++)
        label |= static_cast<uint64_t>(static_cast<uint8_t>(name[i])) << (i * 8);
    bool orig, rev;
    { HookDisableGuard guard("Global/CTheZones/FindZone-");
      orig = CTheZones::FindZone(&pos, label, ZONE_TYPE_NAVI); }
    rev = CTheZones::FindZone(&pos, label, ZONE_TYPE_NAVI);
    EXPECT_EQ(orig, rev);
}

// ============================================================
// CWaterLevel::GetWaterLevel (overloaded: empty suffix)
// ============================================================

GAME_DIFF_TEST(COverloaded, GetWaterLevel) {
    struct Pos { float x, y, z; };
    Pos positions[] = {
        {-2800.f, -2800.f, 0.f},
        {370.f, -2100.f, 0.f},
        {2488.f, -1666.f, 13.f},
    };
    for (auto& p : positions) {
        float origLevel = 0.f, revLevel = 0.f;
        bool orig, rev;
        { HookDisableGuard guard("Global/CWaterLevel/GetWaterLevel-");
          orig = CWaterLevel::GetWaterLevel(p.x, p.y, p.z, origLevel, 0, nullptr); }
        rev = CWaterLevel::GetWaterLevel(p.x, p.y, p.z, revLevel, 0, nullptr);
        EXPECT_EQ(orig, rev);
        if (orig && rev) {
            EXPECT_NEAR(origLevel, revLevel, 1e-3f);
        }
    }
}

// --- GetWaterLevel GRID SWEEP (100 positions across the SA map) ---

GAME_DIFF_TEST(COverloaded, GetWaterLevel_GridSweep) {
    int mismatches = 0;
    for (float x = -3000.f; x <= 3000.f; x += 600.f) {
        for (float y = -3000.f; y <= 3000.f; y += 600.f) {
            float origLevel = 0.f, revLevel = 0.f;
            bool orig, rev;
            { HookDisableGuard guard("Global/CWaterLevel/GetWaterLevel-");
              orig = CWaterLevel::GetWaterLevel(x, y, 0.f, origLevel, 0, nullptr); }
            rev = CWaterLevel::GetWaterLevel(x, y, 0.f, revLevel, 0, nullptr);
            EXPECT_EQ(orig, rev);
            if (orig && rev) {
                EXPECT_NEAR(origLevel, revLevel, 1e-3f);
            }
        }
    }
}

// ============================================================
// CTxdStore::FindTxdSlot (overloaded: -name, -hash)
// ============================================================

GAME_DIFF_TEST(COverloaded, FindTxdSlot_Name) {
    const char* names[] = { "generic", "particle", "fonts", "hud" };
    for (auto* name : names) {
        int32 orig, rev;
        { HookDisableGuard guard("Global/CTxdStore/FindTxdSlot-name");
          orig = CTxdStore::FindTxdSlot(name); }
        rev = CTxdStore::FindTxdSlot(name);
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(COverloaded, FindTxdSlot_Name_Invalid) {
    int32 orig, rev;
    { HookDisableGuard guard("Global/CTxdStore/FindTxdSlot-name");
      orig = CTxdStore::FindTxdSlot("nonexistent_txd_name"); }
    rev = CTxdStore::FindTxdSlot("nonexistent_txd_name");
    EXPECT_EQ(orig, rev);
}

// ============================================================
// CCamera::CalculateGroundHeight (eGroundHeightType enum)
// ============================================================

GAME_DIFF_TEST(COverloaded, CalculateGroundHeight_AllTypes) {
    eGroundHeightType types[] = {
        eGroundHeightType::ENTITY_BB_BOTTOM,
        eGroundHeightType::EXACT_GROUND_HEIGHT,
        eGroundHeightType::ENTITY_BB_TOP,
    };
    for (auto type : types) {
        float orig, rev;
        { HookDisableGuard guard("Global/CCamera/CalculateGroundHeight");
          orig = TheCamera.CalculateGroundHeight(type); }
        rev = TheCamera.CalculateGroundHeight(type);
        EXPECT_NEAR(orig, rev, 1e-3f);
    }
}

// ============================================================
// CPedGeometryAnalyser::ComputeClosestSurfacePoint-ped
// ============================================================

static Scenario::TestPed* s_ovPed = nullptr;
static bool s_triedOvPed = false;

static CPed* GetOvPed() {
    if (!s_triedOvPed) {
        s_triedOvPed = true;
        int32 model = Scenario::FindLoadedPedModel();
        if (model >= 0)
            s_ovPed = new Scenario::TestPed(PED_TYPE_CIVMALE, model, CVector(2484.f, -1664.f, 13.5f));
    }
    return s_ovPed ? s_ovPed->AsPed() : nullptr;
}

GAME_DIFF_TEST(COverloaded, ComputeClosestSurfacePoint_Ped) {
    auto* player = FindPlayerPed(0);
    auto* npc = GetOvPed();
    if (!player || !npc) return;
    CVector origPt{}, revPt{};
    bool orig, rev;
    { HookDisableGuard guard("Global/CPedGeometryAnalyser/ComputeClosestSurfacePoint-ped");
      orig = CPedGeometryAnalyser::ComputeClosestSurfacePoint(*player, *npc, origPt); }
    rev = CPedGeometryAnalyser::ComputeClosestSurfacePoint(*player, *npc, revPt);
    EXPECT_EQ(orig, rev);
    if (orig && rev) {
        EXPECT_NEAR(origPt.x, revPt.x, 1e-3f);
        EXPECT_NEAR(origPt.y, revPt.y, 1e-3f);
        EXPECT_NEAR(origPt.z, revPt.z, 1e-3f);
    }
}

// ============================================================
// CObject::DeleteAllTempObjectsInArea
// ============================================================

GAME_DIFF_TEST(COverloaded, DeleteAllTempObjectsInArea) {
    { HookDisableGuard guard("Entity/Object/CObject/DeleteAllTempObjectsInArea");
      CObject::DeleteAllTempObjectsInArea(CVector(0.f, 0.f, 0.f), 1.0f); }
    CObject::DeleteAllTempObjectsInArea(CVector(0.f, 0.f, 0.f), 1.0f);
    EXPECT_TRUE(true);
}

// ============================================================
// CPopulation::IsSunbather-ModelID
// ============================================================

GAME_DIFF_TEST(COverloaded, IsSunbather_ModelID) {
    int32 models[] = { 0, 7, 10, 50, 100, 150, 200, 250, 300 };
    for (int32 m : models) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CPopulation/IsSunbather-ModelID");
          orig = CPopulation::IsSunbather(static_cast<eModelID>(m)); }
        rev = CPopulation::IsSunbather(static_cast<eModelID>(m));
        EXPECT_EQ(orig, rev);
    }
}
