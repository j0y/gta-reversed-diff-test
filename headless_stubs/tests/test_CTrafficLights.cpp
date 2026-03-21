// test_CTrafficLights.cpp — Differential tests for CTrafficLights.
// Hook paths: Global/CTrafficLights/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "TrafficLights.h"

GAME_DIFF_TEST(CTrafficLights, LightForPeds) {
    uint8 orig, rev;
    { HookDisableGuard guard("Global/CTrafficLights/LightForPeds"); orig = CTrafficLights::LightForPeds(); }
    rev = CTrafficLights::LightForPeds();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CTrafficLights, IsMITrafficLight) {
    // Test known traffic light model IDs and non-traffic-light models
    int ids[] = { 1315, 1316, 1317, 1318, 3756, 3857,
                  0, 400, 500, 1000, 5000 };
    for (int id : ids) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CTrafficLights/IsMITrafficLight"); orig = CTrafficLights::IsMITrafficLight(id); }
        rev = CTrafficLights::IsMITrafficLight(id);
        EXPECT_EQ(orig, rev);
    }
}
