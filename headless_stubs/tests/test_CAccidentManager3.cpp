// test_CAccidentManager3.cpp — More CAccidentManager differential tests.
// Hook paths: Global/CAccidentManager/<fn>
//
// Already tested in test_CAccidentManager2.cpp:
//   GetNearestFreeAccident (4 tests), GetNearestFreeAccidentExceptThisOne, GetInstance
// New targets:
//   GetNumberOfFreeAccidents, GetNearestFreeAccidentExceptThisOne (more positions)

#include "StdInc.h"
#include "TestFramework.h"
#include "AccidentManager.h"

// --- GetNumberOfFreeAccidents ---

GAME_DIFF_TEST(CAccidentManager3, GetNumberOfFreeAccidents) {
    auto* mgr = CAccidentManager::GetInstance();
    if (!mgr) return;
    int32 orig;
    int32 rev;
    { HookDisableGuard guard("Global/CAccidentManager/GetNumberOfFreeAccidents");
      orig = mgr->GetNumberOfFreeAccidents(); }
    rev = mgr->GetNumberOfFreeAccidents();
    EXPECT_EQ(orig, rev);
}

// --- GetNearestFreeAccidentExceptThisOne with nullptr exception ---

GAME_DIFF_TEST(CAccidentManager3, GetNearestFreeAccidentExceptThisOne_NullException) {
    CVector pos(2488.f, -1666.f, 13.f);
    auto* mgr = CAccidentManager::GetInstance();
    if (!mgr) return;
    CAccident* orig;
    CAccident* rev;
    { HookDisableGuard guard("Global/CAccidentManager/GetNearestFreeAccidentExceptThisOne");
      orig = mgr->GetNearestFreeAccidentExceptThisOne(pos, nullptr, false); }
    rev = mgr->GetNearestFreeAccidentExceptThisOne(pos, nullptr, false);
    EXPECT_EQ(orig, rev);
}

// --- GetNearestFreeAccidentExceptThisOne at various positions ---

GAME_DIFF_TEST(CAccidentManager3, GetNearestFreeAccidentExceptThisOne_MultiPos) {
    CVector positions[] = {
        {0.f, 0.f, 0.f},
        {-2170.f, 250.f, 30.f},
        {2027.f, 1008.f, 20.f},
    };
    auto* mgr = CAccidentManager::GetInstance();
    if (!mgr) return;
    for (auto& pos : positions) {
        CAccident* first = mgr->GetNearestFreeAccident(pos, false);
        CAccident* orig;
        CAccident* rev;
        { HookDisableGuard guard("Global/CAccidentManager/GetNearestFreeAccidentExceptThisOne");
          orig = mgr->GetNearestFreeAccidentExceptThisOne(pos, first, false); }
        rev = mgr->GetNearestFreeAccidentExceptThisOne(pos, first, false);
        EXPECT_EQ(orig, rev);
    }
}

// --- GetNearestFreeAccidentExceptThisOne with bIgnoreHeadless=true ---

GAME_DIFF_TEST(CAccidentManager3, GetNearestFreeAccidentExceptThisOne_IgnoreHeadless) {
    CVector pos(2488.f, -1666.f, 13.f);
    auto* mgr = CAccidentManager::GetInstance();
    if (!mgr) return;
    CAccident* first = mgr->GetNearestFreeAccident(pos, true);
    CAccident* orig;
    CAccident* rev;
    { HookDisableGuard guard("Global/CAccidentManager/GetNearestFreeAccidentExceptThisOne");
      orig = mgr->GetNearestFreeAccidentExceptThisOne(pos, first, true); }
    rev = mgr->GetNearestFreeAccidentExceptThisOne(pos, first, true);
    EXPECT_EQ(orig, rev);
}
