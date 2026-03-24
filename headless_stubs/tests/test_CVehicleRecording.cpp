// test_CVehicleRecording.cpp — Deep differential tests.
// Hook paths: Global/CVehicleRecording/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "VehicleRecording.h"

// --- HasRecordingFileBeenLoaded ---

GAME_DIFF_TEST(CVehicleRecording, HasRecordingFileBeenLoaded_Various) {
    int32 files[] = { 0, 1, 5, 10, 50, 100, 500 };
    for (int32 f : files) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CVehicleRecording/HasRecordingFileBeenLoaded");
          orig = CVehicleRecording::HasRecordingFileBeenLoaded(f); }
        rev = CVehicleRecording::HasRecordingFileBeenLoaded(f);
        EXPECT_EQ(orig, rev);
    }
}

// --- RegisterRecordingFile ---

GAME_DIFF_TEST(CVehicleRecording, RegisterRecordingFile_Various) {
    const char* names[] = { "test1", "carrec0", "invalid_xyz" };
    for (auto* name : names) {
        int32 orig, rev;
        { HookDisableGuard guard("Global/CVehicleRecording/RegisterRecordingFile");
          orig = CVehicleRecording::RegisterRecordingFile(name); }
        rev = CVehicleRecording::RegisterRecordingFile(name);
        EXPECT_EQ(orig, rev);
    }
}

// --- FindIndexWithFileNameNumber ---

GAME_DIFF_TEST(CVehicleRecording, FindIndexWithFileNameNumber_Range) {
    for (int32 fileNum = 0; fileNum < 20; fileNum++) {
        int32 orig, rev;
        { HookDisableGuard guard("Global/CVehicleRecording/FindIndexWithFileNameNumber");
          orig = CVehicleRecording::FindIndexWithFileNameNumber(fileNum); }
        rev = CVehicleRecording::FindIndexWithFileNameNumber(fileNum);
        EXPECT_EQ(orig, rev);
    }
}

// --- IsPlaybackGoingOnForCar ---

GAME_DIFF_TEST(CVehicleRecording, IsPlaybackGoingOnForCar_Null) {
    bool orig, rev;
    { HookDisableGuard guard("Global/CVehicleRecording/IsPlaybackGoingOnForCar");
      orig = CVehicleRecording::IsPlaybackGoingOnForCar(nullptr); }
    rev = CVehicleRecording::IsPlaybackGoingOnForCar(nullptr);
    EXPECT_EQ(orig, rev);
}

// --- IsPlaybackPausedForCar ---

GAME_DIFF_TEST(CVehicleRecording, IsPlaybackPausedForCar_Null) {
    bool orig, rev;
    { HookDisableGuard guard("Global/CVehicleRecording/IsPlaybackPausedForCar");
      orig = CVehicleRecording::IsPlaybackPausedForCar(nullptr); }
    rev = CVehicleRecording::IsPlaybackPausedForCar(nullptr);
    EXPECT_EQ(orig, rev);
}
