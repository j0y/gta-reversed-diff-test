// test_CTrafficLights2.cpp — Deep differential tests for CTrafficLights.
// Hook paths: Global/CTrafficLights/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "TrafficLights.h"

// --- LightForPeds (no args, reads game clock) ---

GAME_DIFF_TEST(CTrafficLights, LightForPeds2) {
    uint8 orig, rev;
    { HookDisableGuard guard("Global/CTrafficLights/LightForPeds"); orig = CTrafficLights::LightForPeds(); }
    rev = CTrafficLights::LightForPeds();
    EXPECT_EQ(orig, rev);
}

// LightForCars1/2 and Visual variants are private — skip

// --- IsMITrafficLight ---

GAME_DIFF_TEST(CTrafficLights, IsMITrafficLight_Various) {
    int32 models[] = { 0, 1, 100, 1283, 1315, 1350, 1380, 5000, 10000 };
    for (int32 m : models) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CTrafficLights/IsMITrafficLight"); orig = CTrafficLights::IsMITrafficLight(m); }
        rev = CTrafficLights::IsMITrafficLight(m);
        EXPECT_EQ(orig, rev);
    }
}

// FindTrafficLightType takes CEntity* — skip (need real entity)
