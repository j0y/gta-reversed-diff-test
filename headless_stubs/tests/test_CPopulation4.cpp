// test_CPopulation4.cpp — Extended differential tests for CPopulation.
// Hook paths: Global/CPopulation/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "Population.h"

// --- FindPedRaceFromName ---

GAME_DIFF_TEST(CPopulation4, FindPedRaceFromName_KnownRaces) {
    const char* names[] = { "white", "black", "hispanic", "oriental", "arabic" };
    for (auto* name : names) {
        int32 orig, rev;
        { HookDisableGuard guard("Global/CPopulation/FindPedRaceFromName");
          orig = (int32)CPopulation::FindPedRaceFromName(name); }
        rev = (int32)CPopulation::FindPedRaceFromName(name);
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(CPopulation4, FindPedRaceFromName_Invalid) {
    int32 orig, rev;
    { HookDisableGuard guard("Global/CPopulation/FindPedRaceFromName");
      orig = (int32)CPopulation::FindPedRaceFromName("nonexistent"); }
    rev = (int32)CPopulation::FindPedRaceFromName("nonexistent");
    EXPECT_EQ(orig, rev);
}

// --- ArePedStatsCompatible ---

GAME_DIFF_TEST(CPopulation4, ArePedStatsCompatible_Pairs) {
    struct Pair { int32 a, b; };
    Pair pairs[] = { {0,0}, {0,1}, {1,2}, {3,5}, {7,7}, {0,10}, {15,15} };
    for (auto& p : pairs) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CPopulation/ArePedStatsCompatible");
          orig = CPopulation::ArePedStatsCompatible(static_cast<ePedStats>(p.a), static_cast<ePedStats>(p.b)); }
        rev = CPopulation::ArePedStatsCompatible(static_cast<ePedStats>(p.a), static_cast<ePedStats>(p.b));
        EXPECT_EQ(orig, rev);
    }
}

// --- DoesCarGroupHaveModelId ---

GAME_DIFF_TEST(CPopulation4, DoesCarGroupHaveModelId_PoliceInGroups) {
    for (int32 group = 0; group < 10; group++) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CPopulation/DoesCarGroupHaveModelId");
          orig = CPopulation::DoesCarGroupHaveModelId(group, 596); }
        rev = CPopulation::DoesCarGroupHaveModelId(group, 596);
        EXPECT_EQ(orig, rev);
    }
}

// --- IsSecurityGuard (takes ePedType) ---

GAME_DIFF_TEST(CPopulation4, IsSecurityGuard_AllTypes) {
    for (int32 t = 0; t < 32; t++) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CPopulation/IsSecurityGuard");
          orig = CPopulation::IsSecurityGuard(static_cast<ePedType>(t)); }
        rev = CPopulation::IsSecurityGuard(static_cast<ePedType>(t));
        EXPECT_EQ(orig, rev);
    }
}
