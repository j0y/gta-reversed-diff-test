// test_CGangWars2.cpp — Deep differential tests for CGangWars.
// Hook paths: Global/CGangWars/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "GangWars.h"

GAME_DIFF_TEST(CGangWars2, GangWarGoingOn) {
    bool orig, rev;
    { HookDisableGuard guard("Global/CGangWars/GangWarGoingOn"); orig = CGangWars::GangWarGoingOn(); }
    rev = CGangWars::GangWarGoingOn();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CGangWars2, GangWarFightingGoingOn) {
    bool orig, rev;
    { HookDisableGuard guard("Global/CGangWars/GangWarFightingGoingOn"); orig = CGangWars::GangWarFightingGoingOn(); }
    rev = CGangWars::GangWarFightingGoingOn();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CGangWars2, DontCreateCivilians) {
    bool orig, rev;
    { HookDisableGuard guard("Global/CGangWars/DontCreateCivilians"); orig = CGangWars::DontCreateCivilians(); }
    rev = CGangWars::DontCreateCivilians();
    EXPECT_EQ(orig, rev);
}

// DoesPlayerControlThisZone takes CZoneInfo* — skip (need valid zone pointer)

GAME_DIFF_TEST(CGangWars2, CalculateTimeTillNextAttack) {
    float orig, rev;
    // Save state
    float saved = CGangWars::TimeTillNextAttack;
    { HookDisableGuard guard("Global/CGangWars/CalculateTimeTillNextAttack");
      CGangWars::CalculateTimeTillNextAttack();
      orig = CGangWars::TimeTillNextAttack; }
    CGangWars::TimeTillNextAttack = saved; // restore
    CGangWars::CalculateTimeTillNextAttack();
    rev = CGangWars::TimeTillNextAttack;
    EXPECT_NEAR(orig, rev, 1e-3f);
    CGangWars::TimeTillNextAttack = saved; // restore
}
