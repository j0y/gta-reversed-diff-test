// test_CDecisionMaker.cpp — Differential tests for CDecisionMakerTypesFileLoader.
// Hook paths: DecisionMakers/CDecisionMakerTypesFileLoader/<fn>
// Note: Index 0 may be valid but higher indices may OOB. Test conservatively.

#include "StdInc.h"
#include "TestFramework.h"
#include "DecisionMakers/DecisionMakerTypesFileLoader.h"

GAME_DIFF_TEST(CDecisionMaker, GetPedDMName_Index0) {
    char origName[128] = {};
    char revName[128] = {};
    { HookDisableGuard guard("DecisionMakers/CDecisionMakerTypesFileLoader/GetPedDMName");
      CDecisionMakerTypesFileLoader::GetPedDMName(0, origName); }
    CDecisionMakerTypesFileLoader::GetPedDMName(0, revName);
    EXPECT_EQ(strcmp(origName, revName), 0);
}

GAME_DIFF_TEST(CDecisionMaker, GetGrpDMName_Index0) {
    char origName[128] = {};
    char revName[128] = {};
    { HookDisableGuard guard("DecisionMakers/CDecisionMakerTypesFileLoader/GetGrpDMName");
      CDecisionMakerTypesFileLoader::GetGrpDMName(0, origName); }
    CDecisionMakerTypesFileLoader::GetGrpDMName(0, revName);
    EXPECT_EQ(strcmp(origName, revName), 0);
}
