// test_CRadar2.cpp — Deep differential tests for CRadar.
// Hook paths: Global/CRadar/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "Radar.h"

// --- CalculateBlipAlpha ---

GAME_DIFF_TEST(CRadar, CalculateBlipAlpha_Various) {
    float distances[] = { 0.0f, 10.0f, 50.0f, 100.0f, 200.0f, 500.0f, 1000.0f };
    for (float d : distances) {
        uint8 orig, rev;
        { HookDisableGuard guard("Global/CRadar/CalculateBlipAlpha"); orig = CRadar::CalculateBlipAlpha(d); }
        rev = CRadar::CalculateBlipAlpha(d);
        EXPECT_EQ(orig, rev);
    }
}

// --- GetRadarTraceColour ---

GAME_DIFF_TEST(CRadar, GetRadarTraceColour_AllColors) {
    for (int32 color = 0; color <= 8; color++) {
        for (bool bright : { false, true }) {
            for (bool friendly : { false, true }) {
                uint32 orig, rev;
                { HookDisableGuard guard("Global/CRadar/GetRadarTraceColour");
                  orig = CRadar::GetRadarTraceColour(static_cast<eBlipColour>(color), bright, friendly); }
                rev = CRadar::GetRadarTraceColour(static_cast<eBlipColour>(color), bright, friendly);
                EXPECT_EQ(orig, rev);
            }
        }
    }
}

// --- TransformRealWorldPointToRadarSpace ---

GAME_DIFF_TEST(CRadar, TransformRealWorldToRadar_Various) {
    CVector2D points[] = {
        CVector2D(2495.0f, -1665.0f),  // Grove Street
        CVector2D(0.0f, 0.0f),          // Origin
        CVector2D(-2170.0f, 250.0f),    // San Fierro
        CVector2D(2027.0f, 1008.0f),    // Las Venturas
    };
    for (auto& p : points) {
        CVector2D orig, rev;
        { HookDisableGuard guard("Global/CRadar/TransformRealWorldPointToRadarSpace");
          orig = CRadar::TransformRealWorldPointToRadarSpace(p); }
        rev = CRadar::TransformRealWorldPointToRadarSpace(p);
        EXPECT_NEAR(orig.x, rev.x, 1e-3f);
        EXPECT_NEAR(orig.y, rev.y, 1e-3f);
    }
}

// --- TransformRadarPointToScreenSpace ---

GAME_DIFF_TEST(CRadar, TransformRadarToScreen_Various) {
    CVector2D points[] = {
        CVector2D(0.0f, 0.0f),
        CVector2D(0.5f, 0.5f),
        CVector2D(-1.0f, -1.0f),
        CVector2D(1.0f, 1.0f),
    };
    for (auto& p : points) {
        CVector2D orig, rev;
        { HookDisableGuard guard("Global/CRadar/TransformRadarPointToScreenSpace");
          orig = CRadar::TransformRadarPointToScreenSpace(p); }
        rev = CRadar::TransformRadarPointToScreenSpace(p);
        EXPECT_NEAR(orig.x, rev.x, 1e-3f);
        EXPECT_NEAR(orig.y, rev.y, 1e-3f);
    }
}

// --- GetActualBlipArrayIndex ---

GAME_DIFF_TEST(CRadar, GetActualBlipArrayIndex_Various) {
    int32 handles[] = { 0, 1, 5, 10, 50, 100 };
    for (int32 h : handles) {
        int32 orig, rev;
        { HookDisableGuard guard("Global/CRadar/GetActualBlipArrayIndex");
          orig = CRadar::GetActualBlipArrayIndex(h); }
        rev = CRadar::GetActualBlipArrayIndex(h);
        EXPECT_EQ(orig, rev);
    }
}
