// test_CReplay2.cpp — Deep differential tests for CReplay.
// Hook paths: Global/CReplay/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "Replay.h"

// --- ShouldStandardCameraBeProcessed ---

GAME_DIFF_TEST(CReplay, ShouldStandardCameraBeProcessed2) {
    bool orig, rev;
    { HookDisableGuard guard("Global/CReplay/ShouldStandardCameraBeProcessed"); orig = CReplay::ShouldStandardCameraBeProcessed(); }
    rev = CReplay::ShouldStandardCameraBeProcessed();
    EXPECT_EQ(orig, rev);
}

// --- NumberFramesAvailableToPlay ---

GAME_DIFF_TEST(CReplay, NumberFramesAvailableToPlay2) {
    uint32 orig, rev;
    { HookDisableGuard guard("Global/CReplay/NumberFramesAvailableToPlay"); orig = CReplay::NumberFramesAvailableToPlay(); }
    rev = CReplay::NumberFramesAvailableToPlay();
    EXPECT_EQ(orig, rev);
}

// --- CanWeFindPoolIndexForPed ---

GAME_DIFF_TEST(CReplay, CanWeFindPoolIndexForPed_Various) {
    int32 indices[] = { 0, 1, 5, 10, 50, 100, 500 };
    for (int32 i : indices) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CReplay/CanWeFindPoolIndexForPed"); orig = CReplay::CanWeFindPoolIndexForPed(i); }
        rev = CReplay::CanWeFindPoolIndexForPed(i);
        EXPECT_EQ(orig, rev);
    }
}

// --- CanWeFindPoolIndexForVehicle ---

GAME_DIFF_TEST(CReplay, CanWeFindPoolIndexForVehicle_Various) {
    int32 indices[] = { 0, 1, 5, 10, 50, 100, 500 };
    for (int32 i : indices) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CReplay/CanWeFindPoolIndexForVehicle"); orig = CReplay::CanWeFindPoolIndexForVehicle(i); }
        rev = CReplay::CanWeFindPoolIndexForVehicle(i);
        EXPECT_EQ(orig, rev);
    }
}

// --- IsThisPedUsedInRecording ---

GAME_DIFF_TEST(CReplay, IsThisPedUsedInRecording_Various) {
    int32 indices[] = { 0, 1, 5, 10, 50 };
    for (int32 i : indices) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CReplay/IsThisPedUsedInRecording"); orig = CReplay::IsThisPedUsedInRecording(i); }
        rev = CReplay::IsThisPedUsedInRecording(i);
        EXPECT_EQ(orig, rev);
    }
}

// --- IsThisVehicleUsedInRecording ---

GAME_DIFF_TEST(CReplay, IsThisVehicleUsedInRecording_Various) {
    int32 indices[] = { 0, 1, 5, 10, 50 };
    for (int32 i : indices) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CReplay/IsThisVehicleUsedInRecording"); orig = CReplay::IsThisVehicleUsedInRecording(i); }
        rev = CReplay::IsThisVehicleUsedInRecording(i);
        EXPECT_EQ(orig, rev);
    }
}
