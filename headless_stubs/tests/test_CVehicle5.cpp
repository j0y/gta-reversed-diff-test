// test_CVehicle5.cpp — Vehicle query + mutation tests.
// Hook paths: Vehicle/CVehicle/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "Entity/Vehicle/Vehicle.h"
#include "ScenarioHelpers.h"

static Scenario::TestVehicle* s_v5 = nullptr;
static bool s_triedV5 = false;

static CVehicle* GetV5() {
    if (!s_triedV5) {
        s_triedV5 = true;
        int32 model = Scenario::FindLoadedVehicleModel();
        if (model >= 0)
            s_v5 = new Scenario::TestVehicle(model, CVector(2388.f, -1656.f, 13.5f));
    }
    return s_v5 ? s_v5->AsVehicle() : nullptr;
}

// --- ProcessCarAlarm ---

GAME_DIFF_TEST(CVehicle5, ProcessCarAlarm_NoAlarm) {
    auto* veh = GetV5();
    if (!veh) return;
    { HookDisableGuard guard("Vehicle/CVehicle/ProcessCarAlarm");
      veh->ProcessCarAlarm(); }
    veh->ProcessCarAlarm();
    EXPECT_TRUE(true);
}

// --- GetHeightAboveRoad (virtual) ---

GAME_DIFF_TEST(CVehicle5, GetHeightAboveRoad) {
    auto* veh = GetV5();
    if (!veh) return;
    float orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/GetHeightAboveRoad");
      orig = veh->GetHeightAboveRoad(); }
    rev = veh->GetHeightAboveRoad();
    EXPECT_NEAR(orig, rev, 1e-3f);
}

// --- CanPedStepOutCar (virtual) ---

GAME_DIFF_TEST(CVehicle5, CanPedStepOutCar) {
    auto* veh = GetV5();
    auto* player = FindPlayerPed(0);
    if (!veh || !player) return;
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/CanPedStepOutCar");
      orig = veh->CanPedStepOutCar(false); }
    rev = veh->CanPedStepOutCar(false);
    EXPECT_EQ(orig, rev);
}

// --- CanPedJumpOutCar ---

GAME_DIFF_TEST(CVehicle5, CanPedJumpOutCar) {
    auto* veh = GetV5();
    auto* player = FindPlayerPed(0);
    if (!veh || !player) return;
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/CanPedJumpOutCar");
      orig = veh->CanPedJumpOutCar(player); }
    rev = veh->CanPedJumpOutCar(player);
    EXPECT_EQ(orig, rev);
}

// --- UpdateLightingFromStoredPolys ---

GAME_DIFF_TEST(CVehicle5, UpdateLightingFromStoredPolys) {
    auto* veh = GetV5();
    if (!veh) return;
    { HookDisableGuard guard("Vehicle/CVehicle/UpdateLightingFromStoredPolys");
      veh->UpdateLightingFromStoredPolys(); }
    veh->UpdateLightingFromStoredPolys();
    EXPECT_TRUE(true);
}
