// test_CPopulation.cpp — Differential tests for CPopulation.
// Hook paths: Global/CPopulation/<fn>

#include "StdInc.h"
#include "TestFramework.h"

GAME_DIFF_TEST(CPopulation, IsMale) {
    // Test a range of model IDs
    eModelID models[] = { (eModelID)0, (eModelID)7, (eModelID)50, (eModelID)100, (eModelID)102, (eModelID)200, (eModelID)280 };
    for (auto m : models) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CPopulation/IsMale"); orig = CPopulation::IsMale(m); }
        rev = CPopulation::IsMale(m);
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(CPopulation, IsFemale) {
    eModelID models[] = { (eModelID)0, (eModelID)7, (eModelID)50, (eModelID)100, (eModelID)150, (eModelID)200, (eModelID)280 };
    for (auto m : models) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CPopulation/IsFemale"); orig = CPopulation::IsFemale(m); }
        rev = CPopulation::IsFemale(m);
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(CPopulation, ChoosePolicePedOccupation) {
    eModelID orig, rev;
    { HookDisableGuard guard("Global/CPopulation/ChoosePolicePedOccupation"); orig = CPopulation::ChoosePolicePedOccupation(); }
    rev = CPopulation::ChoosePolicePedOccupation();
    EXPECT_EQ((int)orig, (int)rev);
}

GAME_DIFF_TEST(CPopulation, PedCreationDistMultiplier) {
    float orig, rev;
    { HookDisableGuard guard("Global/CPopulation/PedCreationDistMultiplier"); orig = CPopulation::PedCreationDistMultiplier(); }
    rev = CPopulation::PedCreationDistMultiplier();
    EXPECT_NEAR(orig, rev, 1e-5f);
}

GAME_DIFF_TEST(CPopulation, FindPedDensityMultiplierCullZone) {
    float orig, rev;
    { HookDisableGuard guard("Global/CPopulation/FindPedDensityMultiplierCullZone"); orig = CPopulation::FindPedDensityMultiplierCullZone(); }
    rev = CPopulation::FindPedDensityMultiplierCullZone();
    EXPECT_NEAR(orig, rev, 1e-5f);
}

GAME_DIFF_TEST(CPopulation, DoesCarGroupHaveModelId) {
    for (int group = 0; group < 5; group++) {
        for (int model = 400; model < 410; model++) {
            bool orig, rev;
            { HookDisableGuard guard("Global/CPopulation/DoesCarGroupHaveModelId");
              orig = CPopulation::DoesCarGroupHaveModelId(group, model); }
            rev = CPopulation::DoesCarGroupHaveModelId(group, model);
            EXPECT_EQ(orig, rev);
        }
    }
}
