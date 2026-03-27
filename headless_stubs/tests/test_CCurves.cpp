// test_CCurves.cpp — Differential tests for CCurves.
// Hook paths: Global/CCurves/<fn>
//
// Pure math functions for path/curve calculations used by vehicle AI.
// No global state dependencies.

#include "StdInc.h"
#include "TestFramework.h"
#include "Curves.h"

// --- DistForLineToCrossOtherLine (8 float params) ---

GAME_DIFF_TEST(CCurves, DistForLineToCrossOtherLine) {
    struct Case { float bx1,by1,dx1,dy1, bx2,by2,dx2,dy2; };
    Case cases[] = {
        // Perpendicular cross at origin
        { -1,0, 2,0,  0,-1, 0,2 },
        // Parallel
        { 0,0, 1,0,  0,1, 1,0 },
        // Diagonal cross
        { 0,0, 10,10, 10,0, -10,10 },
        // T intersection
        { 0,0, 10,0,  5,-5, 0,10 },
        // Near-parallel
        { 0,0, 100,1,  0,1, 100,2 },
    };
    for (auto& c : cases) {
        float orig, rev;
        { HookDisableGuard guard("Global/CCurves/DistForLineToCrossOtherLine");
          orig = CCurves::DistForLineToCrossOtherLine(
              c.bx1,c.by1,c.dx1,c.dy1, c.bx2,c.by2,c.dx2,c.dy2); }
        rev = CCurves::DistForLineToCrossOtherLine(
            c.bx1,c.by1,c.dx1,c.dy1, c.bx2,c.by2,c.dx2,c.dy2);
        EXPECT_NEAR(orig, rev, 1e-3f);
    }
}

// --- CalcSpeedVariationInBend (CVector start, end + 4 dir floats) ---

GAME_DIFF_TEST(CCurves, CalcSpeedVariationInBend) {
    struct Case { CVector s, e; float sdx,sdy,edx,edy; };
    Case cases[] = {
        { {0,0,0}, {10,0,0}, 1,0, 1,0 },       // straight
        { {0,0,0}, {10,10,0}, 1,0, 0,1 },       // 90 degree turn
        { {0,0,0}, {5,0,0}, 1,0, 0.707f,0.707f },// 45 degree
        { {0,0,0}, {10,0,0}, 1,0, -1,0 },       // 180 U-turn
    };
    for (auto& c : cases) {
        float orig, rev;
        { HookDisableGuard guard("Global/CCurves/CalcSpeedVariationInBend");
          orig = CCurves::CalcSpeedVariationInBend(c.s, c.e, c.sdx,c.sdy,c.edx,c.edy); }
        rev = CCurves::CalcSpeedVariationInBend(c.s, c.e, c.sdx,c.sdy,c.edx,c.edy);
        EXPECT_NEAR(orig, rev, 1e-4f);
    }
}

// --- CalcSpeedScaleFactor ---

GAME_DIFF_TEST(CCurves, CalcSpeedScaleFactor) {
    struct Case { CVector s, e; float sdx,sdy,edx,edy; };
    Case cases[] = {
        { {0,0,0}, {10,0,0}, 1,0, 1,0 },
        { {0,0,0}, {10,10,0}, 1,0, 0,1 },
        { {-5,-5,0}, {5,5,0}, 0.707f,0.707f, 0.707f,0.707f },
    };
    for (auto& c : cases) {
        float orig, rev;
        { HookDisableGuard guard("Global/CCurves/CalcSpeedScaleFactor");
          orig = CCurves::CalcSpeedScaleFactor(c.s, c.e, c.sdx,c.sdy,c.edx,c.edy); }
        rev = CCurves::CalcSpeedScaleFactor(c.s, c.e, c.sdx,c.sdy,c.edx,c.edy);
        EXPECT_NEAR(orig, rev, 1e-3f);
    }
}

// --- CalcCorrectedDist ---

GAME_DIFF_TEST(CCurves, CalcCorrectedDist) {
    float fractions[] = { 0.0f, 0.1f, 0.25f, 0.5f, 0.75f, 1.0f };
    float totals[] = { 1.0f, 5.0f, 10.0f };
    float speeds[] = { 0.0f, 0.1f, 0.333f };

    for (float total : totals) {
        for (float speed : speeds) {
            for (float frac : fractions) {
                float current = frac * total;
                float origT, revT;
                { HookDisableGuard guard("Global/CCurves/CalcCorrectedDist");
                  origT = CCurves::CalcCorrectedDist(current, total, speed, &origT); }
                revT = CCurves::CalcCorrectedDist(current, total, speed, &revT);
                EXPECT_NEAR(origT, revT, 1e-3f);
            }
        }
    }
}

// --- CalcCurvePoint (CVector params) ---

GAME_DIFF_TEST(CCurves, CalcCurvePoint) {
    struct Case { CVector p0, p1, d0, d1; float t; };
    Case cases[] = {
        { {0,0,0}, {10,0,0}, {1,0,0}, {1,0,0}, 0.0f },
        { {0,0,0}, {10,0,0}, {1,0,0}, {1,0,0}, 0.5f },
        { {0,0,0}, {10,0,0}, {1,0,0}, {1,0,0}, 1.0f },
        { {0,0,0}, {10,10,0}, {1,0,0}, {0,1,0}, 0.25f },
        { {0,0,0}, {10,10,0}, {1,0,0}, {0,1,0}, 0.5f },
        { {0,0,0}, {10,10,0}, {1,0,0}, {0,1,0}, 0.75f },
    };
    for (auto& c : cases) {
        CVector origPos, revPos, origSpd, revSpd;
        { HookDisableGuard guard("Global/CCurves/CalcCurvePoint");
          CCurves::CalcCurvePoint(c.p0, c.p1, c.d0, c.d1, c.t, 1000, origPos, origSpd); }
        CCurves::CalcCurvePoint(c.p0, c.p1, c.d0, c.d1, c.t, 1000, revPos, revSpd);
        EXPECT_NEAR(origPos.x, revPos.x, 1e-2f);
        EXPECT_NEAR(origPos.y, revPos.y, 1e-2f);
        EXPECT_NEAR(origPos.z, revPos.z, 1e-2f);
        EXPECT_NEAR(origSpd.x, revSpd.x, 1e-2f);
        EXPECT_NEAR(origSpd.y, revSpd.y, 1e-2f);
        EXPECT_NEAR(origSpd.z, revSpd.z, 1e-2f);
    }
}
