// test_CAccidentManager2.cpp — Extended differential tests for CAccidentManager.
// Hook paths: Global/CAccidentManager/<fn>
//
// Tests accident manager queries at various positions.

#include "StdInc.h"
#include "TestFramework.h"
#include "AccidentManager.h"

// --- GetNearestFreeAccident at various positions ---

GAME_DIFF_TEST(CAccidentManager, GetNearestFreeAccident_MultiplePositions) {
    CVector positions[] = {
        {2488.f, -1666.f, 13.f},    // Grove Street
        {0.f, 0.f, 0.f},            // Origin
        {-2170.f, 250.f, 30.f},     // San Fierro
        {2027.f, 1008.f, 20.f},     // Las Venturas
    };
    auto* mgr = CAccidentManager::GetInstance();
    if (!mgr) return;
    for (auto& pos : positions) {
        CAccident* orig;
        CAccident* rev;
        { HookDisableGuard guard("Global/CAccidentManager/GetNearestFreeAccident");
          orig = mgr->GetNearestFreeAccident(pos, false); }
        rev = mgr->GetNearestFreeAccident(pos, false);
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(CAccidentManager, GetNearestFreeAccident_IgnoreHeadless) {
    CVector pos(2488.f, -1666.f, 13.f);
    auto* mgr = CAccidentManager::GetInstance();
    if (!mgr) return;
    CAccident* orig;
    CAccident* rev;
    { HookDisableGuard guard("Global/CAccidentManager/GetNearestFreeAccident");
      orig = mgr->GetNearestFreeAccident(pos, true); }
    rev = mgr->GetNearestFreeAccident(pos, true);
    EXPECT_EQ(orig, rev);
}

// --- GetNearestFreeAccidentExceptThisOne ---

GAME_DIFF_TEST(CAccidentManager, GetNearestFreeAccidentExceptThisOne) {
    CVector pos(2488.f, -1666.f, 13.f);
    auto* mgr = CAccidentManager::GetInstance();
    if (!mgr) return;
    // Get first free accident, then find next one excluding it
    CAccident* first = mgr->GetNearestFreeAccident(pos, false);
    CAccident* orig;
    CAccident* rev;
    { HookDisableGuard guard("Global/CAccidentManager/GetNearestFreeAccidentExceptThisOne");
      orig = mgr->GetNearestFreeAccidentExceptThisOne(pos, first, false); }
    rev = mgr->GetNearestFreeAccidentExceptThisOne(pos, first, false);
    EXPECT_EQ(orig, rev);
}

// --- GetInstance ---

GAME_DIFF_TEST(CAccidentManager, GetInstance) {
    CAccidentManager* orig;
    CAccidentManager* rev;
    { HookDisableGuard guard("Global/CAccidentManager/GetInstance");
      orig = CAccidentManager::GetInstance(); }
    rev = CAccidentManager::GetInstance();
    EXPECT_EQ(orig, rev);
}
