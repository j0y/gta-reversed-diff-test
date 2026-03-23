// test_CClothes2.cpp — Deep differential tests for CClothes.
// Hook paths: Global/CClothes/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "Clothes.h"

// --- GetPlayerMotionGroupToLoad ---

GAME_DIFF_TEST(CClothes, GetPlayerMotionGroupToLoad2) {
    int32 orig, rev;
    { HookDisableGuard guard("Global/CClothes/GetPlayerMotionGroupToLoad"); orig = (int32)CClothes::GetPlayerMotionGroupToLoad(); }
    rev = (int32)CClothes::GetPlayerMotionGroupToLoad();
    EXPECT_EQ(orig, rev);
}

// --- GetDependentTexture ---

GAME_DIFF_TEST(CClothes, GetDependentTexture_AllParts) {
    for (int32 part = 0; part < 10; part++) {
        int32 orig, rev;
        { HookDisableGuard guard("Global/CClothes/GetDependentTexture");
          orig = (int32)CClothes::GetDependentTexture(static_cast<eClothesModelPart>(part)); }
        rev = (int32)CClothes::GetDependentTexture(static_cast<eClothesModelPart>(part));
        EXPECT_EQ(orig, rev);
    }
}
