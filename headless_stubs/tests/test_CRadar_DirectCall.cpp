// test_CRadar_DirectCall.cpp — Differential tests using direct asm calls
// to work around the sret calling convention issue.

#include "StdInc.h"
#include "TestFramework.h"
#include "Radar.h"

// --- TransformRadarPointToScreenSpace (0x583480) ---

GAME_TEST(CRadar_DirectCall, TransformRadarPointToScreenSpace) {
    CVector2D inputs[] = {
        {0.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 1.0f}, {-1.0f, 0.0f},
        {0.5f, 0.5f}, {-0.5f, -0.5f}, {0.25f, 0.75f},
    };
    for (auto& in : inputs) {
        CVector2D orig = CallOriginal_CVector2D(0x583480, in);
        CVector2D rev = CRadar::TransformRadarPointToScreenSpace(in);
        EXPECT_NEAR(orig.x, rev.x, 1e-3f);
        EXPECT_NEAR(orig.y, rev.y, 1e-3f);
    }
}

// --- TransformRealWorldPointToRadarSpace (0x583530) ---

GAME_TEST(CRadar_DirectCall, TransformRealWorldPointToRadarSpace) {
    CVector2D inputs[] = {
        {2495.0f, -1665.0f}, {0.0f, 0.0f}, {-2170.0f, 250.0f}, {2027.0f, 1008.0f},
    };
    for (auto& in : inputs) {
        CVector2D orig = CallOriginal_CVector2D(0x583530, in);
        CVector2D rev = CRadar::TransformRealWorldPointToRadarSpace(in);
        EXPECT_NEAR(orig.x, rev.x, 1e-3f);
        EXPECT_NEAR(orig.y, rev.y, 1e-3f);
    }
}
