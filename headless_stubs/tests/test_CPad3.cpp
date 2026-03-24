// test_CPad3.cpp — Differential tests for overloaded CPad functions.
// CPad uses RH_ScopedOverloadedInstall with distinguishing names.
// Hook paths use dash: Global/CPad/GetPedWalkLeftRight-ped
//
// These are instance methods on CPad — called on CPad::GetPad(0).

#include "StdInc.h"
#include "TestFramework.h"
#include "Pad.h"
#include "Entity/Ped/Ped.h"

// --- GetPedWalkLeftRight (ped variant) ---

GAME_DIFF_TEST(CPad3, GetPedWalkLeftRight_Ped) {
    auto* pad = CPad::GetPad(0);
    auto* player = FindPlayerPed(0);
    if (!pad || !player) return;
    int16 orig, rev;
    { HookDisableGuard guard("Global/CPad/GetPedWalkLeftRight-ped");
      orig = pad->GetPedWalkLeftRight(player); }
    rev = pad->GetPedWalkLeftRight(player);
    EXPECT_EQ(orig, rev);
}

// --- GetPedWalkUpDown (ped variant) ---

GAME_DIFF_TEST(CPad3, GetPedWalkUpDown_Ped) {
    auto* pad = CPad::GetPad(0);
    auto* player = FindPlayerPed(0);
    if (!pad || !player) return;
    int16 orig, rev;
    { HookDisableGuard guard("Global/CPad/GetPedWalkUpDown-ped");
      orig = pad->GetPedWalkUpDown(player); }
    rev = pad->GetPedWalkUpDown(player);
    EXPECT_EQ(orig, rev);
}

// --- GetPedWalkLeftRight (no-ped variant, empty suffix) ---

GAME_DIFF_TEST(CPad3, GetPedWalkLeftRight_NoPed) {
    auto* pad = CPad::GetPad(0);
    if (!pad) return;
    int16 orig, rev;
    { HookDisableGuard guard("Global/CPad/GetPedWalkLeftRight-");
      orig = pad->GetPedWalkLeftRight(); }
    rev = pad->GetPedWalkLeftRight();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CPad3, GetPedWalkUpDown_NoPed) {
    auto* pad = CPad::GetPad(0);
    if (!pad) return;
    int16 orig, rev;
    { HookDisableGuard guard("Global/CPad/GetPedWalkUpDown-");
      orig = pad->GetPedWalkUpDown(); }
    rev = pad->GetPedWalkUpDown();
    EXPECT_EQ(orig, rev);
}

// GetWeapon/GetMeleeAttack take args. Skipped.
