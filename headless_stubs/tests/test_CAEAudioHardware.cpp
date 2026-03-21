// test_CAEAudioHardware.cpp — Differential tests for CAEAudioHardware queries.
// Hook paths: Audio/Hardware/CAEAudioHardware/<fn>
// AEAudioHardware is a global singleton — audio channel/track/volume queries.

#include "StdInc.h"
#include "TestFramework.h"
#include "Audio/hardware/AEAudioHardware.h"

GAME_DIFF_TEST(CAEAudioHardware, GetNumAvailableChannels) {
    uint16 orig, rev;
    { HookDisableGuard guard("Audio/Hardware/CAEAudioHardware/GetNumAvailableChannels");
      orig = AEAudioHardware.GetNumAvailableChannels(); }
    rev = AEAudioHardware.GetNumAvailableChannels();
    EXPECT_EQ(orig, rev);
}

// Track queries crash in original code when audio hardware not initialized (headless).
// Use behavior tests.

// Track queries (GetTrackPlayTime, GetTrackLengthMs, GetActiveTrackID, GetPlayingTrackID)
// crash even in reversed code headlessly — audio hardware not initialized. Skipped.

GAME_DIFF_TEST(CAEAudioHardware, GetMusicMasterScalingFactor) {
    float orig, rev;
    { HookDisableGuard guard("Audio/Hardware/CAEAudioHardware/GetMusicMasterScalingFactor");
      orig = AEAudioHardware.GetMusicMasterScalingFactor(); }
    rev = AEAudioHardware.GetMusicMasterScalingFactor();
    EXPECT_NEAR(orig, rev, 1e-5f);
}

GAME_DIFF_TEST(CAEAudioHardware, GetEffectsMasterScalingFactor) {
    float orig, rev;
    { HookDisableGuard guard("Audio/Hardware/CAEAudioHardware/GetEffectsMasterScalingFactor");
      orig = AEAudioHardware.GetEffectsMasterScalingFactor(); }
    rev = AEAudioHardware.GetEffectsMasterScalingFactor();
    EXPECT_NEAR(orig, rev, 1e-5f);
}

GAME_DIFF_TEST(CAEAudioHardware, GetEffectsFaderScalingFactor) {
    float orig, rev;
    { HookDisableGuard guard("Audio/Hardware/CAEAudioHardware/GetEffectsFaderScalingFactor");
      orig = AEAudioHardware.GetEffectsFaderScalingFactor(); }
    rev = AEAudioHardware.GetEffectsFaderScalingFactor();
    EXPECT_NEAR(orig, rev, 1e-5f);
}

// DVD queries crash in original code headlessly — behavior tests only.

// DVD queries (IsStreamingFromDVD, GetDVDDriveLetter, CheckDVD) crash headlessly. Skipped.
