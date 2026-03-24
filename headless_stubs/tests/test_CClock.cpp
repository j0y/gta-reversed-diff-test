// test_CClock.cpp — Differential tests for CClock.
// Hook paths: Global/CClock/<fn>

#include "StdInc.h"
#include "TestFramework.h"

GAME_DIFF_TEST(CClock, GetGameClockMinutesUntil) {
    // Test various hour/minute targets
    struct Case { uint8 h; uint8 m; };
    Case cases[] = { {0,0}, {6,0}, {12,0}, {18,0}, {23,59}, {12,30} };
    for (auto& c : cases) {
        uint16 orig, rev;
        { HookDisableGuard guard("Global/CClock/GetGameClockMinutesUntil");
          orig = CClock::GetGameClockMinutesUntil(c.h, c.m); }
        rev = CClock::GetGameClockMinutesUntil(c.h, c.m);
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(CClock, GetIsTimeInRange) {
    struct Case { uint8 from; uint8 to; };
    Case cases[] = { {6,18}, {18,6}, {0,0}, {12,12}, {0,24}, {23,1} };
    for (auto& c : cases) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CClock/GetIsTimeInRange");
          orig = CClock::GetIsTimeInRange(c.from, c.to); }
        rev = CClock::GetIsTimeInRange(c.from, c.to);
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(CClock, NormaliseGameClock) {
    // Save clock state
    auto savedH = CClock::ms_nGameClockHours;
    auto savedM = CClock::ms_nGameClockMinutes;
    auto savedD = CClock::ms_nGameClockDays;

    auto testCase = [&](uint8 h, uint8 m) {
        // Original
        CClock::ms_nGameClockHours = h;
        CClock::ms_nGameClockMinutes = m;
        CClock::ms_nGameClockDays = 1;
        { HookDisableGuard guard("Global/CClock/NormaliseGameClock"); CClock::NormaliseGameClock(); }
        auto origH = CClock::ms_nGameClockHours;
        auto origM = CClock::ms_nGameClockMinutes;
        auto origD = CClock::ms_nGameClockDays;

        // Reversed
        CClock::ms_nGameClockHours = h;
        CClock::ms_nGameClockMinutes = m;
        CClock::ms_nGameClockDays = 1;
        CClock::NormaliseGameClock();
        auto revH = CClock::ms_nGameClockHours;
        auto revM = CClock::ms_nGameClockMinutes;
        auto revD = CClock::ms_nGameClockDays;

        EXPECT_EQ(origH, revH);
        EXPECT_EQ(origM, revM);
        EXPECT_EQ(origD, revD);
    };

    testCase(0, 0);
    testCase(23, 59);
    testCase(24, 0);    // should wrap to 0:00 next day
    testCase(25, 60);   // should normalize both
    testCase(0, 120);   // minutes overflow

    // Restore
    CClock::ms_nGameClockHours = savedH;
    CClock::ms_nGameClockMinutes = savedM;
    CClock::ms_nGameClockDays = savedD;
}

// --- StoreClock / RestoreClock round-trip ---

GAME_DIFF_TEST(CClock, StoreClock_RestoreClock) {
    auto savedH = CClock::ms_nGameClockHours;
    auto savedM = CClock::ms_nGameClockMinutes;

    // Store via original
    { HookDisableGuard guard("Global/CClock/StoreClock");
      CClock::StoreClock(); }

    // Change clock
    CClock::ms_nGameClockHours = 12;
    CClock::ms_nGameClockMinutes = 30;

    // Restore via original
    { HookDisableGuard guard("Global/CClock/RestoreClock");
      CClock::RestoreClock(); }
    auto origH = CClock::ms_nGameClockHours;
    auto origM = CClock::ms_nGameClockMinutes;

    // Now do the same with reversed code
    CClock::ms_nGameClockHours = savedH;
    CClock::ms_nGameClockMinutes = savedM;
    CClock::StoreClock();
    CClock::ms_nGameClockHours = 12;
    CClock::ms_nGameClockMinutes = 30;
    CClock::RestoreClock();
    auto revH = CClock::ms_nGameClockHours;
    auto revM = CClock::ms_nGameClockMinutes;

    EXPECT_EQ(origH, revH);
    EXPECT_EQ(origM, revM);

    // Final restore
    CClock::ms_nGameClockHours = savedH;
    CClock::ms_nGameClockMinutes = savedM;
}

// --- SetGameClock ---

GAME_DIFF_TEST(CClock, SetGameClock) {
    auto savedH = CClock::ms_nGameClockHours;
    auto savedM = CClock::ms_nGameClockMinutes;
    auto savedD = CClock::ms_nGameClockDays;

    { HookDisableGuard guard("Global/CClock/SetGameClock");
      CClock::SetGameClock(15, 45, 5); }
    auto origH = CClock::ms_nGameClockHours;
    auto origM = CClock::ms_nGameClockMinutes;

    CClock::ms_nGameClockHours = savedH;
    CClock::ms_nGameClockMinutes = savedM;
    CClock::ms_nGameClockDays = savedD;
    CClock::SetGameClock(15, 45, 5);
    auto revH = CClock::ms_nGameClockHours;
    auto revM = CClock::ms_nGameClockMinutes;

    EXPECT_EQ(origH, revH);
    EXPECT_EQ(origM, revM);

    CClock::ms_nGameClockHours = savedH;
    CClock::ms_nGameClockMinutes = savedM;
    CClock::ms_nGameClockDays = savedD;
}
