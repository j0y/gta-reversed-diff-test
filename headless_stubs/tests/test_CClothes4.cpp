// test_CClothes4.cpp — Extended CClothes behavior tests.
// Hook paths: Global/CClothes/<fn>
// RequestMotionGroupAnims and RebuildPlayerIfNeeded are void functions — behavior tests.

#include "StdInc.h"
#include "TestFramework.h"
#include "Clothes.h"

// --- RequestMotionGroupAnims ---

GAME_TEST(CClothes4, RequestMotionGroupAnims) {
    CClothes::RequestMotionGroupAnims();
    EXPECT_TRUE(true); // no crash = pass
}

// --- RebuildPlayerIfNeeded ---

GAME_TEST(CClothes4, RebuildPlayerIfNeeded_WithPlayer) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    CClothes::RebuildPlayerIfNeeded(player);
    EXPECT_TRUE(true); // no crash = pass
}

GAME_TEST(CClothes4, RebuildPlayerIfNeeded_Nullptr) {
    CClothes::RebuildPlayerIfNeeded(nullptr);
    EXPECT_TRUE(true); // no crash = pass
}
