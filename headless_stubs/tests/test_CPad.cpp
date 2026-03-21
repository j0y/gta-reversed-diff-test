// test_CPad.cpp — Differential tests for CPad.
// Hook paths: Global/CPad/<fn>
// CPad is instance-based — use CPad::GetPad(0) for player 1.

#include "StdInc.h"
#include "TestFramework.h"

// Helper: generate diff test for a CPad const bool member
#define DIFF_PAD_BOOL(FnName) \
    GAME_DIFF_TEST(CPad, FnName) { \
        auto* pad = CPad::GetPad(0); \
        if (!pad) return; \
        bool orig, rev; \
        { HookDisableGuard guard("Global/CPad/" #FnName); orig = pad->FnName(); } \
        rev = pad->FnName(); \
        EXPECT_EQ(orig, rev); \
    }

// Helper: diff test for int16 return
#define DIFF_PAD_INT16(FnName) \
    GAME_DIFF_TEST(CPad, FnName) { \
        auto* pad = CPad::GetPad(0); \
        if (!pad) return; \
        int16 orig, rev; \
        { HookDisableGuard guard("Global/CPad/" #FnName); orig = pad->FnName(); } \
        rev = pad->FnName(); \
        EXPECT_EQ(orig, rev); \
    }

DIFF_PAD_BOOL(GetLookLeft)
DIFF_PAD_BOOL(GetLookRight)
DIFF_PAD_BOOL(GetHorn)
DIFF_PAD_BOOL(HornJustDown)
DIFF_PAD_BOOL(GetExitVehicle)
DIFF_PAD_BOOL(ExitVehicleJustDown)
DIFF_PAD_BOOL(GetTarget)
DIFF_PAD_BOOL(GetSprint)
DIFF_PAD_BOOL(SprintJustDown)
DIFF_PAD_BOOL(GetJump)
DIFF_PAD_BOOL(JumpJustDown)
DIFF_PAD_BOOL(GetDuck)
DIFF_PAD_BOOL(DuckJustDown)
DIFF_PAD_BOOL(GetForceCameraBehindPlayer)
DIFF_PAD_BOOL(SniperZoomIn)
DIFF_PAD_BOOL(SniperZoomOut)
DIFF_PAD_BOOL(GetEnterTargeting)
DIFF_PAD_BOOL(GetLookBehindForCar)
DIFF_PAD_BOOL(GetLookBehindForPed)
DIFF_PAD_BOOL(ShiftTargetLeftJustDown)
DIFF_PAD_BOOL(ShiftTargetRightJustDown)
DIFF_PAD_BOOL(GetGroupControlForward)
DIFF_PAD_BOOL(GetGroupControlBack)
DIFF_PAD_BOOL(ConversationYesJustDown)
DIFF_PAD_BOOL(ConversationNoJustDown)
DIFF_PAD_BOOL(CycleWeaponLeftJustDown)
DIFF_PAD_BOOL(CycleWeaponRightJustDown)

DIFF_PAD_INT16(GetSteeringUpDown)
DIFF_PAD_INT16(GetBrake)
DIFF_PAD_INT16(GetHandBrake)
DIFF_PAD_INT16(GetAccelerate)
#undef DIFF_PAD_BOOL
#undef DIFF_PAD_INT16

// Note: Static CPad functions (IsEnterJustPressed, IsMouseLButtonPressed, etc.)
// are inline/not hooked — cannot diff test.
// GetPedWalkLeftRight/UpDown have overloaded names with suffixes in hooks.csv — skip.
