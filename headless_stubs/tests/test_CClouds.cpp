// test_CClouds.cpp — Differential tests for CClouds.
// Hook paths: Global/CClouds/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "Clouds.h"

GAME_DIFF_TEST(CClouds, MovingFog_GetFXIntensity) {
    float orig, rev;
    { HookDisableGuard guard("Global/CClouds/MovingFog_GetFXIntensity"); orig = CClouds::MovingFog_GetFXIntensity(); }
    rev = CClouds::MovingFog_GetFXIntensity();
    EXPECT_NEAR(orig, rev, 1e-5f);
}

GAME_DIFF_TEST(CClouds, VolumetricCloudsGetMaxDistance) {
    float orig, rev;
    { HookDisableGuard guard("Global/CClouds/VolumetricCloudsGetMaxDistance"); orig = CClouds::VolumetricCloudsGetMaxDistance(); }
    rev = CClouds::VolumetricCloudsGetMaxDistance();
    EXPECT_NEAR(orig, rev, 1e-5f);
}
