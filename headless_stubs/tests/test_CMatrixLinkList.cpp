// test_CMatrixLinkList.cpp — Differential tests for CMatrixLinkList.
// Hook paths: Core/CMatrixLinkList/<fn>
//
// Doubly-linked list manager for CMatrixLink nodes.
// Three lists: free, list1 (main), list2 (allocated).
// Init allocates pool, nodes move between lists.

#include "StdInc.h"
#include "TestFramework.h"
#include "Core/MatrixLinkList.h"

static constexpr int32 POOL_SIZE = 10;

// Use the global gMatrixList which is already initialized at state 9.

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

// --- AddToList1 + MoveToFreeList round-trip ---

GAME_DIFF_TEST(CMatrixLinkList, AddToList1_MoveToFreeList) {
    int32 freeBefore = gMatrixList.GetNumFree();
    int32 used1Before = gMatrixList.GetNumUsed1();

    // Add via original
    CMatrixLink* origNode;
    { HookDisableGuard guard("Core/CMatrixLinkList/AddToList1");
      origNode = gMatrixList.AddToList1(); }

    int32 freeAfterOrigAdd = gMatrixList.GetNumFree();
    int32 used1AfterOrigAdd = gMatrixList.GetNumUsed1();

    // Return it
    { HookDisableGuard guard("Core/CMatrixLinkList/MoveToFreeList");
      gMatrixList.MoveToFreeList(origNode); }

    // Add via reversed
    CMatrixLink* revNode = gMatrixList.AddToList1();

    int32 freeAfterRevAdd = gMatrixList.GetNumFree();
    int32 used1AfterRevAdd = gMatrixList.GetNumUsed1();

    // Return it
    gMatrixList.MoveToFreeList(revNode);

    EXPECT_EQ(freeAfterOrigAdd, freeAfterRevAdd);
    EXPECT_EQ(used1AfterOrigAdd, used1AfterRevAdd);
    EXPECT_EQ(freeAfterOrigAdd, freeBefore - 1);
    EXPECT_EQ(used1AfterOrigAdd, used1Before + 1);

    // Verify counts restored
    EXPECT_EQ(gMatrixList.GetNumFree(), freeBefore);
    EXPECT_EQ(gMatrixList.GetNumUsed1(), used1Before);
}

// --- AddToList2 + MoveToFreeList round-trip ---

GAME_DIFF_TEST(CMatrixLinkList, AddToList2_MoveToFreeList) {
    int32 freeBefore = gMatrixList.GetNumFree();
    int32 used2Before = gMatrixList.GetNumUsed2();

    CMatrixLink* origNode;
    { HookDisableGuard guard("Core/CMatrixLinkList/AddToList2");
      origNode = gMatrixList.AddToList2(); }
    int32 freeAfterOrig = gMatrixList.GetNumFree();
    int32 used2AfterOrig = gMatrixList.GetNumUsed2();

    gMatrixList.MoveToFreeList(origNode);

    CMatrixLink* revNode = gMatrixList.AddToList2();
    int32 freeAfterRev = gMatrixList.GetNumFree();
    int32 used2AfterRev = gMatrixList.GetNumUsed2();

    gMatrixList.MoveToFreeList(revNode);

    EXPECT_EQ(freeAfterOrig, freeAfterRev);
    EXPECT_EQ(used2AfterOrig, used2AfterRev);
    EXPECT_EQ(freeAfterOrig, freeBefore - 1);
    EXPECT_EQ(used2AfterOrig, used2Before + 1);

    EXPECT_EQ(gMatrixList.GetNumFree(), freeBefore);
}

// --- MoveToList1 / MoveToList2 ---

GAME_DIFF_TEST(CMatrixLinkList, MoveToList1_MoveToList2) {
    int32 used1Before = gMatrixList.GetNumUsed1();
    int32 used2Before = gMatrixList.GetNumUsed2();

    // Get a node from free list
    CMatrixLink* node = gMatrixList.AddToList1();

    // Move to list2 via original
    { HookDisableGuard guard("Core/CMatrixLinkList/MoveToList2");
      gMatrixList.MoveToList2(node); }
    int32 origUsed1 = gMatrixList.GetNumUsed1();
    int32 origUsed2 = gMatrixList.GetNumUsed2();

    // Move back to list1 via original
    { HookDisableGuard guard("Core/CMatrixLinkList/MoveToList1");
      gMatrixList.MoveToList1(node); }
    int32 origUsed1After = gMatrixList.GetNumUsed1();
    int32 origUsed2After = gMatrixList.GetNumUsed2();

    // Now do the same with reversed
    gMatrixList.MoveToList2(node);
    int32 revUsed1 = gMatrixList.GetNumUsed1();
    int32 revUsed2 = gMatrixList.GetNumUsed2();

    gMatrixList.MoveToList1(node);
    int32 revUsed1After = gMatrixList.GetNumUsed1();
    int32 revUsed2After = gMatrixList.GetNumUsed2();

    EXPECT_EQ(origUsed1, revUsed1);
    EXPECT_EQ(origUsed2, revUsed2);
    EXPECT_EQ(origUsed1After, revUsed1After);
    EXPECT_EQ(origUsed2After, revUsed2After);
    EXPECT_EQ(origUsed1, used1Before);       // removed from list1
    EXPECT_EQ(origUsed2, used2Before + 1);   // added to list2

    // Cleanup
    gMatrixList.MoveToFreeList(node);
}

// --- Behavior tests ---

GAME_TEST(CMatrixLinkList, CountsConsistent) {
    int32 free = gMatrixList.GetNumFree();
    int32 used1 = gMatrixList.GetNumUsed1();
    int32 used2 = gMatrixList.GetNumUsed2();
    // Total should be constant (pool was initialized with some count)
    int32 total = free + used1 + used2;
    EXPECT_GT((float)total, 0.0f);

    // Add and remove, verify total unchanged
    CMatrixLink* n = gMatrixList.AddToList1();
    int32 total2 = gMatrixList.GetNumFree() + gMatrixList.GetNumUsed1() + gMatrixList.GetNumUsed2();
    EXPECT_EQ(total, total2);
    gMatrixList.MoveToFreeList(n);
}
