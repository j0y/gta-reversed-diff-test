// test_CCarAI.cpp — Differential tests for CCarAI static functions.
// Hook paths: AI/CCarAI/<fn>
// Police mission/speed lookups depend on wanted level (0 at state 9).
// Switch distances depend on vehicle state.

#include "StdInc.h"
#include "TestFramework.h"
#include "ScenarioHelpers.h"
#include "CarAI.h"

// --- Police mission lookups (no args, depend on global wanted level) ---

GAME_DIFF_TEST(CCarAI, FindPoliceCarMissionForWantedLevel) {
    int orig, rev;
    { HookDisableGuard guard("AI/CCarAI/FindPoliceCarMissionForWantedLevel");
      orig = (int)CCarAI::FindPoliceCarMissionForWantedLevel(); }
    rev = (int)CCarAI::FindPoliceCarMissionForWantedLevel();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CCarAI, FindPoliceBikeMissionForWantedLevel) {
    int orig, rev;
    { HookDisableGuard guard("AI/CCarAI/FindPoliceBikeMissionForWantedLevel");
      orig = (int)CCarAI::FindPoliceBikeMissionForWantedLevel(); }
    rev = (int)CCarAI::FindPoliceBikeMissionForWantedLevel();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CCarAI, FindPoliceBoatMissionForWantedLevel) {
    int orig, rev;
    { HookDisableGuard guard("AI/CCarAI/FindPoliceBoatMissionForWantedLevel");
      orig = (int)CCarAI::FindPoliceBoatMissionForWantedLevel(); }
    rev = (int)CCarAI::FindPoliceBoatMissionForWantedLevel();
    EXPECT_EQ(orig, rev);
}

// --- Vehicle-dependent queries ---

static Scenario::TestVehicle* s_aiVeh = nullptr;
static bool s_triedAiVeh = false;

static CVehicle* GetAIVehicle() {
    if (!s_triedAiVeh) {
        s_triedAiVeh = true;
        int32 model = Scenario::FindLoadedVehicleModel();
        if (model >= 0) {
            s_aiVeh = new Scenario::TestVehicle(model, CVector(2510.0f, -1665.0f, 13.5f));
        }
    }
    return s_aiVeh ? s_aiVeh->AsVehicle() : nullptr;
}

GAME_DIFF_TEST(CCarAI, FindPoliceCarSpeedForWantedLevel) {
    auto* veh = GetAIVehicle();
    if (!veh) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No vehicle"); return; }
    int32 orig, rev;
    { HookDisableGuard guard("AI/CCarAI/FindPoliceCarSpeedForWantedLevel");
      orig = CCarAI::FindPoliceCarSpeedForWantedLevel(veh); }
    rev = CCarAI::FindPoliceCarSpeedForWantedLevel(veh);
    if (orig != rev) {
        char msg[128];
        snprintf(msg, sizeof(msg), "orig=%d rev=%d", orig, rev);
        GetTestContext().RecordFailure(__FILE__, __LINE__, msg);
        return;
    }
}

GAME_DIFF_TEST(CCarAI, FindSwitchDistanceClose) {
    auto* veh = GetAIVehicle();
    if (!veh) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No vehicle"); return; }
    float orig, rev;
    { HookDisableGuard guard("AI/CCarAI/FindSwitchDistanceClose");
      orig = CCarAI::FindSwitchDistanceClose(veh); }
    rev = CCarAI::FindSwitchDistanceClose(veh);
    EXPECT_NEAR(orig, rev, 1e-3f);
}

GAME_DIFF_TEST(CCarAI, FindSwitchDistanceFar) {
    auto* veh = GetAIVehicle();
    if (!veh) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No vehicle"); return; }
    float orig, rev;
    { HookDisableGuard guard("AI/CCarAI/FindSwitchDistanceFar");
      orig = CCarAI::FindSwitchDistanceFar(veh); }
    rev = CCarAI::FindSwitchDistanceFar(veh);
    EXPECT_NEAR(orig, rev, 1e-3f);
}
