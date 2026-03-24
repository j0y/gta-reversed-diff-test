// test_CReplay3.cpp — Extended differential tests for CReplay.
// Hook paths: Global/CReplay/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "Replay.h"

GAME_DIFF_TEST(CReplay3, ShouldStandardCameraBeProcessed) {
    bool orig, rev;
    { HookDisableGuard guard("Global/CReplay/ShouldStandardCameraBeProcessed"); orig = CReplay::ShouldStandardCameraBeProcessed(); }
    rev = CReplay::ShouldStandardCameraBeProcessed();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CReplay3, NumberFramesAvailableToPlay) {
    uint32 orig, rev;
    { HookDisableGuard guard("Global/CReplay/NumberFramesAvailableToPlay"); orig = CReplay::NumberFramesAvailableToPlay(); }
    rev = CReplay::NumberFramesAvailableToPlay();
    EXPECT_EQ(orig, rev);
}

// FindFirstFocusCoordinate returns void + takes CVector& — test as mutation
GAME_DIFF_TEST(CReplay3, FindFirstFocusCoordinate) {
    CVector origCoord{}, revCoord{};
    { HookDisableGuard guard("Global/CReplay/FindFirstFocusCoordinate");
      CReplay::FindFirstFocusCoordinate(origCoord); }
    CReplay::FindFirstFocusCoordinate(revCoord);
    EXPECT_NEAR(origCoord.x, revCoord.x, 1e-3f);
    EXPECT_NEAR(origCoord.y, revCoord.y, 1e-3f);
    EXPECT_NEAR(origCoord.z, revCoord.z, 1e-3f);
}

// --- IsThisPedUsedInRecording ---

GAME_DIFF_TEST(CReplay3, IsThisPedUsedInRecording) {
    bool orig, rev;
    { HookDisableGuard guard("Global/CReplay/IsThisPedUsedInRecording");
      orig = CReplay::IsThisPedUsedInRecording(0); }
    rev = CReplay::IsThisPedUsedInRecording(0);
    EXPECT_EQ(orig, rev);
}

// --- IsThisVehicleUsedInRecording ---

GAME_DIFF_TEST(CReplay3, IsThisVehicleUsedInRecording) {
    bool orig, rev;
    { HookDisableGuard guard("Global/CReplay/IsThisVehicleUsedInRecording");
      orig = CReplay::IsThisVehicleUsedInRecording(0); }
    rev = CReplay::IsThisVehicleUsedInRecording(0);
    EXPECT_EQ(orig, rev);
}
