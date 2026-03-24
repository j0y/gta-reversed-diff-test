// test_CRadar_DirectCall2.cpp — More direct-call differential tests for CRadar.

#include "StdInc.h"
#include "TestFramework.h"
#include "Radar.h"

// --- TransformRadarPointToRealWorldSpace (0x5835A0) ---

GAME_TEST(CRadar_DirectCall2, TransformRadarPointToRealWorldSpace) {
    CVector2D inputs[] = {
        {0.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 1.0f}, {-1.0f, -1.0f},
        {0.5f, 0.5f}, {0.25f, -0.75f},
    };
    for (auto& in : inputs) {
        CVector2D orig = CallOriginal_CVector2D(0x5835A0, in);
        CVector2D rev = CRadar::TransformRadarPointToRealWorldSpace(in);
        EXPECT_NEAR(orig.x, rev.x, 1e-1f); // world coords can be large
        EXPECT_NEAR(orig.y, rev.y, 1e-1f);
    }
}

// --- TransformRadarPointToScreenSpace extended inputs ---

GAME_TEST(CRadar_DirectCall2, TransformRadarToScreen_Extended) {
    CVector2D inputs[] = {
        {0.0f, 0.0f}, {1.0f, 1.0f}, {-1.0f, -1.0f},
        {0.1f, 0.1f}, {0.9f, 0.9f}, {-0.9f, 0.9f},
        {0.0f, -1.0f}, {-0.5f, 0.5f},
    };
    for (auto& in : inputs) {
        CVector2D orig = CallOriginal_CVector2D(0x583480, in);
        CVector2D rev = CRadar::TransformRadarPointToScreenSpace(in);
        EXPECT_NEAR(orig.x, rev.x, 1e-3f);
        EXPECT_NEAR(orig.y, rev.y, 1e-3f);
    }
}

// --- TransformRealWorldPointToRadarSpace extended ---

GAME_TEST(CRadar_DirectCall2, TransformRealWorldToRadar_Extended) {
    CVector2D inputs[] = {
        {2495.0f, -1665.0f}, {-2170.0f, 250.0f}, {2027.0f, 1008.0f},
        {0.0f, 0.0f}, {3000.0f, 3000.0f}, {-3000.0f, -3000.0f},
    };
    for (auto& in : inputs) {
        CVector2D orig = CallOriginal_CVector2D(0x583530, in);
        CVector2D rev = CRadar::TransformRealWorldPointToRadarSpace(in);
        EXPECT_NEAR(orig.x, rev.x, 1e-3f);
        EXPECT_NEAR(orig.y, rev.y, 1e-3f);
    }
}
