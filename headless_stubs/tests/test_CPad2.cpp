// test_CPad2.cpp — Deep differential tests for CPad.
// Hook paths: Global/CPad/<fn>
//
// CPad methods are instance methods on the player pad (CPad::GetPad(0)).
// Most return int16 input values at the current frame.

#include "StdInc.h"
#include "TestFramework.h"
#include "Pad.h"

#define PAD_DIFF(Name) \
    GAME_DIFF_TEST(CPad2, Name) { \
        auto* pad = CPad::GetPad(0); \
        if (!pad) return; \
        int16 orig, rev; \
        { HookDisableGuard guard("Global/CPad/" #Name); orig = pad->Name(); } \
        rev = pad->Name(); \
        EXPECT_EQ(orig, rev); \
    }

PAD_DIFF(GetSteeringLeftRight)
PAD_DIFF(GetSteeringUpDown)
PAD_DIFF(GetCarGunLeftRight)
PAD_DIFF(GetCarGunUpDown)
PAD_DIFF(GetCarGunFired)
PAD_DIFF(GetLookLeft)
PAD_DIFF(GetLookRight)
PAD_DIFF(GetLookBehindForCar)
PAD_DIFF(GetLookBehindForPed)
PAD_DIFF(GetHorn)
PAD_DIFF(GetHandBrake)
PAD_DIFF(GetBrake)
PAD_DIFF(GetExitVehicle)
PAD_DIFF(GetAccelerate)
PAD_DIFF(GetAccelerateJustDown)
PAD_DIFF(GetTarget)
PAD_DIFF(GetSprint)
// GetMeleeAttack, GetWeapon, GetDisplayVitalStats take args — skip macro form
PAD_DIFF(GetHydraulicJump)
PAD_DIFF(GetDuck)
PAD_DIFF(GetJump)
PAD_DIFF(GetForceCameraBehindPlayer)
PAD_DIFF(ShiftTargetLeftJustDown)
PAD_DIFF(ShiftTargetRightJustDown)
PAD_DIFF(GetGroupControlForward)
PAD_DIFF(GetGroupControlBack)
PAD_DIFF(GetAnaloguePadUp)
PAD_DIFF(GetAnaloguePadDown)
PAD_DIFF(GetAnaloguePadLeft)
PAD_DIFF(GetAnaloguePadRight)
