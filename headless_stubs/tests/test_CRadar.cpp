// test_CRadar.cpp — Differential tests for CRadar.
// Hook paths: Global/CRadar/<fn>

#include "StdInc.h"
#include "TestFramework.h"

GAME_DIFF_TEST(CRadar, CalculateBlipAlpha) {
    float distances[] = { 0.f, 10.f, 50.f, 100.f, 500.f, 1000.f, 5000.f };
    for (float d : distances) {
        uint8 orig, rev;
        { HookDisableGuard guard("Global/CRadar/CalculateBlipAlpha"); orig = CRadar::CalculateBlipAlpha(d); }
        rev = CRadar::CalculateBlipAlpha(d);
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(CRadar, GetRadarTraceColour) {
    eBlipColour colors[] = { BLIP_COLOUR_RED, BLIP_COLOUR_GREEN, BLIP_COLOUR_BLUE, BLIP_COLOUR_WHITE };
    for (auto color : colors) {
        for (bool bright : {true, false}) {
            for (bool friendly : {true, false}) {
                uint32 orig, rev;
                { HookDisableGuard guard("Global/CRadar/GetRadarTraceColour");
                  orig = CRadar::GetRadarTraceColour(color, bright, friendly); }
                rev = CRadar::GetRadarTraceColour(color, bright, friendly);
                EXPECT_EQ(orig, rev);
            }
        }
    }
}

GAME_DIFF_TEST(CRadar, LimitRadarPoint) {
    CVector2D points[] = { {0,0}, {0.5f,0.5f}, {1.5f,0}, {0,2.0f}, {-1,-1}, {3,4} };
    for (auto p : points) {
        CVector2D origP = p, revP = p;
        float origR, revR;
        { HookDisableGuard guard("Global/CRadar/LimitRadarPoint"); origR = CRadar::LimitRadarPoint(origP); }
        revR = CRadar::LimitRadarPoint(revP);
        EXPECT_NEAR(origR, revR, 1e-5f);
        EXPECT_NEAR(origP.x, revP.x, 1e-5f);
        EXPECT_NEAR(origP.y, revP.y, 1e-5f);
    }
}

GAME_DIFF_TEST(CRadar, TransformRealWorldPointToRadarSpace) {
    CVector2D points[] = { {0,0}, {2488,-1666}, {-2200,2300}, {500,500} };
    for (auto& p : points) {
        CVector2D orig, rev;
        { HookDisableGuard guard("Global/CRadar/TransformRealWorldPointToRadarSpace");
          orig = CRadar::TransformRealWorldPointToRadarSpace(p); }
        rev = CRadar::TransformRealWorldPointToRadarSpace(p);
        EXPECT_NEAR(orig.x, rev.x, 1e-3f);
        EXPECT_NEAR(orig.y, rev.y, 1e-3f);
    }
}

GAME_DIFF_TEST(CRadar, TransformRadarPointToRealWorldSpace) {
    CVector2D points[] = { {0,0}, {0.5f,0.5f}, {-0.5f,-0.5f}, {1,0} };
    for (auto& p : points) {
        CVector2D orig, rev;
        { HookDisableGuard guard("Global/CRadar/TransformRadarPointToRealWorldSpace");
          orig = CRadar::TransformRadarPointToRealWorldSpace(p); }
        rev = CRadar::TransformRadarPointToRealWorldSpace(p);
        EXPECT_NEAR(orig.x, rev.x, 1e-1f); // world coords — wider tolerance
        EXPECT_NEAR(orig.y, rev.y, 1e-1f);
    }
}

// FindTraceNotTrackingBlipIndex — not hooked, skip
