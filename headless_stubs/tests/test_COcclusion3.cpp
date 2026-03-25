// test_COcclusion3.cpp — More COcclusion differential tests.
// Hook paths: Global/COcclusion/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "Occlusion.h"

GAME_DIFF_TEST(COcclusion3, IsPositionOccluded_PlayerPos) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    CVector pos = player->GetPosition();
    bool orig, rev;
    { HookDisableGuard guard("Global/COcclusion/IsPositionOccluded");
      orig = COcclusion::IsPositionOccluded(pos, 2.0f); }
    rev = COcclusion::IsPositionOccluded(pos, 2.0f);
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(COcclusion3, IsPositionOccluded_HighAltitude) {
    CVector pos(2488.f, -1666.f, 200.f);
    bool orig, rev;
    { HookDisableGuard guard("Global/COcclusion/IsPositionOccluded");
      orig = COcclusion::IsPositionOccluded(pos, 5.0f); }
    rev = COcclusion::IsPositionOccluded(pos, 5.0f);
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(COcclusion3, IsPositionOccluded_Underground) {
    CVector pos(2488.f, -1666.f, -10.f);
    bool orig, rev;
    { HookDisableGuard guard("Global/COcclusion/IsPositionOccluded");
      orig = COcclusion::IsPositionOccluded(pos, 1.0f); }
    rev = COcclusion::IsPositionOccluded(pos, 1.0f);
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(COcclusion3, IsPositionOccluded_SF) {
    CVector pos(-2170.f, 250.f, 30.f);
    bool orig, rev;
    { HookDisableGuard guard("Global/COcclusion/IsPositionOccluded");
      orig = COcclusion::IsPositionOccluded(pos, 3.0f); }
    rev = COcclusion::IsPositionOccluded(pos, 3.0f);
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(COcclusion3, IsPositionOccluded_LV) {
    CVector pos(2000.f, 1500.f, 20.f);
    bool orig, rev;
    { HookDisableGuard guard("Global/COcclusion/IsPositionOccluded");
      orig = COcclusion::IsPositionOccluded(pos, 3.0f); }
    rev = COcclusion::IsPositionOccluded(pos, 3.0f);
    EXPECT_EQ(orig, rev);
}

// OccluderHidesBehind(nullptr, nullptr) crashes original code — skip
