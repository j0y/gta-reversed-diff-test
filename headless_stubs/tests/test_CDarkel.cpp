// test_CDarkel.cpp — Differential tests for CDarkel (rampage/kill frenzy).
// Hook paths: Global/CDarkel/<fn>

#include "StdInc.h"
#include "TestFramework.h"

GAME_DIFF_TEST(CDarkel, FrenzyOnGoing) {
    bool orig, rev;
    { HookDisableGuard guard("Global/CDarkel/FrenzyOnGoing"); orig = CDarkel::FrenzyOnGoing(); }
    rev = CDarkel::FrenzyOnGoing();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CDarkel, ReadStatus) {
    // BUG: eDarkelStatus is `enum class` (4 bytes) in reversed code,
    // but original binary reads only 1 byte from 0x96A704.
    // Reversed StaticRef<eDarkelStatus> picks up garbage in upper bytes.
    // Fix: eDarkelStatus should be `enum class eDarkelStatus : uint8`.
    eDarkelStatus orig, rev;
    { HookDisableGuard guard("Global/CDarkel/ReadStatus"); orig = CDarkel::ReadStatus(); }
    rev = CDarkel::ReadStatus();
    EXPECT_EQ((int)orig, (int)rev);
}

GAME_DIFF_TEST(CDarkel, CalcFade) {
    // CalcFade(t, begin, end) — pure function, no state dependency
    struct Case { uint32 t, begin, end; };
    Case cases[] = { {0,0,100}, {50,0,100}, {100,0,100}, {0,50,100}, {200,0,100}, {75,50,100} };
    for (auto& c : cases) {
        uint8 orig, rev;
        { HookDisableGuard guard("Global/CDarkel/CalcFade"); orig = CDarkel::CalcFade(c.t, c.begin, c.end); }
        rev = CDarkel::CalcFade(c.t, c.begin, c.end);
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(CDarkel, FindTotalPedsKilledByPlayer) {
    int32 orig, rev;
    { HookDisableGuard guard("Global/CDarkel/FindTotalPedsKilledByPlayer"); orig = CDarkel::FindTotalPedsKilledByPlayer(0); }
    rev = CDarkel::FindTotalPedsKilledByPlayer(0);
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CDarkel, QueryModelsKilledByPlayer) {
    // Test a few model IDs
    for (int modelId = 0; modelId < 10; modelId++) {
        int16 orig, rev;
        { HookDisableGuard guard("Global/CDarkel/QueryModelsKilledByPlayer");
          orig = CDarkel::QueryModelsKilledByPlayer((eModelID)modelId, 0); }
        rev = CDarkel::QueryModelsKilledByPlayer((eModelID)modelId, 0);
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(CDarkel, CheckDamagedWeaponType) {
    eWeaponType weapons[] = { WEAPON_UNARMED, WEAPON_PISTOL, WEAPON_AK47, WEAPON_SHOTGUN };
    for (auto expected : weapons) {
        for (auto actual : weapons) {
            bool orig, rev;
            { HookDisableGuard guard("Global/CDarkel/CheckDamagedWeaponType");
              orig = CDarkel::CheckDamagedWeaponType(actual, expected); }
            rev = CDarkel::CheckDamagedWeaponType(actual, expected);
            EXPECT_EQ(orig, rev);
        }
    }
}
