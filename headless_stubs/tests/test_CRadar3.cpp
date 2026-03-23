// test_CRadar3.cpp — Extended differential tests for CRadar.
// Hook paths: Global/CRadar/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "Radar.h"

// --- LimitRadarPoint ---

GAME_DIFF_TEST(CRadar, LimitRadarPoint_Inside) {
    CVector2D point(0.5f, 0.5f);
    CVector2D origPt = point, revPt = point;
    { HookDisableGuard guard("Global/CRadar/LimitRadarPoint");
      CRadar::LimitRadarPoint(origPt); }
    CRadar::LimitRadarPoint(revPt);
    EXPECT_NEAR(origPt.x, revPt.x, 1e-3f);
    EXPECT_NEAR(origPt.y, revPt.y, 1e-3f);
}

GAME_DIFF_TEST(CRadar, LimitRadarPoint_Outside) {
    CVector2D point(5.0f, 5.0f);
    CVector2D origPt = point, revPt = point;
    { HookDisableGuard guard("Global/CRadar/LimitRadarPoint");
      CRadar::LimitRadarPoint(origPt); }
    CRadar::LimitRadarPoint(revPt);
    EXPECT_NEAR(origPt.x, revPt.x, 1e-3f);
    EXPECT_NEAR(origPt.y, revPt.y, 1e-3f);
}

GAME_DIFF_TEST(CRadar, LimitRadarPoint_Various) {
    CVector2D points[] = {
        CVector2D(0.0f, 0.0f),
        CVector2D(1.0f, 0.0f),
        CVector2D(0.0f, 1.0f),
        CVector2D(-1.0f, -1.0f),
        CVector2D(2.0f, 3.0f),
        CVector2D(-0.5f, 0.8f),
    };
    for (auto& p : points) {
        CVector2D origPt = p, revPt = p;
        { HookDisableGuard guard("Global/CRadar/LimitRadarPoint");
          CRadar::LimitRadarPoint(origPt); }
        CRadar::LimitRadarPoint(revPt);
        EXPECT_NEAR(origPt.x, revPt.x, 1e-3f);
        EXPECT_NEAR(origPt.y, revPt.y, 1e-3f);
    }
}

// --- CalculateBlipAlpha extended ---

GAME_DIFF_TEST(CRadar, CalculateBlipAlpha_ExtendedRange) {
    float distances[] = { 0.0f, 1.0f, 5.0f, 25.0f, 75.0f, 150.0f, 300.0f, 600.0f, 1500.0f, 5000.0f };
    for (float d : distances) {
        uint8 orig, rev;
        { HookDisableGuard guard("Global/CRadar/CalculateBlipAlpha"); orig = CRadar::CalculateBlipAlpha(d); }
        rev = CRadar::CalculateBlipAlpha(d);
        EXPECT_EQ(orig, rev);
    }
}
