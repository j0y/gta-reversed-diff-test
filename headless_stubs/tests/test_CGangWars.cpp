// test_CGangWars.cpp — Differential tests for CGangWars.
// Hook paths: Global/CGangWars/<fn>

#include "StdInc.h"
#include "TestFramework.h"

GAME_DIFF_TEST(CGangWars, GangWarGoingOn) {
    bool orig, rev;
    { HookDisableGuard guard("Global/CGangWars/GangWarGoingOn"); orig = CGangWars::GangWarGoingOn(); }
    rev = CGangWars::GangWarGoingOn();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CGangWars, GangWarFightingGoingOn) {
    bool orig, rev;
    { HookDisableGuard guard("Global/CGangWars/GangWarFightingGoingOn"); orig = CGangWars::GangWarFightingGoingOn(); }
    rev = CGangWars::GangWarFightingGoingOn();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CGangWars, AttackWaveOvercome) {
    bool orig, rev;
    { HookDisableGuard guard("Global/CGangWars/AttackWaveOvercome"); orig = CGangWars::AttackWaveOvercome(); }
    rev = CGangWars::AttackWaveOvercome();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CGangWars, DontCreateCivilians) {
    bool orig, rev;
    { HookDisableGuard guard("Global/CGangWars/DontCreateCivilians"); orig = CGangWars::DontCreateCivilians(); }
    rev = CGangWars::DontCreateCivilians();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CGangWars, CalculateTimeTillNextAttack) {
    float orig, rev;
    { HookDisableGuard guard("Global/CGangWars/CalculateTimeTillNextAttack"); orig = CGangWars::CalculateTimeTillNextAttack(); }
    rev = CGangWars::CalculateTimeTillNextAttack();
    EXPECT_NEAR(orig, rev, 1e-5f);
}
