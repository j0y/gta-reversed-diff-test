// test_CPopulation3.cpp — Extended differential tests for CPopulation.
// Hook paths: Global/CPopulation/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "Population.h"

// --- IsSunbather ---

// IsSunbather hook not found at runtime — likely different path or not hooked

// --- IsSkateable ---

GAME_DIFF_TEST(CPopulation3, IsSkateable_Various) {
    CVector positions[] = {
        CVector(2495.0f, -1665.0f, 13.5f),
        CVector(0.0f, 0.0f, 10.0f),
        CVector(1000.0f, -1000.0f, 20.0f),
    };
    for (auto& pos : positions) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CPopulation/IsSkateable"); orig = CPopulation::IsSkateable(pos); }
        rev = CPopulation::IsSkateable(pos);
        EXPECT_EQ(orig, rev);
    }
}
