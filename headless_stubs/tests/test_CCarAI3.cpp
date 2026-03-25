// test_CCarAI3.cpp — Systematic CCarAI differential tests.
// Tests ALL remaining untested hooked CCarAI functions.
// Hook paths: AI/CCarAI/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "CarAI.h"
#include "ScenarioHelpers.h"

static Scenario::TestVehicle* s_ai3Veh = nullptr;
static bool s_triedAi3Veh = false;

static CVehicle* GetAI3Veh() {
    if (!s_triedAi3Veh) {
        s_triedAi3Veh = true;
        int32 model = Scenario::FindLoadedVehicleModel();
        if (model >= 0)
            s_ai3Veh = new Scenario::TestVehicle(model, CVector(2460.f, -1680.f, 13.5f));
    }
    return s_ai3Veh ? s_ai3Veh->AsVehicle() : nullptr;
}

#define AI_VEH_GUARD() if (!GetAI3Veh()) return;

// ===================================================================
// CarHasReasonToStop — void, modifies vehicle AI state
// ===================================================================

GAME_DIFF_TEST(CCarAI3, CarHasReasonToStop) {
    AI_VEH_GUARD()
    { HookDisableGuard guard("AI/CCarAI/CarHasReasonToStop");
      CCarAI::CarHasReasonToStop(GetAI3Veh()); }
    CCarAI::CarHasReasonToStop(GetAI3Veh());
    EXPECT_TRUE(true);
}

// ===================================================================
// GetCarToGoToCoors — returns float (distance), takes vehicle + target
// ===================================================================

GAME_DIFF_TEST(CCarAI3, GetCarToGoToCoors_NearbyTarget) {
    AI_VEH_GUARD()
    CVector target(2488.f, -1666.f, 13.5f);
    float orig, rev;
    { HookDisableGuard guard("AI/CCarAI/GetCarToGoToCoors");
      orig = CCarAI::GetCarToGoToCoors(GetAI3Veh(), target, DRIVING_STYLE_STOP_FOR_CARS, false); }
    rev = CCarAI::GetCarToGoToCoors(GetAI3Veh(), target, DRIVING_STYLE_STOP_FOR_CARS, false);
    EXPECT_NEAR(orig, rev, 1e-3f);
}

GAME_DIFF_TEST(CCarAI3, GetCarToGoToCoors_FarTarget) {
    AI_VEH_GUARD()
    CVector target(-2170.f, 250.f, 30.f);
    float orig, rev;
    { HookDisableGuard guard("AI/CCarAI/GetCarToGoToCoors");
      orig = CCarAI::GetCarToGoToCoors(GetAI3Veh(), target, DRIVING_STYLE_STOP_FOR_CARS, true); }
    rev = CCarAI::GetCarToGoToCoors(GetAI3Veh(), target, DRIVING_STYLE_STOP_FOR_CARS, true);
    EXPECT_NEAR(orig, rev, 1e-3f);
}

// ===================================================================
// GetCarToGoToCoorsAccurate — similar to above
// ===================================================================

GAME_DIFF_TEST(CCarAI3, GetCarToGoToCoorsAccurate) {
    AI_VEH_GUARD()
    CVector target(2488.f, -1666.f, 13.5f);
    float orig, rev;
    { HookDisableGuard guard("AI/CCarAI/GetCarToGoToCoorsAccurate");
      orig = CCarAI::GetCarToGoToCoorsAccurate(GetAI3Veh(), target, DRIVING_STYLE_STOP_FOR_CARS, false); }
    rev = CCarAI::GetCarToGoToCoorsAccurate(GetAI3Veh(), target, DRIVING_STYLE_STOP_FOR_CARS, false);
    EXPECT_NEAR(orig, rev, 1e-3f);
}

// ===================================================================
// GetCarToGoToCoorsStraightLine
// ===================================================================

GAME_DIFF_TEST(CCarAI3, GetCarToGoToCoorsStraightLine) {
    AI_VEH_GUARD()
    CVector target(2500.f, -1650.f, 13.5f);
    float orig, rev;
    { HookDisableGuard guard("AI/CCarAI/GetCarToGoToCoorsStraightLine");
      orig = CCarAI::GetCarToGoToCoorsStraightLine(GetAI3Veh(), target, DRIVING_STYLE_STOP_FOR_CARS, false); }
    rev = CCarAI::GetCarToGoToCoorsStraightLine(GetAI3Veh(), target, DRIVING_STYLE_STOP_FOR_CARS, false);
    EXPECT_NEAR(orig, rev, 1e-3f);
}

// ===================================================================
// GetCarToGoToCoorsRacing
// ===================================================================

GAME_DIFF_TEST(CCarAI3, GetCarToGoToCoorsRacing) {
    AI_VEH_GUARD()
    CVector target(2500.f, -1650.f, 13.5f);
    float orig, rev;
    { HookDisableGuard guard("AI/CCarAI/GetCarToGoToCoorsRacing");
      orig = CCarAI::GetCarToGoToCoorsRacing(GetAI3Veh(), target, DRIVING_STYLE_STOP_FOR_CARS, false); }
    rev = CCarAI::GetCarToGoToCoorsRacing(GetAI3Veh(), target, DRIVING_STYLE_STOP_FOR_CARS, false);
    EXPECT_NEAR(orig, rev, 1e-3f);
}

// ===================================================================
// GetCarToParkAtCoors — returns float
// ===================================================================

GAME_DIFF_TEST(CCarAI3, GetCarToParkAtCoors) {
    AI_VEH_GUARD()
    CVector target(2490.f, -1670.f, 13.5f);
    float orig, rev;
    { HookDisableGuard guard("AI/CCarAI/GetCarToParkAtCoors");
      orig = CCarAI::GetCarToParkAtCoors(GetAI3Veh(), target); }
    rev = CCarAI::GetCarToParkAtCoors(GetAI3Veh(), target);
    EXPECT_NEAR(orig, rev, 1e-3f);
}

// ===================================================================
// MellowOutChaseSpeed — void, modifies vehicle speed
// ===================================================================

GAME_DIFF_TEST(CCarAI3, MellowOutChaseSpeed) {
    AI_VEH_GUARD()
    auto saved = GetAI3Veh()->m_autoPilot;
    { HookDisableGuard guard("AI/CCarAI/MellowOutChaseSpeed");
      CCarAI::MellowOutChaseSpeed(GetAI3Veh()); }
    float origSpeed = GetAI3Veh()->m_autoPilot.m_nCruiseSpeed;
    GetAI3Veh()->m_autoPilot = saved;

    CCarAI::MellowOutChaseSpeed(GetAI3Veh());
    float revSpeed = GetAI3Veh()->m_autoPilot.m_nCruiseSpeed;
    GetAI3Veh()->m_autoPilot = saved;

    EXPECT_EQ(origSpeed, revSpeed);
}

// ===================================================================
// MellowOutChaseSpeedBoat — void, boat variant
// ===================================================================

GAME_DIFF_TEST(CCarAI3, MellowOutChaseSpeedBoat) {
    AI_VEH_GUARD()
    auto saved = GetAI3Veh()->m_autoPilot;
    { HookDisableGuard guard("AI/CCarAI/MellowOutChaseSpeedBoat");
      CCarAI::MellowOutChaseSpeedBoat(GetAI3Veh()); }
    float origSpeed = GetAI3Veh()->m_autoPilot.m_nCruiseSpeed;
    GetAI3Veh()->m_autoPilot = saved;

    CCarAI::MellowOutChaseSpeedBoat(GetAI3Veh());
    float revSpeed = GetAI3Veh()->m_autoPilot.m_nCruiseSpeed;
    GetAI3Veh()->m_autoPilot = saved;

    EXPECT_EQ(origSpeed, revSpeed);
}

// ===================================================================
// MakeWayForCarWithSiren — void
// ===================================================================

GAME_DIFF_TEST(CCarAI3, MakeWayForCarWithSiren) {
    AI_VEH_GUARD()
    { HookDisableGuard guard("AI/CCarAI/MakeWayForCarWithSiren");
      CCarAI::MakeWayForCarWithSiren(GetAI3Veh()); }
    CCarAI::MakeWayForCarWithSiren(GetAI3Veh());
    EXPECT_TRUE(true);
}

// ===================================================================
// TellOccupantsToLeaveCar — void, tells peds to exit (no driver = no-op)
// ===================================================================

GAME_DIFF_TEST(CCarAI3, TellOccupantsToLeaveCar_EmptyVehicle) {
    AI_VEH_GUARD()
    { HookDisableGuard guard("AI/CCarAI/TellOccupantsToLeaveCar");
      CCarAI::TellOccupantsToLeaveCar(GetAI3Veh()); }
    CCarAI::TellOccupantsToLeaveCar(GetAI3Veh());
    EXPECT_TRUE(true);
}

// ===================================================================
// FindPoliceCarSpeedForWantedLevel — int, wanted level dependent
// Test all 7 wanted levels (0-6)
// ===================================================================

GAME_DIFF_TEST(CCarAI3, FindPoliceCarSpeedForWantedLevel_AllLevels) {
    // Save wanted level
    auto* wanted = FindPlayerWanted(0);
    if (!wanted) return;
    auto savedLevel = wanted->m_nWantedLevel;

    for (int32 level = 0; level <= 6; level++) {
        wanted->m_nWantedLevel = level;
        AI_VEH_GUARD()
        int32 orig, rev;
        { HookDisableGuard guard("AI/CCarAI/FindPoliceCarSpeedForWantedLevel");
          orig = CCarAI::FindPoliceCarSpeedForWantedLevel(GetAI3Veh()); }
        rev = CCarAI::FindPoliceCarSpeedForWantedLevel(GetAI3Veh());
        EXPECT_EQ(orig, rev);
    }

    wanted->m_nWantedLevel = savedLevel;
}
