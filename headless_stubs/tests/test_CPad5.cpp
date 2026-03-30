// test_CPad5.cpp — Differential tests for CPad input query functions.
// Hook paths: Global/CPad/<fn>
//
// CPad has 70+ untested hooks. These are all switch-on-Mode queries
// that read controller state (NewState/OldState). At game state 9
// headlessly, all controller inputs are zero, but the Mode-based
// branching and threshold logic can still diverge between original
// and reversed code.
//
// Tests cover vehicle input queries, on-foot queries, and targeting.

#include "StdInc.h"
#include "TestFramework.h"
#include "Pad.h"

static CPad* GetPad0() { return CPad::GetPad(0); }

// ── Vehicle input queries ──

GAME_DIFF_TEST(CPad5, Diff_GetAccelerate) {
    auto* pad = GetPad0();
    int16 orig, rev;
    { HookDisableGuard guard("Global/CPad/GetAccelerate"); orig = pad->GetAccelerate(); }
    rev = pad->GetAccelerate();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CPad5, Diff_GetAccelerateJustDown) {
    auto* pad = GetPad0();
    bool orig, rev;
    { HookDisableGuard guard("Global/CPad/GetAccelerateJustDown"); orig = pad->GetAccelerateJustDown(); }
    rev = pad->GetAccelerateJustDown();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CPad5, Diff_GetBrake) {
    auto* pad = GetPad0();
    int16 orig, rev;
    { HookDisableGuard guard("Global/CPad/GetBrake"); orig = pad->GetBrake(); }
    rev = pad->GetBrake();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CPad5, Diff_GetCarGunFired) {
    auto* pad = GetPad0();
    bool orig, rev;
    { HookDisableGuard guard("Global/CPad/GetCarGunFired"); orig = pad->GetCarGunFired(); }
    rev = pad->GetCarGunFired();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CPad5, Diff_GetCarGunLeftRight) {
    auto* pad = GetPad0();
    int16 orig, rev;
    { HookDisableGuard guard("Global/CPad/GetCarGunLeftRight"); orig = pad->GetCarGunLeftRight(); }
    rev = pad->GetCarGunLeftRight();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CPad5, Diff_GetCarGunUpDown) {
    auto* pad = GetPad0();
    int16 orig, rev;
    { HookDisableGuard guard("Global/CPad/GetCarGunUpDown"); orig = pad->GetCarGunUpDown(); }
    rev = pad->GetCarGunUpDown();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CPad5, Diff_GetHorn) {
    auto* pad = GetPad0();
    bool orig, rev;
    { HookDisableGuard guard("Global/CPad/GetHorn"); orig = pad->GetHorn(); }
    rev = pad->GetHorn();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CPad5, Diff_GetLookBehindForCar) {
    auto* pad = GetPad0();
    bool orig, rev;
    { HookDisableGuard guard("Global/CPad/GetLookBehindForCar"); orig = pad->GetLookBehindForCar(); }
    rev = pad->GetLookBehindForCar();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CPad5, Diff_GetLookLeft) {
    auto* pad = GetPad0();
    bool orig, rev;
    { HookDisableGuard guard("Global/CPad/GetLookLeft"); orig = pad->GetLookLeft(); }
    rev = pad->GetLookLeft();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CPad5, Diff_GetLookRight) {
    auto* pad = GetPad0();
    bool orig, rev;
    { HookDisableGuard guard("Global/CPad/GetLookRight"); orig = pad->GetLookRight(); }
    rev = pad->GetLookRight();
    EXPECT_EQ(orig, rev);
}

// ── On-foot input queries ──

GAME_DIFF_TEST(CPad5, Diff_GetSprint) {
    auto* pad = GetPad0();
    bool orig, rev;
    { HookDisableGuard guard("Global/CPad/GetSprint"); orig = pad->GetSprint(); }
    rev = pad->GetSprint();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CPad5, Diff_GetTarget) {
    auto* pad = GetPad0();
    bool orig, rev;
    { HookDisableGuard guard("Global/CPad/GetTarget"); orig = pad->GetTarget(); }
    rev = pad->GetTarget();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CPad5, Diff_GetWeapon) {
    auto* pad = GetPad0();
    auto* ped = FindPlayerPed(0);
    if (!ped) return;
    int32 orig, rev;
    { HookDisableGuard guard("Global/CPad/GetWeapon"); orig = pad->GetWeapon(ped); }
    rev = pad->GetWeapon(ped);
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CPad5, Diff_GetExitVehicle) {
    auto* pad = GetPad0();
    bool orig, rev;
    { HookDisableGuard guard("Global/CPad/GetExitVehicle"); orig = pad->GetExitVehicle(); }
    rev = pad->GetExitVehicle();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CPad5, Diff_GetEnterTargeting) {
    auto* pad = GetPad0();
    bool orig, rev;
    { HookDisableGuard guard("Global/CPad/GetEnterTargeting"); orig = pad->GetEnterTargeting(); }
    rev = pad->GetEnterTargeting();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CPad5, Diff_GetForceCameraBehindPlayer) {
    auto* pad = GetPad0();
    bool orig, rev;
    { HookDisableGuard guard("Global/CPad/GetForceCameraBehindPlayer"); orig = pad->GetForceCameraBehindPlayer(); }
    rev = pad->GetForceCameraBehindPlayer();
    EXPECT_EQ(orig, rev);
}

// ── Analog stick queries ──

GAME_DIFF_TEST(CPad5, Diff_GetAnaloguePadUp) {
    auto* pad = GetPad0();
    bool orig, rev;
    { HookDisableGuard guard("Global/CPad/GetAnaloguePadUp"); orig = pad->GetAnaloguePadUp(); }
    rev = pad->GetAnaloguePadUp();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CPad5, Diff_GetAnaloguePadDown) {
    auto* pad = GetPad0();
    bool orig, rev;
    { HookDisableGuard guard("Global/CPad/GetAnaloguePadDown"); orig = pad->GetAnaloguePadDown(); }
    rev = pad->GetAnaloguePadDown();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CPad5, Diff_GetAnaloguePadLeft) {
    auto* pad = GetPad0();
    bool orig, rev;
    { HookDisableGuard guard("Global/CPad/GetAnaloguePadLeft"); orig = pad->GetAnaloguePadLeft(); }
    rev = pad->GetAnaloguePadLeft();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CPad5, Diff_GetAnaloguePadRight) {
    auto* pad = GetPad0();
    bool orig, rev;
    { HookDisableGuard guard("Global/CPad/GetAnaloguePadRight"); orig = pad->GetAnaloguePadRight(); }
    rev = pad->GetAnaloguePadRight();
    EXPECT_EQ(orig, rev);
}

// ── Misc queries ──

GAME_DIFF_TEST(CPad5, Diff_GetDisplayVitalStats) {
    auto* pad = GetPad0();
    auto* ped = FindPlayerPed(0);
    if (!ped) return;
    int16 orig, rev;
    { HookDisableGuard guard("Global/CPad/GetDisplayVitalStats"); orig = pad->GetDisplayVitalStats(ped); }
    rev = pad->GetDisplayVitalStats(ped);
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CPad5, Diff_ExitVehicleJustDown) {
    auto* pad = GetPad0();
    bool orig, rev;
    { HookDisableGuard guard("Global/CPad/ExitVehicleJustDown"); orig = pad->ExitVehicleJustDown(); }
    rev = pad->ExitVehicleJustDown();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CPad5, Diff_CollectPickupJustDown) {
    auto* pad = GetPad0();
    bool orig, rev;
    { HookDisableGuard guard("Global/CPad/CollectPickupJustDown"); orig = pad->CollectPickupJustDown(); }
    rev = pad->CollectPickupJustDown();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CPad5, Diff_CycleWeaponLeftJustDown) {
    auto* pad = GetPad0();
    bool orig, rev;
    { HookDisableGuard guard("Global/CPad/CycleWeaponLeftJustDown"); orig = pad->CycleWeaponLeftJustDown(); }
    rev = pad->CycleWeaponLeftJustDown();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CPad5, Diff_CycleWeaponRightJustDown) {
    auto* pad = GetPad0();
    bool orig, rev;
    { HookDisableGuard guard("Global/CPad/CycleWeaponRightJustDown"); orig = pad->CycleWeaponRightJustDown(); }
    rev = pad->CycleWeaponRightJustDown();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CPad5, Diff_GetTouchedTimeDelta) {
    auto* pad = GetPad0();
    int32 orig, rev;
    { HookDisableGuard guard("Global/CPad/GetTouchedTimeDelta"); orig = pad->GetTouchedTimeDelta(); }
    rev = pad->GetTouchedTimeDelta();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CPad5, Diff_AimWeaponLeftRight) {
    auto* pad = GetPad0();
    auto* ped = FindPlayerPed(0);
    if (!ped) return;
    int16 orig, rev;
    { HookDisableGuard guard("Global/CPad/AimWeaponLeftRight"); orig = pad->AimWeaponLeftRight(ped); }
    rev = pad->AimWeaponLeftRight(ped);
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CPad5, Diff_AimWeaponUpDown) {
    auto* pad = GetPad0();
    auto* ped = FindPlayerPed(0);
    if (!ped) return;
    int16 orig, rev;
    { HookDisableGuard guard("Global/CPad/AimWeaponUpDown"); orig = pad->AimWeaponUpDown(ped); }
    rev = pad->AimWeaponUpDown(ped);
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CPad5, Diff_CarGunJustDown) {
    auto* pad = GetPad0();
    bool orig, rev;
    { HookDisableGuard guard("Global/CPad/CarGunJustDown"); orig = pad->CarGunJustDown(); }
    rev = pad->CarGunJustDown();
    EXPECT_EQ(orig, rev);
}
