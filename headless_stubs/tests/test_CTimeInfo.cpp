// test_CTimeInfo.cpp — Differential tests for CTimeInfo.
// Hook paths: Models/CTimeInfo/<fn>
//
// Time-of-day model info: GetTimeOn/Off, SetTimes, SetOtherTimeModel.
// Works on 'this' member fields, no global state for basic accessors.

#include "StdInc.h"
#include "TestFramework.h"
#include "Models/TimeInfo.h"

// --- SetTimes / GetTimeOn / GetTimeOff ---

GAME_DIFF_TEST(CTimeInfo, SetAndGetTimes) {
    struct Case { uint8 on, off; };
    Case cases[] = {
        {0, 12}, {6, 22}, {0, 24}, {20, 6}, {12, 12}, {0, 0}, {23, 1},
    };
    for (auto& c : cases) {
        CTimeInfo orig{}, rev{};
        { HookDisableGuard guard("Models/CTimeInfo/SetTimes");
          orig.SetTimes(c.on, c.off); }
        rev.SetTimes(c.on, c.off);

        uint8 origOn, revOn, origOff, revOff;
        { HookDisableGuard guard("Models/CTimeInfo/GetTimeOn");
          origOn = orig.GetTimeOn(); }
        revOn = rev.GetTimeOn();
        { HookDisableGuard guard("Models/CTimeInfo/GetTimeOff");
          origOff = orig.GetTimeOff(); }
        revOff = rev.GetTimeOff();

        EXPECT_EQ(origOn, revOn);
        EXPECT_EQ(origOff, revOff);
    }
}

// --- SetOtherTimeModel / GetOtherTimeModel ---

GAME_DIFF_TEST(CTimeInfo, SetAndGetOtherTimeModel) {
    int16 models[] = { -1, 0, 100, 500, 32767 };
    for (int16 m : models) {
        CTimeInfo orig{}, rev{};
        { HookDisableGuard guard("Models/CTimeInfo/SetOtherTimeModel");
          orig.SetOtherTimeModel(m); }
        rev.SetOtherTimeModel(m);

        int16 origM, revM;
        { HookDisableGuard guard("Models/CTimeInfo/GetOtherTimeModel");
          origM = orig.GetOtherTimeModel(); }
        revM = rev.GetOtherTimeModel();

        EXPECT_EQ(origM, revM);
        EXPECT_EQ(revM, m);
    }
}

// --- Behavior tests ---

GAME_TEST(CTimeInfo, DefaultState) {
    CTimeInfo ti{};
    ti.SetTimes(6, 22);
    EXPECT_EQ(ti.GetTimeOn(), (uint8)6);
    EXPECT_EQ(ti.GetTimeOff(), (uint8)22);
}
