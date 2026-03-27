// test_BoneNodeManager.cpp — Differential tests for BoneNodeManager_c.
// Hook paths: Global/BoneNodeManager_c/<fn>
//
// Pool-based bone node manager: Init fills pool, GetBoneNode removes from pool,
// ReturnBoneNode adds back, GetNumBoneNodesLeft returns count.

#include "StdInc.h"
#include "TestFramework.h"
#include "BoneNodeManager_c.h"

// --- Init + GetNumBoneNodesLeft ---

GAME_DIFF_TEST(BoneNodeManager_c, Init_SetsFullPool) {
    BoneNodeManager_c orig{}, rev{};
    { HookDisableGuard guard("Global/BoneNodeManager_c/Init");
      orig.Init(); }
    rev.Init();

    uint32 origCount, revCount;
    { HookDisableGuard guard("Global/BoneNodeManager_c/GetNumBoneNodesLeft");
      origCount = orig.GetNumBoneNodesLeft(); }
    revCount = rev.GetNumBoneNodesLeft();

    EXPECT_EQ(origCount, revCount);
    EXPECT_GT((float)revCount, 0.0f);
}

// --- GetBoneNode + ReturnBoneNode ---

GAME_DIFF_TEST(BoneNodeManager_c, GetAndReturn) {
    BoneNodeManager_c mgr{};
    mgr.Init();
    uint32 initialCount = mgr.GetNumBoneNodesLeft();

    // Get a node via original
    BoneNode_c* origNode;
    { HookDisableGuard guard("Global/BoneNodeManager_c/GetBoneNode");
      origNode = mgr.GetBoneNode(); }
    uint32 afterGetOrig = mgr.GetNumBoneNodesLeft();

    // Return it via original
    { HookDisableGuard guard("Global/BoneNodeManager_c/ReturnBoneNode");
      mgr.ReturnBoneNode(origNode); }
    uint32 afterReturnOrig = mgr.GetNumBoneNodesLeft();

    // Now same with reversed
    BoneNode_c* revNode = mgr.GetBoneNode();
    uint32 afterGetRev = mgr.GetNumBoneNodesLeft();

    mgr.ReturnBoneNode(revNode);
    uint32 afterReturnRev = mgr.GetNumBoneNodesLeft();

    EXPECT_EQ(afterGetOrig, afterGetRev);
    EXPECT_EQ(afterReturnOrig, afterReturnRev);
    EXPECT_EQ(afterReturnRev, initialCount);
}

// --- Reset ---

GAME_DIFF_TEST(BoneNodeManager_c, Reset_RestoresPool) {
    BoneNodeManager_c mgr{};
    mgr.Init();
    uint32 fullCount = mgr.GetNumBoneNodesLeft();

    // Drain some nodes
    mgr.GetBoneNode();
    mgr.GetBoneNode();
    mgr.GetBoneNode();

    { HookDisableGuard guard("Global/BoneNodeManager_c/Reset");
      mgr.Reset(); }
    uint32 origAfterReset = mgr.GetNumBoneNodesLeft();

    // Drain again and reset via reversed
    mgr.GetBoneNode();
    mgr.GetBoneNode();
    mgr.Reset();
    uint32 revAfterReset = mgr.GetNumBoneNodesLeft();

    EXPECT_EQ(origAfterReset, revAfterReset);
    EXPECT_EQ(revAfterReset, fullCount);
}

// --- Exit ---

GAME_DIFF_TEST(BoneNodeManager_c, Exit_EmptiesPool) {
    BoneNodeManager_c orig{}, rev{};
    orig.Init(); rev.Init();

    { HookDisableGuard guard("Global/BoneNodeManager_c/Exit");
      orig.Exit(); }
    rev.Exit();

    uint32 origCount = orig.GetNumBoneNodesLeft();
    uint32 revCount = rev.GetNumBoneNodesLeft();
    EXPECT_EQ(origCount, revCount);
    EXPECT_EQ(revCount, (uint32)0);
}
