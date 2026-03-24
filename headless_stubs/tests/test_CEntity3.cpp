// test_CEntity3.cpp — Extended differential tests for CEntity.
// Hook paths: Entity/CEntity/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "Entity/Entity.h"
#include "Entity/Ped/Ped.h"
#include "Entity/Vehicle/Vehicle.h"
#include "ScenarioHelpers.h"

// --- GetDistanceFromCentreOfMassToBaseOfModel ---

GAME_DIFF_TEST(CEntity3, GetDistToBase_Player) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    float orig, rev;
    { HookDisableGuard guard("Entity/CEntity/GetDistanceFromCentreOfMassToBaseOfModel");
      orig = player->GetDistanceFromCentreOfMassToBaseOfModel(); }
    rev = player->GetDistanceFromCentreOfMassToBaseOfModel();
    EXPECT_NEAR(orig, rev, 1e-3f);
}

// --- IsVisible ---

GAME_DIFF_TEST(CEntity3, IsVisible_Player) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    bool orig, rev;
    { HookDisableGuard guard("Entity/CEntity/IsVisible");
      orig = player->IsVisible(); }
    rev = player->IsVisible();
    EXPECT_EQ(orig, rev);
}

// --- GetIsStatic ---

GAME_DIFF_TEST(CEntity3, GetIsStatic_Player) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    bool orig, rev;
    { HookDisableGuard guard("Entity/CEntity/GetIsStatic");
      orig = player->GetIsStatic(); }
    rev = player->GetIsStatic();
    EXPECT_EQ(orig, rev);
}

// --- GetColModel ---

GAME_DIFF_TEST(CEntity3, GetColModel_Player) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    CColModel* orig;
    CColModel* rev;
    { HookDisableGuard guard("Entity/CEntity/GetColModel");
      orig = player->GetColModel(); }
    rev = player->GetColModel();
    EXPECT_EQ(orig, rev);
}

// --- HasPreRenderEffects ---

GAME_DIFF_TEST(CEntity3, HasPreRenderEffects_Player) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    bool orig, rev;
    { HookDisableGuard guard("Entity/CEntity/HasPreRenderEffects");
      orig = player->HasPreRenderEffects(); }
    rev = player->HasPreRenderEffects();
    EXPECT_EQ(orig, rev);
}

// --- Spawned vehicle ---

static Scenario::TestVehicle* s_e3Veh = nullptr;
static bool s_triedE3Veh = false;

static CVehicle* GetE3Veh() {
    if (!s_triedE3Veh) {
        s_triedE3Veh = true;
        int32 model = Scenario::FindLoadedVehicleModel();
        if (model >= 0)
            s_e3Veh = new Scenario::TestVehicle(model, CVector(2390.f, -1658.f, 13.5f));
    }
    return s_e3Veh ? s_e3Veh->AsVehicle() : nullptr;
}

GAME_DIFF_TEST(CEntity3, GetDistToBase_Vehicle) {
    auto* veh = GetE3Veh();
    if (!veh) return;
    float orig, rev;
    { HookDisableGuard guard("Entity/CEntity/GetDistanceFromCentreOfMassToBaseOfModel");
      orig = veh->GetDistanceFromCentreOfMassToBaseOfModel(); }
    rev = veh->GetDistanceFromCentreOfMassToBaseOfModel();
    EXPECT_NEAR(orig, rev, 1e-3f);
}

GAME_DIFF_TEST(CEntity3, IsVisible_Vehicle) {
    auto* veh = GetE3Veh();
    if (!veh) return;
    bool orig, rev;
    { HookDisableGuard guard("Entity/CEntity/IsVisible");
      orig = veh->IsVisible(); }
    rev = veh->IsVisible();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CEntity3, GetIsStatic_Vehicle) {
    auto* veh = GetE3Veh();
    if (!veh) return;
    bool orig, rev;
    { HookDisableGuard guard("Entity/CEntity/GetIsStatic");
      orig = veh->GetIsStatic(); }
    rev = veh->GetIsStatic();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CEntity3, GetColModel_Vehicle) {
    auto* veh = GetE3Veh();
    if (!veh) return;
    CColModel* orig;
    CColModel* rev;
    { HookDisableGuard guard("Entity/CEntity/GetColModel");
      orig = veh->GetColModel(); }
    rev = veh->GetColModel();
    EXPECT_EQ(orig, rev);
}
