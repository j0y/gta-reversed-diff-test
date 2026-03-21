// test_CPedClothesDesc.cpp — Differential tests for CPedClothesDesc queries.
// Hook paths: Global/CPedClothesDesc/<fn>
// Player clothes state queries — balaclava, haircut, tattoo visibility.

#include "StdInc.h"
#include "TestFramework.h"
#include "PedClothesDesc.h"
#include "Entity/Ped/Ped.h"

static CPedClothesDesc* GetPlayerClothes() {
    auto* ped = FindPlayerPed(0);
    if (!ped) return nullptr;
    return ped->GetClothesDesc();
}

GAME_DIFF_TEST(CPedClothesDesc, GetIsWearingBalaclava) {
    auto* clothes = GetPlayerClothes();
    if (!clothes) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No player clothes desc"); return; }
    bool orig, rev;
    { HookDisableGuard guard("Global/CPedClothesDesc/GetIsWearingBalaclava");
      orig = clothes->GetIsWearingBalaclava(); }
    rev = clothes->GetIsWearingBalaclava();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CPedClothesDesc, HasVisibleTattoo) {
    auto* clothes = GetPlayerClothes();
    if (!clothes) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No player clothes desc"); return; }
    bool orig, rev;
    { HookDisableGuard guard("Global/CPedClothesDesc/HasVisibleTattoo");
      orig = clothes->HasVisibleTattoo(); }
    rev = clothes->HasVisibleTattoo();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CPedClothesDesc, HasVisibleNewHairCut) {
    auto* clothes = GetPlayerClothes();
    if (!clothes) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No player clothes desc"); return; }
    // Test with part index 0
    bool orig, rev;
    { HookDisableGuard guard("Global/CPedClothesDesc/HasVisibleNewHairCut");
      orig = clothes->HasVisibleNewHairCut(0); }
    rev = clothes->HasVisibleNewHairCut(0);
    EXPECT_EQ(orig, rev);
}
