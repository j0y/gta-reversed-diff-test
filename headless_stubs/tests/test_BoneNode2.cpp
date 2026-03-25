// test_BoneNode2.cpp — BoneNode_c differential tests.
// Hook paths: Global/BoneNode_c/<fn>
// BoneNode_c::Init requires RpHAnimBlendInterpFrame* — can't easily construct.
// Test GetSpeed/SetSpeed on a zero-initialized node.

#include "StdInc.h"
#include "TestFramework.h"
#include "BoneNode_c.h"

GAME_DIFF_TEST(BoneNode2, SetSpeed_GetSpeed) {
    BoneNode_c node{};
    float testSpeed = 2.5f;
    { HookDisableGuard guard("Global/BoneNode_c/SetSpeed");
      node.SetSpeed(testSpeed); }
    float origSpeed;
    { HookDisableGuard guard("Global/BoneNode_c/GetSpeed");
      origSpeed = node.GetSpeed(); }

    node.SetSpeed(testSpeed);
    float revSpeed = node.GetSpeed();
    EXPECT_NEAR(origSpeed, revSpeed, 1e-5f);
}

GAME_DIFF_TEST(BoneNode2, GetSpeed_ZeroInit) {
    BoneNode_c node{};
    float orig, rev;
    { HookDisableGuard guard("Global/BoneNode_c/GetSpeed");
      orig = node.GetSpeed(); }
    rev = node.GetSpeed();
    EXPECT_NEAR(orig, rev, 1e-5f);
}
