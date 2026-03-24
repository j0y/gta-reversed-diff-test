// test_CLocalisation.cpp — Differential tests for CLocalisation.
// Hook paths: Global/CLocalisation/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "Localisation.h"

GAME_DIFF_TEST(CLocalisation, GermanGame) {
    bool orig, rev;
    { HookDisableGuard guard("Global/CLocalisation/GermanGame");
      orig = CLocalisation::GermanGame(); }
    rev = CLocalisation::GermanGame();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CLocalisation, FrenchGame) {
    bool orig, rev;
    { HookDisableGuard guard("Global/CLocalisation/FrenchGame");
      orig = CLocalisation::FrenchGame(); }
    rev = CLocalisation::FrenchGame();
    EXPECT_EQ(orig, rev);
}
