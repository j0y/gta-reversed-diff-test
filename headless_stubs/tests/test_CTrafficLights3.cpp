// test_CTrafficLights3.cpp — Differential tests for CTrafficLights.
// Hook paths: Global/CTrafficLights/<fn>
//
// Previously tested: LightForPeds, IsMITrafficLight
//
// New targets (public functions only):
//   ShouldCarStopForBridge — vehicle pathfinding query for bridge lights
//   ShouldCarStopForLight  — vehicle traffic light query using autopilot path nodes
//
// Private functions (LightForCars1/2, LightForCars1/2_Visual,
// FindTrafficLightType, FindOrientationForTrafficLightType,
// FindTrafficLightTypeFromOrientation) can't be called directly.
// They're exercised indirectly through ShouldCarStopForLight.
//
// Skipped:
//   DisplayActualLight — rendering (coronas, shadows, shiny texts)

#include "StdInc.h"
#include "TestFramework.h"
#include "TrafficLights.h"
#include "ScenarioHelpers.h"

// ── ShouldCarStopForBridge ──
// Checks vehicle pathfinding nodes for bridge lights.
// Spawned vehicles have default (uninitialized) autopilot, so path nodes
// are likely invalid — function should return false safely.

GAME_DIFF_TEST(CTrafficLights3, Diff_ShouldCarStopForBridge) {
    int32 model = Scenario::FindLoadedVehicleModel();
    if (model < 0) return;
    Scenario::TestVehicle veh(model, CVector(2525.f, -1640.f, 13.5f));

    bool orig, rev;
    { HookDisableGuard guard("Global/CTrafficLights/ShouldCarStopForBridge");
      orig = CTrafficLights::ShouldCarStopForBridge(veh.AsVehicle()); }
    rev = CTrafficLights::ShouldCarStopForBridge(veh.AsVehicle());
    EXPECT_EQ(orig, rev);
}

// ── ShouldCarStopForLight ──
// Complex query on vehicle's autopilot path nodes and current traffic light state.
// Tests both bUnkn=false and bUnkn=true variants.

GAME_DIFF_TEST(CTrafficLights3, Diff_ShouldCarStopForLight) {
    int32 model = Scenario::FindLoadedVehicleModel();
    if (model < 0) return;
    Scenario::TestVehicle veh(model, CVector(2525.f, -1640.f, 13.5f));

    bool orig, rev;
    { HookDisableGuard guard("Global/CTrafficLights/ShouldCarStopForLight");
      orig = CTrafficLights::ShouldCarStopForLight(veh.AsVehicle(), false); }
    rev = CTrafficLights::ShouldCarStopForLight(veh.AsVehicle(), false);
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CTrafficLights3, Diff_ShouldCarStopForLight_Unkn) {
    int32 model = Scenario::FindLoadedVehicleModel();
    if (model < 0) return;
    Scenario::TestVehicle veh(model, CVector(2525.f, -1640.f, 13.5f));

    bool orig, rev;
    { HookDisableGuard guard("Global/CTrafficLights/ShouldCarStopForLight");
      orig = CTrafficLights::ShouldCarStopForLight(veh.AsVehicle(), true); }
    rev = CTrafficLights::ShouldCarStopForLight(veh.AsVehicle(), true);
    EXPECT_EQ(orig, rev);
}

// ── ShouldCarStopForLight on multiple positions ──
// Spawn vehicles at different city locations to exercise different path node states.

GAME_DIFF_TEST(CTrafficLights3, Diff_ShouldCarStopForLight_Positions) {
    int32 model = Scenario::FindLoadedVehicleModel();
    if (model < 0) return;

    CVector positions[] = {
        CVector(2495.f, -1665.f, 13.3f),   // LS — Grove Street area
        CVector(1545.f, -1675.f, 13.5f),    // LS — near Unity Station
        CVector(2025.f, -1420.f, 24.0f),    // LS — east side
    };

    for (auto& pos : positions) {
        Scenario::TestVehicle veh(model, pos);
        bool orig, rev;
        { HookDisableGuard guard("Global/CTrafficLights/ShouldCarStopForLight");
          orig = CTrafficLights::ShouldCarStopForLight(veh.AsVehicle(), false); }
        rev = CTrafficLights::ShouldCarStopForLight(veh.AsVehicle(), false);
        EXPECT_EQ(orig, rev);
    }
}

// ── Behavior: LightForPeds cycle sanity ──
// Verify LightForPeds returns a valid traffic light state.

GAME_TEST(CTrafficLights3, LightForPeds_ValidState) {
    uint8 state = CTrafficLights::LightForPeds();
    EXPECT_TRUE(state == LIGHT_GREEN || state == LIGHT_YELLOW || state == LIGHT_RED);
}

// ── Behavior: IsMITrafficLight known models ──
// Model indices are runtime-resolved from game data, use ModelIndices:: refs.

GAME_TEST(CTrafficLights3, IsMITrafficLight_KnownModels) {
    // These should be traffic lights
    EXPECT_TRUE(CTrafficLights::IsMITrafficLight(ModelIndices::MI_TRAFFICLIGHTS));
    EXPECT_TRUE(CTrafficLights::IsMITrafficLight(ModelIndices::MI_TRAFFICLIGHTS_VERTICAL));
    EXPECT_TRUE(CTrafficLights::IsMITrafficLight(ModelIndices::MI_TRAFFICLIGHTS_MIAMI));
    EXPECT_TRUE(CTrafficLights::IsMITrafficLight(ModelIndices::MI_TRAFFICLIGHTS_VEGAS));
    EXPECT_TRUE(CTrafficLights::IsMITrafficLight(ModelIndices::MI_TRAFFICLIGHTS_TWOVERTICAL));

    // These should NOT be traffic lights
    EXPECT_FALSE(CTrafficLights::IsMITrafficLight(0));
    EXPECT_FALSE(CTrafficLights::IsMITrafficLight(596));   // police car
    EXPECT_FALSE(CTrafficLights::IsMITrafficLight(1000));
}
