// test_CMatrixLinkList.cpp — Differential tests for CMatrixLinkList.
// Hook paths: Core/CMatrixLinkList/<fn>
//
// CMatrixLinkList manages a pool of CMatrix objects used by entities.
// At state 9 it should have some allocated matrices.

#include "StdInc.h"
#include "TestFramework.h"

// CMatrixLinkList is a global — gMatrixList
extern CMatrixLinkList& gMatrixList;

// --- GetNumFree ---

GAME_DIFF_TEST(CMatrixLinkList, GetNumFree) {
    int32 orig, rev;
    { HookDisableGuard guard("Core/CMatrixLinkList/GetNumFree");
      orig = gMatrixList.GetNumFree(); }
    rev = gMatrixList.GetNumFree();
    EXPECT_EQ(orig, rev);
}

// --- GetNumUsed1 ---

GAME_DIFF_TEST(CMatrixLinkList, GetNumUsed1) {
    int32 orig, rev;
    { HookDisableGuard guard("Core/CMatrixLinkList/GetNumUsed1");
      orig = gMatrixList.GetNumUsed1(); }
    rev = gMatrixList.GetNumUsed1();
    EXPECT_EQ(orig, rev);
}

// --- GetNumUsed2 ---

GAME_DIFF_TEST(CMatrixLinkList, GetNumUsed2) {
    int32 orig, rev;
    { HookDisableGuard guard("Core/CMatrixLinkList/GetNumUsed2");
      orig = gMatrixList.GetNumUsed2(); }
    rev = gMatrixList.GetNumUsed2();
    EXPECT_EQ(orig, rev);
}

// --- Behavior: counts should be reasonable ---

GAME_TEST(CMatrixLinkList, Counts_Reasonable) {
    int32 free = gMatrixList.GetNumFree();
    int32 used1 = gMatrixList.GetNumUsed1();
    int32 used2 = gMatrixList.GetNumUsed2();
    // Total should be > 0 (pool is initialized)
    EXPECT_TRUE(free + used1 + used2 > 0);
    // All non-negative
    EXPECT_TRUE(free >= 0);
    EXPECT_TRUE(used1 >= 0);
    EXPECT_TRUE(used2 >= 0);
}
