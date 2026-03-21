// test_CTimer.cpp — Differential + behavior tests for CTimer.
// Hook paths: Global/CTimer/<fn>
//
// Core timing system. Most functions are simple state reads/writes.
// Update() depends on wall clock so we test UpdateVariables() instead
// (deterministic given a known timeElapsed input).

#include "StdInc.h"
#include "TestFramework.h"

// --- Differential tests ---

GAME_DIFF_TEST(CTimer, GetCyclesPerMillisecond) {
    uint32 rev = CTimer::GetCyclesPerMillisecond();
    uint32 orig;
    { HookDisableGuard guard("Global/CTimer/GetCyclesPerMillisecond");
      orig = CTimer::GetCyclesPerMillisecond(); }
    EXPECT_EQ(rev, orig);
}

GAME_DIFF_TEST(CTimer, GetCyclesPerFrame) {
    uint32 rev = CTimer::GetCyclesPerFrame();
    uint32 orig;
    { HookDisableGuard guard("Global/CTimer/GetCyclesPerFrame");
      orig = CTimer::GetCyclesPerFrame(); }
    EXPECT_EQ(rev, orig);
}

GAME_DIFF_TEST(CTimer, GetIsSlowMotionActive) {
    // Test with current state
    bool rev = CTimer::GetIsSlowMotionActive();
    bool orig;
    { HookDisableGuard guard("Global/CTimer/GetIsSlowMotionActive");
      orig = CTimer::GetIsSlowMotionActive(); }
    EXPECT_EQ(rev, orig);

    // Test with forced slow motion (timeScale < 1)
    float savedScale = CTimer::ms_fTimeScale;
    CTimer::ms_fTimeScale = 0.5f;
    rev = CTimer::GetIsSlowMotionActive();
    { HookDisableGuard guard("Global/CTimer/GetIsSlowMotionActive");
      orig = CTimer::GetIsSlowMotionActive(); }
    EXPECT_EQ(rev, orig);
    EXPECT_TRUE(rev);

    // Test with normal speed
    CTimer::ms_fTimeScale = 1.0f;
    rev = CTimer::GetIsSlowMotionActive();
    { HookDisableGuard guard("Global/CTimer/GetIsSlowMotionActive");
      orig = CTimer::GetIsSlowMotionActive(); }
    EXPECT_EQ(rev, orig);
    EXPECT_FALSE(rev);

    CTimer::ms_fTimeScale = savedScale;
}

GAME_DIFF_TEST(CTimer, Stop) {
    // Save state
    uint32 savedTime = CTimer::m_snTimeInMilliseconds;
    uint32 savedPrev = CTimer::m_snPreviousTimeInMilliseconds;
    uint32 savedPP = CTimer::m_snPPreviousTimeInMilliseconds;
    uint32 savedPPP = CTimer::m_snPPPreviousTimeInMilliseconds;
    uint32 savedPPPP = CTimer::m_snPPPPreviousTimeInMilliseconds;
    uint32 savedNonClipped = CTimer::m_snTimeInMillisecondsNonClipped;
    uint32 savedPrevNonClipped = CTimer::m_snPreviousTimeInMillisecondsNonClipped;
    bool savedDebug = CTimer::m_sbEnableTimeDebug;

    // Set known state
    CTimer::m_snTimeInMilliseconds = 12345;
    CTimer::m_snTimeInMillisecondsNonClipped = 12340;
    CTimer::m_snPreviousTimeInMilliseconds = 11111;

    // Reversed Stop
    CTimer::Stop();
    uint32 revPrev = CTimer::m_snPreviousTimeInMilliseconds;
    uint32 revPP = CTimer::m_snPPreviousTimeInMilliseconds;
    uint32 revPPP = CTimer::m_snPPPreviousTimeInMilliseconds;
    uint32 revPPPP = CTimer::m_snPPPPreviousTimeInMilliseconds;
    uint32 revPrevNC = CTimer::m_snPreviousTimeInMillisecondsNonClipped;
    bool revDebug = CTimer::m_sbEnableTimeDebug;

    // Reset and run original
    CTimer::m_snTimeInMilliseconds = 12345;
    CTimer::m_snTimeInMillisecondsNonClipped = 12340;
    CTimer::m_snPreviousTimeInMilliseconds = 11111;
    CTimer::m_sbEnableTimeDebug = true;
    { HookDisableGuard guard("Global/CTimer/Stop");
      CTimer::Stop(); }
    EXPECT_EQ(revPrev, CTimer::m_snPreviousTimeInMilliseconds);
    EXPECT_EQ(revPP, CTimer::m_snPPreviousTimeInMilliseconds);
    EXPECT_EQ(revPPP, CTimer::m_snPPPreviousTimeInMilliseconds);
    EXPECT_EQ(revPPPP, CTimer::m_snPPPPreviousTimeInMilliseconds);
    EXPECT_EQ(revPrevNC, CTimer::m_snPreviousTimeInMillisecondsNonClipped);
    EXPECT_EQ(revDebug, CTimer::m_sbEnableTimeDebug);

    // Restore
    CTimer::m_snTimeInMilliseconds = savedTime;
    CTimer::m_snPreviousTimeInMilliseconds = savedPrev;
    CTimer::m_snPPreviousTimeInMilliseconds = savedPP;
    CTimer::m_snPPPreviousTimeInMilliseconds = savedPPP;
    CTimer::m_snPPPPreviousTimeInMilliseconds = savedPPPP;
    CTimer::m_snTimeInMillisecondsNonClipped = savedNonClipped;
    CTimer::m_snPreviousTimeInMillisecondsNonClipped = savedPrevNonClipped;
    CTimer::m_sbEnableTimeDebug = savedDebug;
}

GAME_DIFF_TEST(CTimer, StartEndUserPause) {
    bool savedPause = CTimer::m_UserPause;

    // Reversed StartUserPause
    CTimer::StartUserPause();
    bool revAfterStart = CTimer::m_UserPause;
    CTimer::m_UserPause = false;
    { HookDisableGuard guard("Global/CTimer/StartUserPause");
      CTimer::StartUserPause(); }
    EXPECT_EQ(revAfterStart, CTimer::m_UserPause);

    // Reversed EndUserPause
    CTimer::EndUserPause();
    bool revAfterEnd = CTimer::m_UserPause;
    CTimer::m_UserPause = true;
    { HookDisableGuard guard("Global/CTimer/EndUserPause");
      CTimer::EndUserPause(); }
    EXPECT_EQ(revAfterEnd, CTimer::m_UserPause);

    CTimer::m_UserPause = savedPause;
}

GAME_DIFF_TEST(CTimer, Shutdown) {
    bool savedDebug = CTimer::m_sbEnableTimeDebug;

    CTimer::m_sbEnableTimeDebug = true;
    CTimer::Shutdown();
    bool revResult = CTimer::m_sbEnableTimeDebug;

    CTimer::m_sbEnableTimeDebug = true;
    { HookDisableGuard guard("Global/CTimer/Shutdown");
      CTimer::Shutdown(); }
    EXPECT_EQ(revResult, CTimer::m_sbEnableTimeDebug);

    CTimer::m_sbEnableTimeDebug = savedDebug;
}

GAME_DIFF_TEST(CTimer, UpdateVariables) {
    // Save all state that UpdateVariables modifies
    uint32 savedTimeNC = CTimer::m_snTimeInMillisecondsNonClipped;
    uint32 savedTime = CTimer::m_snTimeInMilliseconds;
    float savedStepNC = CTimer::ms_fTimeStepNonClipped;
    float savedOldStep = CTimer::ms_fOldTimeStep;
    float savedStep = CTimer::ms_fTimeStep;
    uint32 savedDivider = CTimer::m_snTimerDivider;
    bool savedUserPause = CTimer::m_UserPause;
    bool savedCodePause = CTimer::m_CodePause;

    // Test with several known timeElapsed values
    float inputs[] = { 0.0f, 1000.0f, 5000.0f, 20000.0f, 500000.0f };
    for (float elapsed : inputs) {
        // Set known starting state
        CTimer::m_snTimeInMillisecondsNonClipped = 10000;
        CTimer::m_snTimeInMilliseconds = 10000;
        CTimer::ms_fTimeStep = 1.0f;
        CTimer::ms_fOldTimeStep = 1.0f;
        CTimer::ms_fTimeStepNonClipped = 1.0f;
        CTimer::m_snTimerDivider = 1000; // 1 cycle per ms (simplified)
        CTimer::m_UserPause = false;
        CTimer::m_CodePause = false;

        // Reversed
        CTimer::UpdateVariables(elapsed);
        uint32 revTimeNC = CTimer::m_snTimeInMillisecondsNonClipped;
        uint32 revTime = CTimer::m_snTimeInMilliseconds;
        float revStepNC = CTimer::ms_fTimeStepNonClipped;
        float revStep = CTimer::ms_fTimeStep;

        // Reset and run original
        CTimer::m_snTimeInMillisecondsNonClipped = 10000;
        CTimer::m_snTimeInMilliseconds = 10000;
        CTimer::ms_fTimeStep = 1.0f;
        CTimer::ms_fOldTimeStep = 1.0f;
        CTimer::ms_fTimeStepNonClipped = 1.0f;
        CTimer::m_snTimerDivider = 1000;
        CTimer::m_UserPause = false;
        CTimer::m_CodePause = false;

        { HookDisableGuard guard("Global/CTimer/UpdateVariables");
          CTimer::UpdateVariables(elapsed); }
        EXPECT_EQ(revTimeNC, CTimer::m_snTimeInMillisecondsNonClipped);
        EXPECT_EQ(revTime, CTimer::m_snTimeInMilliseconds);
        EXPECT_NEAR(revStepNC, CTimer::ms_fTimeStepNonClipped, 1e-5f);
        EXPECT_NEAR(revStep, CTimer::ms_fTimeStep, 1e-5f);
    }

    // Restore
    CTimer::m_snTimeInMillisecondsNonClipped = savedTimeNC;
    CTimer::m_snTimeInMilliseconds = savedTime;
    CTimer::ms_fTimeStepNonClipped = savedStepNC;
    CTimer::ms_fOldTimeStep = savedOldStep;
    CTimer::ms_fTimeStep = savedStep;
    CTimer::m_snTimerDivider = savedDivider;
    CTimer::m_UserPause = savedUserPause;
    CTimer::m_CodePause = savedCodePause;
}

// --- Behavior tests ---

GAME_TEST(CTimer, TimeStepConversions) {
    // Verify conversion math: GetTimeStepInSeconds = step / 50, GetTimeStepInMS = step / 50 * 1000
    float step = CTimer::GetTimeStep();
    EXPECT_NEAR(CTimer::GetTimeStepInSeconds(), step / 50.0f, 1e-6f);
    EXPECT_NEAR(CTimer::GetTimeStepInMS(), step / 50.0f * 1000.0f, 1e-3f);
}

GAME_TEST(CTimer, FrameCounterIncreasing) {
    // At state 9, frame counter should be > 0
    EXPECT_TRUE(CTimer::GetFrameCounter() > 0);
}

GAME_TEST(CTimer, TimeInMSPositive) {
    EXPECT_TRUE(CTimer::GetTimeInMS() > 0);
    EXPECT_TRUE(CTimer::GetTimeInMSNonClipped() > 0);
}

GAME_TEST(CTimer, CyclesPerMillisecondPositive) {
    EXPECT_TRUE(CTimer::GetCyclesPerMillisecond() > 0);
}

GAME_TEST(CTimer, CyclesPerFrame_Is20xCyclesPerMS) {
    // GetCyclesPerFrame = (float)divider * 20.0f
    uint32 cpm = CTimer::GetCyclesPerMillisecond();
    uint32 cpf = CTimer::GetCyclesPerFrame();
    EXPECT_EQ(cpf, (uint32)((float)cpm * 20.0f));
}

GAME_TEST(CTimer, PauseStateConsistency) {
    // GetIsPaused should be true iff either UserPause or CodePause
    bool paused = CTimer::GetIsPaused();
    bool user = CTimer::GetIsUserPaused();
    bool code = CTimer::GetIsCodePaused();
    EXPECT_EQ(paused, user || code);
}

GAME_TEST(CTimer, Constants) {
    EXPECT_NEAR(CTimer::TIMESTEP_PER_SECOND, 50.0f, 1e-6f);
    EXPECT_NEAR(CTimer::TIMESTEP_LEN_IN_MS, 20.0f, 1e-6f);
}
