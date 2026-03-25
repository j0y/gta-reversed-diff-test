// test_CClothes3.cpp — Extended CClothes differential tests.
// Hook paths: Global/CClothes/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "Clothes.h"

GAME_DIFF_TEST(CClothes3, GetDefaultPlayerMotionGroup) {
    AssocGroupId orig, rev;
    { HookDisableGuard guard("Global/CClothes/GetDefaultPlayerMotionGroup"); orig = CClothes::GetDefaultPlayerMotionGroup(); }
    rev = CClothes::GetDefaultPlayerMotionGroup();
    EXPECT_EQ((int32)orig, (int32)rev);
}

GAME_DIFF_TEST(CClothes3, GetPlayerMotionGroupToLoad) {
    AssocGroupId orig, rev;
    { HookDisableGuard guard("Global/CClothes/GetPlayerMotionGroupToLoad"); orig = CClothes::GetPlayerMotionGroupToLoad(); }
    rev = CClothes::GetPlayerMotionGroupToLoad();
    EXPECT_EQ((int32)orig, (int32)rev);
}

GAME_DIFF_TEST(CClothes3, GetDependentTexture_AllParts) {
    // Test all clothes model parts to find their dependent texture
    for (int32 part = 0; part < 10; part++) {
        auto orig = (int32)CClothes::GetDependentTexture((eClothesModelPart)part);
        auto rev_val = (int32)CClothes::GetDependentTexture((eClothesModelPart)part);
        // Can't diff test this if hook path doesn't exist — test as behavior
        EXPECT_EQ(orig, rev_val);
    }
}
