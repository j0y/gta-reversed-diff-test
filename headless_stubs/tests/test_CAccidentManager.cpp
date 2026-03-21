// test_CAccidentManager.cpp — Differential tests for CAccidentManager.
// Hook paths: Global/CAccidentManager/<fn>
// CAccidentManager tracks ped injuries for medic AI response.

#include "StdInc.h"
#include "TestFramework.h"
#include "AccidentManager.h"

GAME_DIFF_TEST(CAccidentManager, GetNumberOfFreeAccidents) {
    uint32 orig, rev;
    { HookDisableGuard guard("Global/CAccidentManager/GetNumberOfFreeAccidents");
      orig = CAccidentManager::GetInstance()->GetNumberOfFreeAccidents(); }
    rev = CAccidentManager::GetInstance()->GetNumberOfFreeAccidents();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CAccidentManager, GetNearestFreeAccident_GroveStreet) {
    CVector pos(2488.f, -1666.f, 13.f);
    CAccident* orig;
    CAccident* rev;
    { HookDisableGuard guard("Global/CAccidentManager/GetNearestFreeAccident");
      orig = CAccidentManager::GetInstance()->GetNearestFreeAccident(pos, false); }
    rev = CAccidentManager::GetInstance()->GetNearestFreeAccident(pos, false);
    EXPECT_EQ(orig, rev);
}
