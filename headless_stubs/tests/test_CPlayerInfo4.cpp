// test_CPlayerInfo4.cpp — Differential tests for CPlayerInfo and CWeapon globals.
// Hook paths: Global/CPlayerInfo/<fn>, Global/CWeapon/<fn>
//
// Previously tested (CPlayerInfo): IsRestartingAfterMissionFailed,
//   IsRestartingAfterArrest, IsRestartingAfterDeath, IsPlayerInRemoteMode,
//   AddHealth, StreamParachuteWeapon
//
// New targets:
//   CPlayerInfo::GetPos_Hook         — returns player/vehicle position
//   CPlayerInfo::GetSpeed_Hook       — returns player/vehicle velocity
//   CPlayerInfo::EvaluateCarPosition — scoring function: angle+distance to car
//   CPlayerInfo::SetLastTargetVehicle — safe reference setter
//   CWeapon::EvaluateTargetForHeatSeekingMissile — pure scoring function
//   CWeapon::ProcessLineOfSight      — line of sight query

#include "StdInc.h"
#include "TestFramework.h"
#include "PlayerInfo.h"
#include "Weapon.h"
#include "ScenarioHelpers.h"

// ── GetPos ──
// Returns vehicle position if in vehicle, else ped position.
// The hook is GetPos_Hook (sret wrapper) but we can call GetPos() directly.

GAME_DIFF_TEST(CPlayerInfo4, Diff_GetPos) {
    auto* info = &FindPlayerInfo(0);
    if (!info->m_pPed) return;

    CVector orig, rev;
    { HookDisableGuard guard("Global/CPlayerInfo/GetPos_Hook");
      orig = info->GetPos(); }
    rev = info->GetPos();
    EXPECT_NEAR(orig.x, rev.x, 1e-4f);
    EXPECT_NEAR(orig.y, rev.y, 1e-4f);
    EXPECT_NEAR(orig.z, rev.z, 1e-4f);
}

// ── GetSpeed ──
// Returns vehicle move speed if in vehicle, else ped move speed.

GAME_DIFF_TEST(CPlayerInfo4, Diff_GetSpeed) {
    auto* info = &FindPlayerInfo(0);
    if (!info->m_pPed) return;

    CVector orig, rev;
    { HookDisableGuard guard("Global/CPlayerInfo/GetSpeed_Hook");
      orig = info->GetSpeed(); }
    rev = info->GetSpeed();
    EXPECT_NEAR(orig.x, rev.x, 1e-4f);
    EXPECT_NEAR(orig.y, rev.y, 1e-4f);
    EXPECT_NEAR(orig.z, rev.z, 1e-4f);
}

// ── EvaluateCarPosition ──
// Pure scoring: calculates how well a car is positioned relative to a ped
// based on angle from forward direction and distance.
// Takes: car entity, ped, distance, output score, output vehicle.

GAME_DIFF_TEST(CPlayerInfo4, Diff_EvaluateCarPosition) {
    int32 model = Scenario::FindLoadedVehicleModel();
    if (model < 0) return;
    Scenario::TestVehicle veh(model, CVector(2490.f, -1665.f, 13.5f));
    auto* player = FindPlayerPed(0);
    if (!player) return;

    float pedToVehDist = (player->GetPosition() - veh.AsVehicle()->GetPosition()).Magnitude();

    auto* info = &FindPlayerInfo(0);

    float origDist = -1.0f;
    CVehicle* origVeh = nullptr;
    { HookDisableGuard guard("Global/CPlayerInfo/EvaluateCarPosition");
      info->EvaluateCarPosition(veh.AsVehicle(), player, pedToVehDist, &origDist, &origVeh); }

    float revDist = -1.0f;
    CVehicle* revVeh = nullptr;
    info->EvaluateCarPosition(veh.AsVehicle(), player, pedToVehDist, &revDist, &revVeh);

    EXPECT_NEAR(origDist, revDist, 1e-3f);
    EXPECT_EQ(origVeh, revVeh);
}

GAME_DIFF_TEST(CPlayerInfo4, Diff_EvaluateCarPosition_MultipleDistances) {
    int32 model = Scenario::FindLoadedVehicleModel();
    if (model < 0) return;
    auto* player = FindPlayerPed(0);
    if (!player) return;
    auto* info = &FindPlayerInfo(0);

    // Test with vehicles at different distances
    CVector positions[] = {
        player->GetPosition() + CVector(5.0f, 0.0f, 0.0f),
        player->GetPosition() + CVector(0.0f, 8.0f, 0.0f),
        player->GetPosition() + CVector(-3.0f, -3.0f, 0.0f),
    };

    for (auto& pos : positions) {
        Scenario::TestVehicle veh(model, pos);
        float dist = (player->GetPosition() - pos).Magnitude();

        float origScore = -1.0f;
        CVehicle* origVeh = nullptr;
        { HookDisableGuard guard("Global/CPlayerInfo/EvaluateCarPosition");
          info->EvaluateCarPosition(veh.AsVehicle(), player, dist, &origScore, &origVeh); }

        float revScore = -1.0f;
        CVehicle* revVeh = nullptr;
        info->EvaluateCarPosition(veh.AsVehicle(), player, dist, &revScore, &revVeh);

        EXPECT_NEAR(origScore, revScore, 1e-3f);
        EXPECT_EQ(origVeh, revVeh);
    }
}

// ── SetLastTargetVehicle ──
// Safe setter using SafeCleanUpRef/SafeRegisterRef.
// Test roundtrip: set vehicle, verify, set nullptr, verify.

GAME_DIFF_TEST(CPlayerInfo4, Diff_SetLastTargetVehicle) {
    int32 model = Scenario::FindLoadedVehicleModel();
    if (model < 0) return;
    Scenario::TestVehicle veh(model, CVector(2525.f, -1640.f, 13.5f));
    auto* info = &FindPlayerInfo(0);

    // Save original
    CVehicle* savedTarget = info->m_pLastTargetVehicle;

    { HookDisableGuard guard("Global/CPlayerInfo/SetLastTargetVehicle");
      info->SetLastTargetVehicle(veh.AsVehicle()); }
    CVehicle* origAfterSet = info->m_pLastTargetVehicle;

    // Restore and try reversed
    info->m_pLastTargetVehicle = savedTarget;
    info->SetLastTargetVehicle(veh.AsVehicle());
    CVehicle* revAfterSet = info->m_pLastTargetVehicle;

    EXPECT_EQ(origAfterSet, revAfterSet);

    // Clean up: set back to nullptr
    info->SetLastTargetVehicle(nullptr);

    { HookDisableGuard guard("Global/CPlayerInfo/SetLastTargetVehicle");
      info->SetLastTargetVehicle(nullptr); }
    CVehicle* origAfterClear = info->m_pLastTargetVehicle;

    info->SetLastTargetVehicle(nullptr);
    CVehicle* revAfterClear = info->m_pLastTargetVehicle;

    EXPECT_EQ(origAfterClear, revAfterClear);

    // Restore
    info->m_pLastTargetVehicle = savedTarget;
}

// ── EvaluateTargetForHeatSeekingMissile ──
// Global function: pure scoring. Takes entity, origin, direction, tolerance.
// Returns -1 if outside tolerance, else distance-based score.

GAME_DIFF_TEST(CPlayerInfo4, Diff_EvaluateTargetForHeatSeeking) {
    int32 model = Scenario::FindLoadedVehicleModel();
    if (model < 0) return;
    Scenario::TestVehicle veh(model, CVector(2500.f, -1665.f, 13.5f));
    auto* player = FindPlayerPed(0);
    if (!player) return;

    CVector origin = player->GetPosition();
    CVector aimDir = (veh.AsVehicle()->GetPosition() - origin);
    aimDir.Normalise();

    float orig, rev;
    { HookDisableGuard guard("Global/CWeapon/EvaluateTargetForHeatSeekingMissile");
      orig = CWeapon::EvaluateTargetForHeatSeekingMissile(veh.AsVehicle(), origin, aimDir, 5.0f, false, nullptr); }
    rev = CWeapon::EvaluateTargetForHeatSeekingMissile(veh.AsVehicle(), origin, aimDir, 5.0f, false, nullptr);
    EXPECT_NEAR(orig, rev, 1e-3f);
}

GAME_DIFF_TEST(CPlayerInfo4, Diff_EvaluateTargetForHeatSeeking_OutOfTolerance) {
    int32 model = Scenario::FindLoadedVehicleModel();
    if (model < 0) return;
    // Place vehicle far to the side, outside tolerance
    Scenario::TestVehicle veh(model, CVector(2600.f, -1500.f, 13.5f));
    auto* player = FindPlayerPed(0);
    if (!player) return;

    CVector origin = player->GetPosition();
    CVector aimDir(1.0f, 0.0f, 0.0f); // aiming east, vehicle is northeast

    float orig, rev;
    { HookDisableGuard guard("Global/CWeapon/EvaluateTargetForHeatSeekingMissile");
      orig = CWeapon::EvaluateTargetForHeatSeekingMissile(veh.AsVehicle(), origin, aimDir, 0.5f, false, nullptr); }
    rev = CWeapon::EvaluateTargetForHeatSeekingMissile(veh.AsVehicle(), origin, aimDir, 0.5f, false, nullptr);
    EXPECT_NEAR(orig, rev, 1e-3f);
}

GAME_DIFF_TEST(CPlayerInfo4, Diff_EvaluateTargetForHeatSeeking_PlanePriority) {
    int32 model = Scenario::FindLoadedVehicleModel();
    if (model < 0) return;
    Scenario::TestVehicle veh(model, CVector(2500.f, -1665.f, 13.5f));
    auto* player = FindPlayerPed(0);
    if (!player) return;

    CVector origin = player->GetPosition();
    CVector aimDir = (veh.AsVehicle()->GetPosition() - origin);
    aimDir.Normalise();

    float orig, rev;
    { HookDisableGuard guard("Global/CWeapon/EvaluateTargetForHeatSeekingMissile");
      orig = CWeapon::EvaluateTargetForHeatSeekingMissile(veh.AsVehicle(), origin, aimDir, 5.0f, true, nullptr); }
    rev = CWeapon::EvaluateTargetForHeatSeekingMissile(veh.AsVehicle(), origin, aimDir, 5.0f, true, nullptr);
    EXPECT_NEAR(orig, rev, 1e-3f);
}

GAME_DIFF_TEST(CPlayerInfo4, Diff_EvaluateTargetForHeatSeeking_PreferredTarget) {
    int32 model = Scenario::FindLoadedVehicleModel();
    if (model < 0) return;
    Scenario::TestVehicle veh(model, CVector(2500.f, -1665.f, 13.5f));
    auto* player = FindPlayerPed(0);
    if (!player) return;

    CVector origin = player->GetPosition();
    CVector aimDir = (veh.AsVehicle()->GetPosition() - origin);
    aimDir.Normalise();

    // Test with the vehicle as preferred target (score *= 0.25)
    float orig, rev;
    { HookDisableGuard guard("Global/CWeapon/EvaluateTargetForHeatSeekingMissile");
      orig = CWeapon::EvaluateTargetForHeatSeekingMissile(veh.AsVehicle(), origin, aimDir, 5.0f, false, veh.AsVehicle()); }
    rev = CWeapon::EvaluateTargetForHeatSeekingMissile(veh.AsVehicle(), origin, aimDir, 5.0f, false, veh.AsVehicle());
    EXPECT_NEAR(orig, rev, 1e-3f);
}
