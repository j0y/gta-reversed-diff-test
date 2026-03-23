// test_CAnimManager.cpp — Tests for CAnimManager.
// Hook paths: Animation/CAnimManager/<fn>
//
// CAnimManager is the global animation manager — handles animation blocks,
// groups, hierarchies, and association lookups. Tests verify that lookups
// and queries match between reversed and original code.

#include "StdInc.h"
#include "TestFramework.h"
#include "Animation/AnimManager.h"

// --- Block lookups by name ---

// GetAnimationBlockIndex is not hooked — behavior tests only

GAME_TEST(CAnimManager, GetAnimationBlockIndex_ped) {
    int32 idx = CAnimManager::GetAnimationBlockIndex("ped");
    EXPECT_TRUE(idx >= 0);
}

GAME_TEST(CAnimManager, GetAnimationBlockIndex_car) {
    int32 idx = CAnimManager::GetAnimationBlockIndex("car");
    EXPECT_TRUE(idx >= 0);
}

GAME_TEST(CAnimManager, GetAnimationBlockIndex_nonexistent) {
    int32 idx = CAnimManager::GetAnimationBlockIndex("this_block_does_not_exist_xyz");
    EXPECT_TRUE(idx < 0);
}

// --- Block pointer lookups ---

GAME_TEST(CAnimManager, GetAnimationBlock_ped_NotNull) {
    auto* block = CAnimManager::GetAnimationBlock("ped");
    EXPECT_TRUE(block != nullptr);
}

GAME_TEST(CAnimManager, GetAnimationBlock_ped_IsLoaded) {
    auto* block = CAnimManager::GetAnimationBlock("ped");
    if (!block) { GetTestContext().RecordFailure(__FILE__, __LINE__, "ped block null"); return; }
    EXPECT_TRUE(block->IsLoaded);
}

GAME_TEST(CAnimManager, GetAnimationBlock_ped_HasAnims) {
    auto* block = CAnimManager::GetAnimationBlock("ped");
    if (!block) { GetTestContext().RecordFailure(__FILE__, __LINE__, "ped block null"); return; }
    EXPECT_GT(block->NumAnims, (uint32)0);
}

// --- Group name lookups ---

GAME_DIFF_TEST(CAnimManager, GetAnimGroupName) {
    const char* orig;
    const char* rev;
    { HookDisableGuard guard("Animation/CAnimManager/GetAnimGroupName");
      orig = CAnimManager::GetAnimGroupName(ANIM_GROUP_DEFAULT); }
    rev = CAnimManager::GetAnimGroupName(ANIM_GROUP_DEFAULT);
    // Both should return the same pointer (same static data)
    EXPECT_EQ((uintptr_t)orig, (uintptr_t)rev);
}

GAME_DIFF_TEST(CAnimManager, GetAnimBlockName) {
    const char* orig;
    const char* rev;
    { HookDisableGuard guard("Animation/CAnimManager/GetAnimBlockName");
      orig = CAnimManager::GetAnimBlockName(ANIM_GROUP_DEFAULT); }
    rev = CAnimManager::GetAnimBlockName(ANIM_GROUP_DEFAULT);
    EXPECT_EQ((uintptr_t)orig, (uintptr_t)rev);
}

// --- GetFirstAssocGroup ---

GAME_DIFF_TEST(CAnimManager, GetFirstAssocGroup_ped) {
    int32 orig, rev;
    { HookDisableGuard guard("Animation/CAnimManager/GetFirstAssocGroup");
      orig = (int32)CAnimManager::GetFirstAssocGroup("ped"); }
    rev = (int32)CAnimManager::GetFirstAssocGroup("ped");
    EXPECT_EQ(orig, rev);
}

// --- GetNumRefsToAnimBlock ---

GAME_DIFF_TEST(CAnimManager, GetNumRefsToAnimBlock) {
    int32 pedIdx = CAnimManager::GetAnimationBlockIndex("ped");
    if (pedIdx < 0) { GetTestContext().RecordFailure(__FILE__, __LINE__, "ped block not found"); return; }

    int32 orig, rev;
    { HookDisableGuard guard("Animation/CAnimManager/GetNumRefsToAnimBlock");
      orig = CAnimManager::GetNumRefsToAnimBlock(pedIdx); }
    rev = CAnimManager::GetNumRefsToAnimBlock(pedIdx);
    EXPECT_EQ(orig, rev);
}

// --- Animation hierarchy lookup ---

GAME_TEST(CAnimManager, GetAnimation_ById_NoExcept) {
    // ANIM_ID_WALK is a common animation that should exist
    auto& hier = CAnimManager::GetAnimation(ANIM_ID_WALK);
    // Just verify no crash — hierarchy should have data
    (void)hier;
    EXPECT_TRUE(true);
}

// --- GetAnimAssociation ---

GAME_TEST(CAnimManager, GetAnimAssociation_Walk) {
    auto* assoc = CAnimManager::GetAnimAssociation(ANIM_GROUP_DEFAULT, ANIM_ID_WALK);
    EXPECT_TRUE(assoc != nullptr);
}

GAME_TEST(CAnimManager, GetAnimAssociation_Idle) {
    auto* assoc = CAnimManager::GetAnimAssociation(ANIM_GROUP_DEFAULT, ANIM_ID_IDLE);
    EXPECT_TRUE(assoc != nullptr);
}

GAME_TEST(CAnimManager, GetAnimAssociation_Run) {
    auto* assoc = CAnimManager::GetAnimAssociation(ANIM_GROUP_DEFAULT, ANIM_ID_RUN);
    EXPECT_TRUE(assoc != nullptr);
}
