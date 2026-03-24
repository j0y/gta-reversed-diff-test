// test_CWorld5.cpp — Differential tests for CWorld player-finding functions.
// Hook paths: Global/CWorld/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "World.h"
#include "Entity/Ped/Ped.h"

// --- FindPlayerPed ---

GAME_DIFF_TEST(CWorld5, FindPlayerPed_Player0) {
    CPed* orig;
    CPed* rev;
    { HookDisableGuard guard("Global/CWorld/FindPlayerPed");
      orig = FindPlayerPed(0); }
    rev = FindPlayerPed(0);
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CWorld5, FindPlayerPed_InvalidSlot) {
    CPed* orig;
    CPed* rev;
    { HookDisableGuard guard("Global/CWorld/FindPlayerPed");
      orig = FindPlayerPed(-1); }
    rev = FindPlayerPed(-1);
    EXPECT_EQ(orig, rev);
}

// --- FindPlayerVehicle ---

GAME_DIFF_TEST(CWorld5, FindPlayerVehicle_Player0) {
    CVehicle* orig;
    CVehicle* rev;
    { HookDisableGuard guard("Global/CWorld/FindPlayerVehicle");
      orig = FindPlayerVehicle(0, false); }
    rev = FindPlayerVehicle(0, false);
    EXPECT_EQ(orig, rev);
}

// --- FindPlayerEntity ---

GAME_DIFF_TEST(CWorld5, FindPlayerEntity_Player0) {
    CEntity* orig;
    CEntity* rev;
    { HookDisableGuard guard("Global/CWorld/FindPlayerEntity");
      orig = FindPlayerEntity(0); }
    rev = FindPlayerEntity(0);
    EXPECT_EQ(orig, rev);
}

// --- FindPlayerHeading ---

GAME_DIFF_TEST(CWorld5, FindPlayerHeading_Player0) {
    float orig, rev;
    { HookDisableGuard guard("Global/CWorld/FindPlayerHeading");
      orig = FindPlayerHeading(0); }
    rev = FindPlayerHeading(0);
    EXPECT_NEAR(orig, rev, 1e-5f);
}

// --- FindPlayerSpeed ---

GAME_DIFF_TEST(CWorld5, FindPlayerSpeed_Player0) {
    CVector orig, rev;
    { HookDisableGuard guard("Global/CWorld/FindPlayerSpeed");
      orig = FindPlayerSpeed(0); }
    rev = FindPlayerSpeed(0);
    EXPECT_NEAR(orig.x, rev.x, 1e-5f);
    EXPECT_NEAR(orig.y, rev.y, 1e-5f);
    EXPECT_NEAR(orig.z, rev.z, 1e-5f);
}

// --- FindPlayerWanted ---

GAME_DIFF_TEST(CWorld5, FindPlayerWanted_Player0) {
    CWanted* orig;
    CWanted* rev;
    { HookDisableGuard guard("Global/CWorld/FindPlayerWanted");
      orig = FindPlayerWanted(0); }
    rev = FindPlayerWanted(0);
    EXPECT_EQ(orig, rev);
}

// --- FindPlayerCentreOfWorld ---

GAME_DIFF_TEST(CWorld5, FindPlayerCentreOfWorld_Player0) {
    CVector orig, rev;
    { HookDisableGuard guard("Global/CWorld/FindPlayerCentreOfWorld");
      orig = FindPlayerCentreOfWorld(0); }
    rev = FindPlayerCentreOfWorld(0);
    EXPECT_NEAR(orig.x, rev.x, 1e-3f);
    EXPECT_NEAR(orig.y, rev.y, 1e-3f);
    EXPECT_NEAR(orig.z, rev.z, 1e-3f);
}

// --- TestSphereAgainstWorld ---

GAME_DIFF_TEST(CWorld5, TestSphereAgainstWorld) {
    CVector positions[] = {
        {2488.f, -1666.f, 13.f},
        {0.f, 0.f, 0.f},
    };
    for (auto& pos : positions) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CWorld/TestSphereAgainstWorld");
          orig = CWorld::TestSphereAgainstWorld(pos, 2.0f, nullptr, true, true, false, false, false, false); }
        rev = CWorld::TestSphereAgainstWorld(pos, 2.0f, nullptr, true, true, false, false, false, false);
        EXPECT_EQ(orig, rev);
    }
}
