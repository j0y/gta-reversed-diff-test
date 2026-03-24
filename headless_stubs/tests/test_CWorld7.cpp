// test_CWorld7.cpp — CWorld object-finding query tests.
// Hook paths: Global/CWorld/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "World.h"
#include "Entity/Ped/Ped.h"
#include "Entity/Vehicle/Vehicle.h"

// --- FindUnsuspectingTargetCar ---

GAME_DIFF_TEST(CWorld7, FindUnsuspectingTargetCar) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    CVector pos = player->GetPosition();
    CVehicle* orig;
    CVehicle* rev;
    { HookDisableGuard guard("Global/CWorld/FindUnsuspectingTargetCar");
      orig = CWorld::FindUnsuspectingTargetCar(pos, player->GetForward()); }
    rev = CWorld::FindUnsuspectingTargetCar(pos, player->GetForward());
    EXPECT_EQ(orig, rev);
}

// --- FindUnsuspectingTargetPed ---

GAME_DIFF_TEST(CWorld7, FindUnsuspectingTargetPed) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    CVector pos = player->GetPosition();
    CPed* orig;
    CPed* rev;
    { HookDisableGuard guard("Global/CWorld/FindUnsuspectingTargetPed");
      orig = CWorld::FindUnsuspectingTargetPed(pos, player->GetForward()); }
    rev = CWorld::FindUnsuspectingTargetPed(pos, player->GetForward());
    EXPECT_EQ(orig, rev);
}

// --- FindPlayerTrain ---

GAME_DIFF_TEST(CWorld7, FindPlayerTrain) {
    CVehicle* orig;
    CVehicle* rev;
    { HookDisableGuard guard("Global/CWorld/FindPlayerTrain");
      orig = FindPlayerTrain(0); }
    rev = FindPlayerTrain(0);
    EXPECT_EQ(orig, rev);
}

// --- FindPlayerCentreOfWorld_NoSniperShift ---

GAME_DIFF_TEST(CWorld7, FindPlayerCentreOfWorld_NoSniperShift) {
    CVector orig, rev;
    { HookDisableGuard guard("Global/CWorld/FindPlayerCentreOfWorld_NoSniperShift");
      orig = FindPlayerCentreOfWorld_NoSniperShift(0); }
    rev = FindPlayerCentreOfWorld_NoSniperShift(0);
    EXPECT_NEAR(orig.x, rev.x, 1e-3f);
    EXPECT_NEAR(orig.y, rev.y, 1e-3f);
    EXPECT_NEAR(orig.z, rev.z, 1e-3f);
}
