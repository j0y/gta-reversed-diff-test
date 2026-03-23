// test_CDarkel2.cpp — Extended differential tests for CDarkel.
// Hook paths: Global/CDarkel/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "Darkel.h"
#include "Entity/Ped/Ped.h"

// --- CalcFade ---

GAME_DIFF_TEST(CDarkel2, CalcFade_Various) {
    struct Case { uint32 t; uint32 begin; uint32 end; };
    Case cases[] = {
        {0, 0, 100}, {50, 0, 100}, {100, 0, 100},
        {0, 0, 0}, {500, 100, 1000}, {1000, 100, 1000},
        {50, 100, 200}, {150, 100, 200}, {200, 100, 200},
    };
    for (auto& c : cases) {
        uint8 orig, rev;
        { HookDisableGuard guard("Global/CDarkel/CalcFade"); orig = CDarkel::CalcFade(c.t, c.begin, c.end); }
        rev = CDarkel::CalcFade(c.t, c.begin, c.end);
        EXPECT_EQ(orig, rev);
    }
}

// --- FindTotalPedsKilledByPlayer ---

GAME_DIFF_TEST(CDarkel2, FindTotalPedsKilledByPlayer) {
    int32 orig, rev;
    { HookDisableGuard guard("Global/CDarkel/FindTotalPedsKilledByPlayer"); orig = CDarkel::FindTotalPedsKilledByPlayer(0); }
    rev = CDarkel::FindTotalPedsKilledByPlayer(0);
    EXPECT_EQ(orig, rev);
}

// --- CheckDamagedWeaponType ---

GAME_DIFF_TEST(CDarkel2, CheckDamagedWeaponType_Various) {
    eWeaponType weapons[] = { WEAPON_UNARMED, WEAPON_PISTOL, WEAPON_SHOTGUN, WEAPON_AK47, WEAPON_ROCKET, WEAPON_FLAMETHROWER };
    for (auto expected : weapons) {
        for (auto actual : weapons) {
            bool orig, rev;
            { HookDisableGuard guard("Global/CDarkel/CheckDamagedWeaponType"); orig = CDarkel::CheckDamagedWeaponType(actual, expected); }
            rev = CDarkel::CheckDamagedWeaponType(actual, expected);
            EXPECT_EQ(orig, rev);
        }
    }
}

// --- ThisPedShouldBeKilledForFrenzy ---

GAME_DIFF_TEST(CDarkel2, ThisPedShouldBeKilledForFrenzy_Player) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    bool orig, rev;
    { HookDisableGuard guard("Global/CDarkel/ThisPedShouldBeKilledForFrenzy"); orig = CDarkel::ThisPedShouldBeKilledForFrenzy(*player); }
    rev = CDarkel::ThisPedShouldBeKilledForFrenzy(*player);
    EXPECT_EQ(orig, rev);
}
