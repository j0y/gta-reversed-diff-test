// test_CCarEnterExit2.cpp — Deep differential tests for CCarEnterExit.
// Hook paths: Global/CCarEnterExit/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "CarEnterExit.h"
#include "Entity/Ped/Ped.h"
#include "ScenarioHelpers.h"

static Scenario::TestVehicle* s_enterExitVeh = nullptr;
static bool s_triedEnterExitVeh = false;

static CVehicle* GetEnterExitVehicle() {
    if (!s_triedEnterExitVeh) {
        s_triedEnterExitVeh = true;
        int32 model = Scenario::FindLoadedVehicleModel();
        if (model >= 0)
            s_enterExitVeh = new Scenario::TestVehicle(model, CVector(2416.0f, -1668.0f, 13.5f));
    }
    return s_enterExitVeh ? s_enterExitVeh->AsVehicle() : nullptr;
}

#define EE_GUARD() if (!GetEnterExitVehicle()) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No vehicle"); return; }

// --- IsVehicleHealthy ---

GAME_DIFF_TEST(CCarEnterExit, IsVehicleHealthy2) {
    EE_GUARD()
    auto* veh = GetEnterExitVehicle();
    bool orig, rev;
    { HookDisableGuard guard("Global/CCarEnterExit/IsVehicleHealthy"); orig = CCarEnterExit::IsVehicleHealthy(veh); }
    rev = CCarEnterExit::IsVehicleHealthy(veh);
    EXPECT_EQ(orig, rev);
}

// --- IsPedHealthy ---

GAME_DIFF_TEST(CCarEnterExit, IsPedHealthy2_Player) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    bool orig, rev;
    { HookDisableGuard guard("Global/CCarEnterExit/IsPedHealthy"); orig = CCarEnterExit::IsPedHealthy(player); }
    rev = CCarEnterExit::IsPedHealthy(player);
    EXPECT_EQ(orig, rev);
}

// --- IsCarDoorInUse ---

// IsCarDoorInUse takes (CVehicle*, uint32, CPed*) — need 3 args, skip simple form

// --- IsCarQuickJackPossible ---

GAME_DIFF_TEST(CCarEnterExit, IsCarQuickJackPossible) {
    EE_GUARD()
    auto* veh = GetEnterExitVehicle();
    auto* player = FindPlayerPed(0);
    if (!player) return;
    bool orig, rev;
    { HookDisableGuard guard("Global/CCarEnterExit/IsCarQuickJackPossible");
      orig = CCarEnterExit::IsCarQuickJackPossible(veh, 0, player); }
    rev = CCarEnterExit::IsCarQuickJackPossible(veh, 0, player);
    EXPECT_EQ(orig, rev);
}

// --- IsCarSlowJackRequired ---

GAME_DIFF_TEST(CCarEnterExit, IsCarSlowJackRequired) {
    EE_GUARD()
    auto* veh = GetEnterExitVehicle();
    bool orig, rev;
    { HookDisableGuard guard("Global/CCarEnterExit/IsCarSlowJackRequired");
      orig = CCarEnterExit::IsCarSlowJackRequired(veh, 0); }
    rev = CCarEnterExit::IsCarSlowJackRequired(veh, 0);
    EXPECT_EQ(orig, rev);
}

// --- ComputeTargetDoorToExit ---

GAME_DIFF_TEST(CCarEnterExit, ComputeTargetDoorToExit2) {
    EE_GUARD()
    auto* veh = GetEnterExitVehicle();
    auto* player = FindPlayerPed(0);
    if (!player) return;
    int32 orig, rev;
    { HookDisableGuard guard("Global/CCarEnterExit/ComputeTargetDoorToExit");
      orig = CCarEnterExit::ComputeTargetDoorToExit(veh, player); }
    rev = CCarEnterExit::ComputeTargetDoorToExit(veh, player);
    EXPECT_EQ(orig, rev);
}
