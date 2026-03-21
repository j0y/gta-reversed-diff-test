// test_COcclusion.cpp — Differential tests for COcclusion queries.
// Hook paths: Global/COcclusion/<fn>
// Occlusion culling determines what's visible to the camera.

#include "StdInc.h"
#include "TestFramework.h"
#include "Occlusion.h"

GAME_DIFF_TEST(COcclusion, IsPositionOccluded_PlayerPos) {
    auto* ped = FindPlayerPed(0);
    if (!ped) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No player ped"); return; }
    CVector pos = ped->GetPosition();
    bool orig, rev;
    { HookDisableGuard guard("Global/COcclusion/IsPositionOccluded");
      orig = COcclusion::IsPositionOccluded(pos, 1.0f); }
    rev = COcclusion::IsPositionOccluded(pos, 1.0f);
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(COcclusion, IsPositionOccluded_Various) {
    struct TestCase { CVector pos; float radius; };
    TestCase cases[] = {
        { { 2488.f, -1666.f, 13.f }, 1.0f },   // Grove Street (near player)
        { { 0.f, 0.f, 0.f }, 5.0f },            // World origin
        { { 2488.f, -1666.f, 100.f }, 2.0f },   // High above Grove Street
    };
    for (auto& tc : cases) {
        bool orig, rev;
        { HookDisableGuard guard("Global/COcclusion/IsPositionOccluded");
          orig = COcclusion::IsPositionOccluded(tc.pos, tc.radius); }
        rev = COcclusion::IsPositionOccluded(tc.pos, tc.radius);
        EXPECT_EQ(orig, rev);
    }
}
