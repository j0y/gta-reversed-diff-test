// test_CPad4.cpp — Deep differential tests for CPad input queries.
// Hook paths: Global/CPad/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "Pad.h"

#define PAD_TEST(Name) \
GAME_DIFF_TEST(CPad4, Name) { \
    auto* pad = CPad::GetPad(0); \
    if (!pad) return; \
    int16 orig, rev; \
    { HookDisableGuard guard("Global/CPad/" #Name); orig = pad->Name(); } \
    rev = pad->Name(); \
    EXPECT_EQ(orig, rev); \
}

PAD_TEST(GetAccelerate)
PAD_TEST(GetBrake)
PAD_TEST(GetHandBrake)
PAD_TEST(GetHorn)
PAD_TEST(GetDuck)
PAD_TEST(GetExitVehicle)
PAD_TEST(GetCarGunFired)
PAD_TEST(GetAccelerateJustDown)
PAD_TEST(GetForceCameraBehindPlayer)
PAD_TEST(GetGroupControlForward)
PAD_TEST(GetGroupControlBack)
PAD_TEST(GetHydraulicJump)
PAD_TEST(GetAnaloguePadUp)
PAD_TEST(GetAnaloguePadDown)
PAD_TEST(GetAnaloguePadLeft)
PAD_TEST(GetAnaloguePadRight)
PAD_TEST(GetCarGunLeftRight)
PAD_TEST(GetCarGunUpDown)
PAD_TEST(GetEnterTargeting)

#undef PAD_TEST
