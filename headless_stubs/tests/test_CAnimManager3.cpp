// test_CAnimManager3.cpp — Differential tests for CAnimManager.
// Hook paths: Animation/CAnimManager/<fn>
// Tests animation group/block queries.

#include "StdInc.h"
#include "TestFramework.h"
#include "Animation/AnimManager.h"
#include "Animation/AnimBlendAssocGroup.h"

GAME_DIFF_TEST(CAnimManager3, GetAnimGroupName_AllGroups) {
    // There are roughly 170+ anim groups. Test first 50.
    for (int32 i = 0; i < 50; i++) {
        const char* orig;
        const char* rev;
        { HookDisableGuard guard("Animation/CAnimManager/GetAnimGroupName"); orig = CAnimManager::GetAnimGroupName((AssocGroupId)i); }
        rev = CAnimManager::GetAnimGroupName((AssocGroupId)i);
        if (orig && rev) {
            EXPECT_EQ(strcmp(orig, rev), 0);
        } else {
            EXPECT_EQ(orig, rev);
        }
    }
}

GAME_DIFF_TEST(CAnimManager3, GetAnimBlockName_AllBlocks) {
    // Test first 50 anim blocks
    for (int32 i = 0; i < 50; i++) {
        const char* orig;
        const char* rev;
        { HookDisableGuard guard("Animation/CAnimManager/GetAnimBlockName"); orig = CAnimManager::GetAnimBlockName((AssocGroupId)i); }
        rev = CAnimManager::GetAnimBlockName((AssocGroupId)i);
        if (orig && rev) {
            EXPECT_EQ(strcmp(orig, rev), 0);
        } else {
            EXPECT_EQ(orig, rev);
        }
    }
}

GAME_DIFF_TEST(CAnimManager3, GetNumRefsToAnimBlock_First20) {
    for (int32 i = 0; i < 20; i++) {
        int32 orig, rev;
        { HookDisableGuard guard("Animation/CAnimManager/GetNumRefsToAnimBlock"); orig = CAnimManager::GetNumRefsToAnimBlock(i); }
        rev = CAnimManager::GetNumRefsToAnimBlock(i);
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(CAnimManager3, GetFirstAssocGroup_Various) {
    // GetFirstAssocGroup returns AssocGroupId
    const char* names[] = { "default", "man", "woman", "sexywoman", "fat" };
    for (const char* name : names) {
        AssocGroupId orig, rev;
        { HookDisableGuard guard("Animation/CAnimManager/GetFirstAssocGroup"); orig = CAnimManager::GetFirstAssocGroup(name); }
        rev = CAnimManager::GetFirstAssocGroup(name);
        EXPECT_EQ((int32)orig, (int32)rev);
    }
}
