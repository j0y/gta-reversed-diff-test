// test_CCamera4.cpp — CCamera tests.
// Find3rdPersonQuickAimPitch is state-dependent (camera moves between hook toggles).
// Tested as behavior only.

#include "StdInc.h"
#include "TestFramework.h"
#include "Camera.h"

GAME_TEST(CCamera4, Find3rdPersonQuickAimPitch_NoCrash) {
    float val = TheCamera.Find3rdPersonQuickAimPitch();
    EXPECT_TRUE(val > -3.15f && val < 3.15f); // within reasonable radian range
}
