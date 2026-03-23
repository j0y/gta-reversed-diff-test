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
