// test_CLocalisation2.cpp — Differential tests for CLocalisation bool getters.
// Hook paths: Global/CLocalisation/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "Localisation.h"

GAME_DIFF_TEST(CLocalisation, Metric) {
    bool orig, rev;
    { HookDisableGuard guard("Global/CLocalisation/Metric");
      orig = CLocalisation::Metric(); }
    rev = CLocalisation::Metric();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CLocalisation, Blood) {
    bool orig, rev;
    { HookDisableGuard guard("Global/CLocalisation/Blood");
      orig = CLocalisation::Blood(); }
    rev = CLocalisation::Blood();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CLocalisation, Porn) {
    bool orig, rev;
    { HookDisableGuard guard("Global/CLocalisation/Porn");
      orig = CLocalisation::Porn(); }
    rev = CLocalisation::Porn();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CLocalisation, ScreamsFromKills) {
    bool orig, rev;
    { HookDisableGuard guard("Global/CLocalisation/ScreamsFromKills");
      orig = CLocalisation::ScreamsFromKills(); }
    rev = CLocalisation::ScreamsFromKills();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CLocalisation, Prostitutes) {
    bool orig, rev;
    { HookDisableGuard guard("Global/CLocalisation/Prostitutes");
      orig = CLocalisation::Prostitutes(); }
    rev = CLocalisation::Prostitutes();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CLocalisation, KickingWhenDown) {
    bool orig, rev;
    { HookDisableGuard guard("Global/CLocalisation/KickingWhenDown");
      orig = CLocalisation::KickingWhenDown(); }
    rev = CLocalisation::KickingWhenDown();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CLocalisation, ShootLimbs) {
    bool orig, rev;
    { HookDisableGuard guard("Global/CLocalisation/ShootLimbs");
      orig = CLocalisation::ShootLimbs(); }
    rev = CLocalisation::ShootLimbs();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CLocalisation, KnockDownPeds) {
    bool orig, rev;
    { HookDisableGuard guard("Global/CLocalisation/KnockDownPeds");
      orig = CLocalisation::KnockDownPeds(); }
    rev = CLocalisation::KnockDownPeds();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CLocalisation, KillFrenzy) {
    bool orig, rev;
    { HookDisableGuard guard("Global/CLocalisation/KillFrenzy");
      orig = CLocalisation::KillFrenzy(); }
    rev = CLocalisation::KillFrenzy();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CLocalisation, StealFromDeadPed) {
    bool orig, rev;
    { HookDisableGuard guard("Global/CLocalisation/StealFromDeadPed");
      orig = CLocalisation::StealFromDeadPed(); }
    rev = CLocalisation::StealFromDeadPed();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CLocalisation, KillPeds) {
    bool orig, rev;
    { HookDisableGuard guard("Global/CLocalisation/KillPeds");
      orig = CLocalisation::KillPeds(); }
    rev = CLocalisation::KillPeds();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CLocalisation, PedsOnFire) {
    bool orig, rev;
    { HookDisableGuard guard("Global/CLocalisation/PedsOnFire");
      orig = CLocalisation::PedsOnFire(); }
    rev = CLocalisation::PedsOnFire();
    EXPECT_EQ(orig, rev);
}
