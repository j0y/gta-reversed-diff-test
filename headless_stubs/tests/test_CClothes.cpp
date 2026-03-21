// test_CClothes.cpp — Differential tests for CClothes static functions.
// Hook paths: Global/CClothes/<fn>
// CClothes manages player model customization — motion groups and textures.

#include "StdInc.h"
#include "TestFramework.h"
#include "Clothes.h"

GAME_DIFF_TEST(CClothes, GetPlayerMotionGroupToLoad) {
    int orig, rev;
    { HookDisableGuard guard("Global/CClothes/GetPlayerMotionGroupToLoad");
      orig = (int)CClothes::GetPlayerMotionGroupToLoad(); }
    rev = (int)CClothes::GetPlayerMotionGroupToLoad();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CClothes, GetDefaultPlayerMotionGroup) {
    int orig, rev;
    { HookDisableGuard guard("Global/CClothes/GetDefaultPlayerMotionGroup");
      orig = (int)CClothes::GetDefaultPlayerMotionGroup(); }
    rev = (int)CClothes::GetDefaultPlayerMotionGroup();
    EXPECT_EQ(orig, rev);
}
