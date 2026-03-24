// test_COcclusion2.cpp — Extended differential tests for COcclusion.
// Hook paths: Global/COcclusion/<fn>
//
// Tests occlusion queries at various positions and radii.

#include "StdInc.h"
#include "TestFramework.h"
#include "Occlusion.h"

// --- IsPositionOccluded with varying radii ---

GAME_DIFF_TEST(COcclusion, IsPositionOccluded_RadiusSweep) {
    CVector pos(2488.f, -1666.f, 13.f);
    float radii[] = { 0.1f, 0.5f, 1.0f, 2.0f, 5.0f, 10.0f, 50.0f };
    for (float r : radii) {
        bool orig, rev;
        { HookDisableGuard guard("Global/COcclusion/IsPositionOccluded");
          orig = COcclusion::IsPositionOccluded(pos, r); }
        rev = COcclusion::IsPositionOccluded(pos, r);
        EXPECT_EQ(orig, rev);
    }
}

// --- IsPositionOccluded grid sweep ---

GAME_DIFF_TEST(COcclusion, IsPositionOccluded_GridSweep) {
    // Sweep a grid around Grove Street to find any divergences
    for (float x = 2400.f; x <= 2600.f; x += 50.f) {
        for (float y = -1800.f; y <= -1600.f; y += 50.f) {
            CVector pos(x, y, 20.f);
            bool orig, rev;
            { HookDisableGuard guard("Global/COcclusion/IsPositionOccluded");
              orig = COcclusion::IsPositionOccluded(pos, 1.0f); }
            rev = COcclusion::IsPositionOccluded(pos, 1.0f);
            EXPECT_EQ(orig, rev);
        }
    }
}

// --- OccluderHidesBehind ---

GAME_DIFF_TEST(COcclusion, OccluderHidesBehind_ActivePairs) {
    // Test all pairs of active occluders (if any exist at state 9)
    int32 numActive = COcclusion::NumActiveOccluders;
    if (numActive < 2) return;
    int32 limit = (numActive > 5) ? 5 : numActive;
    for (int32 i = 0; i < limit; i++) {
        for (int32 j = i + 1; j < limit; j++) {
            bool orig, rev;
            { HookDisableGuard guard("Global/COcclusion/OccluderHidesBehind");
              orig = COcclusion::OccluderHidesBehind(&COcclusion::ActiveOccluders[i],
                                                      &COcclusion::ActiveOccluders[j]); }
            rev = COcclusion::OccluderHidesBehind(&COcclusion::ActiveOccluders[i],
                                                    &COcclusion::ActiveOccluders[j]);
            EXPECT_EQ(orig, rev);
        }
    }
}
