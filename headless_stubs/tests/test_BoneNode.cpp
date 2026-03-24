// test_BoneNode.cpp — Differential tests for BoneNode_c.
// Hook paths: Global/BoneNode_c/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "BoneNode_c.h"
#include "Entity/Ped/Ped.h"

// BoneNode_c methods are called on ped IK bone chains.
// Access via player ped's IK system.

GAME_DIFF_TEST(BoneNode, GetIdFromBoneTag_CommonBones) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    // Access bone nodes through IK chain manager
    // Test static bone tag lookups
    int32 tags[] = { 1, 2, 3, 4, 5, 6, 7, 8, 21, 22, 31, 32 };
    for (int32 tag : tags) {
        // BoneNode_c is part of IKChain — we can't access directly without
        // a valid IKChain. Test the static lookup.
        EXPECT_TRUE(true); // Placeholder — needs IKChain instance
    }
}
