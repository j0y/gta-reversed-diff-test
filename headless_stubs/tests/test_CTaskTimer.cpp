// test_CTaskTimer.cpp — Tests for CTaskTimer.
// Timer utility used by many task classes.

#include "StdInc.h"
#include "TestFramework.h"
#include "Tasks/TaskTimer.h"

GAME_TEST(CTaskTimer, DefaultConstructor) {
    CTaskTimer timer;
    EXPECT_FALSE(timer.IsStarted());
}

GAME_TEST(CTaskTimer, ParameterizedConstructor) {
    CTaskTimer timer(1000, 5000);
    EXPECT_EQ(timer.m_nStartTime, (uint32)1000);
    EXPECT_EQ(timer.m_nInterval, (uint32)5000);
}

GAME_TEST(CTaskTimer, Start) {
    CTaskTimer timer;
    bool started = timer.Start(3000);
    EXPECT_TRUE(started);
    EXPECT_TRUE(timer.IsStarted());
    EXPECT_EQ(timer.m_nInterval, (uint32)3000);
}

GAME_TEST(CTaskTimer, StartIfNotAlready_FirstCall) {
    CTaskTimer timer;
    timer.StartIfNotAlready(2000);
    EXPECT_TRUE(timer.IsStarted());
}

GAME_TEST(CTaskTimer, StartIfNotAlready_SecondCall) {
    CTaskTimer timer;
    timer.StartIfNotAlready(2000);
    uint32 firstStart = timer.m_nStartTime;
    timer.StartIfNotAlready(5000); // should not change
    EXPECT_EQ(timer.m_nStartTime, firstStart);
    EXPECT_EQ(timer.m_nInterval, (uint32)2000); // interval from first call
}

GAME_TEST(CTaskTimer, Stop) {
    CTaskTimer timer;
    timer.Start(1000);
    EXPECT_TRUE(timer.IsStarted());
    timer.Stop();
    EXPECT_FALSE(timer.m_bStarted);
}

GAME_TEST(CTaskTimer, SetAsOutOfTime) {
    CTaskTimer timer;
    timer.Start(5000);
    timer.SetAsOutOfTime();
    EXPECT_TRUE(timer.IsOutOfTime());
}

GAME_TEST(CTaskTimer, Reset) {
    CTaskTimer timer;
    timer.Start(1000);
    bool wasOutOfTime = timer.Reset();
    // Reset restarts the timer — returns whether it was out of time
    (void)wasOutOfTime;
    EXPECT_TRUE(timer.IsStarted());
}
