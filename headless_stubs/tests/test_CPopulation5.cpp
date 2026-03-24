// test_CPopulation5.cpp — Extended differential tests for CPopulation.
// Hook paths: Global/CPopulation/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "Population.h"

GAME_DIFF_TEST(CPopulation5, ChoosePolicePedOccupation) {
    int32 orig, rev;
    { HookDisableGuard guard("Global/CPopulation/ChoosePolicePedOccupation");
      orig = (int32)CPopulation::ChoosePolicePedOccupation(); }
    rev = (int32)CPopulation::ChoosePolicePedOccupation();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CPopulation5, ChooseGangOccupation_AllGangs) {
    for (int32 gang = 0; gang < 10; gang++) {
        int32 orig, rev;
        { HookDisableGuard guard("Global/CPopulation/ChooseGangOccupation");
          orig = (int32)CPopulation::ChooseGangOccupation(static_cast<eGangID>(gang)); }
        rev = (int32)CPopulation::ChooseGangOccupation(static_cast<eGangID>(gang));
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(CPopulation5, FindCarMultiplierMotorway) {
    float orig, rev;
    { HookDisableGuard guard("Global/CPopulation/FindCarMultiplierMotorway");
      orig = CPopulation::FindCarMultiplierMotorway(); }
    rev = CPopulation::FindCarMultiplierMotorway();
    EXPECT_NEAR(orig, rev, 1e-5f);
}

GAME_DIFF_TEST(CPopulation5, FindPedDensityMultiplierCullZone) {
    float orig, rev;
    { HookDisableGuard guard("Global/CPopulation/FindPedDensityMultiplierCullZone");
      orig = CPopulation::FindPedDensityMultiplierCullZone(); }
    rev = CPopulation::FindPedDensityMultiplierCullZone();
    EXPECT_NEAR(orig, rev, 1e-5f);
}

GAME_DIFF_TEST(CPopulation5, FindPedMultiplierMotorway) {
    float orig, rev;
    { HookDisableGuard guard("Global/CPopulation/FindPedMultiplierMotorway");
      orig = CPopulation::FindPedMultiplierMotorway(); }
    rev = CPopulation::FindPedMultiplierMotorway();
    EXPECT_NEAR(orig, rev, 1e-5f);
}

GAME_DIFF_TEST(CPopulation5, PedMICanBeCreatedAtAttractor) {
    eModelID models[] = { MODEL_MALE01, MODEL_HFYST, MODEL_BFYST, MODEL_WFYST };
    for (auto m : models) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CPopulation/PedMICanBeCreatedAtAttractor");
          orig = CPopulation::PedMICanBeCreatedAtAttractor(m); }
        rev = CPopulation::PedMICanBeCreatedAtAttractor(m);
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(CPopulation5, PedMICanBeCreatedInInterior) {
    eModelID models[] = { MODEL_MALE01, MODEL_HFYST, MODEL_BFYST, MODEL_WFYST };
    for (auto m : models) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CPopulation/PedMICanBeCreatedInInterior");
          orig = CPopulation::PedMICanBeCreatedInInterior(m); }
        rev = CPopulation::PedMICanBeCreatedInInterior(m);
        EXPECT_EQ(orig, rev);
    }
}
