// test_CRect2.cpp — Differential tests for CRect geometry functions.
// Hook paths: Core/CRect/<fn>
//
// Pure 2D rectangle math: point-in-rect, restrict, resize, center, stretch.

#include "StdInc.h"
#include "TestFramework.h"
#include "Core/Rect.h"

// --- IsFlipped ---

GAME_DIFF_TEST(CRect, IsFlipped) {
    struct Case { float l, t, r, b; };
    Case cases[] = {
        {0, 0, 10, 10},    // normal
        {10, 10, 0, 0},    // flipped both
        {10, 0, 0, 10},    // flipped X
        {0, 10, 10, 0},    // flipped Y
        {5, 5, 5, 5},      // zero size
    };
    for (auto& c : cases) {
        CRect orig(c.l, c.t, c.r, c.b), rev(c.l, c.t, c.r, c.b);
        bool o, r;
        { HookDisableGuard guard("Core/CRect/IsFlipped");
          o = orig.IsFlipped(); }
        r = rev.IsFlipped();
        EXPECT_EQ(o, r);
    }
}

// --- IsPointInside ---

GAME_DIFF_TEST(CRect, IsPointInside) {
    CRect rect(0, 0, 100, 100);
    CVector2D points[] = {
        {50, 50}, {0, 0}, {100, 100}, {-1, 50}, {50, -1},
        {101, 50}, {50, 101}, {0, 100}, {100, 0},
    };
    for (auto& pt : points) {
        CRect o = rect, r = rect;
        bool orig, rev;
        { HookDisableGuard guard("Core/CRect/IsPointInside-");
          orig = o.IsPointInside(pt); }
        rev = r.IsPointInside(pt);
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(CRect, IsPointInside_Tolerance) {
    CRect rect(10, 10, 90, 90);
    CVector2D points[] = { {5, 50}, {95, 50}, {50, 5}, {50, 95}, {50, 50} };
    float tolerances[] = { 0.0f, 5.0f, 10.0f, 20.0f };
    for (float tol : tolerances) {
        for (auto& pt : points) {
            CRect o = rect, r = rect;
            bool orig, rev;
            { HookDisableGuard guard("Core/CRect/IsPointInside-Tolerance");
              orig = o.IsPointInside(pt, tol); }
            rev = r.IsPointInside(pt, tol);
            EXPECT_EQ(orig, rev);
        }
    }
}

// --- Restrict ---

GAME_DIFF_TEST(CRect, Restrict) {
    struct Case { float l1,t1,r1,b1, l2,t2,r2,b2; };
    Case cases[] = {
        {0,0,100,100, 20,20,80,80},     // inner
        {0,0,100,100, -50,-50,150,150},  // outer
        {0,0,100,100, 50,50,200,200},    // partial overlap
        {10,10,50,50, 10,10,50,50},      // same
    };
    for (auto& c : cases) {
        CRect orig(c.l1,c.t1,c.r1,c.b1), rev(c.l1,c.t1,c.r1,c.b1);
        CRect bounds(c.l2,c.t2,c.r2,c.b2);
        { HookDisableGuard guard("Core/CRect/Restrict");
          orig.Restrict(bounds); }
        rev.Restrict(bounds);
        EXPECT_NEAR(orig.left, rev.left, 1e-5f);
        EXPECT_NEAR(orig.top, rev.top, 1e-5f);
        EXPECT_NEAR(orig.right, rev.right, 1e-5f);
        EXPECT_NEAR(orig.bottom, rev.bottom, 1e-5f);
    }
}

// --- Resize ---

GAME_DIFF_TEST(CRect, Resize) {
    float deltas[][2] = { {0,0}, {5,5}, {-5,-5}, {10,0}, {0,10}, {-20,-20} };
    for (auto& d : deltas) {
        CRect orig(10,10,90,90), rev(10,10,90,90);
        { HookDisableGuard guard("Core/CRect/Resize");
          orig.Resize(d[0], d[1]); }
        rev.Resize(d[0], d[1]);
        EXPECT_NEAR(orig.left, rev.left, 1e-5f);
        EXPECT_NEAR(orig.top, rev.top, 1e-5f);
        EXPECT_NEAR(orig.right, rev.right, 1e-5f);
        EXPECT_NEAR(orig.bottom, rev.bottom, 1e-5f);
    }
}

// --- SetFromCenter ---

GAME_DIFF_TEST(CRect, SetFromCenter) {
    struct Case { float x, y, sz; };
    Case cases[] = { {0,0,10}, {50,50,5}, {-10,20,100}, {0,0,0} };
    for (auto& c : cases) {
        CRect orig{}, rev{};
        { HookDisableGuard guard("Core/CRect/SetFromCenter");
          orig.SetFromCenter(c.x, c.y, c.sz); }
        rev.SetFromCenter(c.x, c.y, c.sz);
        EXPECT_NEAR(orig.left, rev.left, 1e-5f);
        EXPECT_NEAR(orig.top, rev.top, 1e-5f);
        EXPECT_NEAR(orig.right, rev.right, 1e-5f);
        EXPECT_NEAR(orig.bottom, rev.bottom, 1e-5f);
    }
}

// --- GetCenter ---

GAME_DIFF_TEST(CRect, GetCenter) {
    CRect rects[] = { {0,0,100,100}, {-50,-50,50,50}, {10,20,30,40} };
    for (auto& rect : rects) {
        CRect o = rect, r = rect;
        float ox, oy, rx, ry;
        { HookDisableGuard guard("Core/CRect/GetCenter-");
          o.GetCenter(&ox, &oy); }
        r.GetCenter(&rx, &ry);
        EXPECT_NEAR(ox, rx, 1e-5f);
        EXPECT_NEAR(oy, ry, 1e-5f);
    }
}

// --- StretchToPoint ---

GAME_DIFF_TEST(CRect, StretchToPoint) {
    struct Case { float px, py; };
    Case cases[] = { {50,50}, {-10,-10}, {200,200}, {0,0}, {150,50} };
    for (auto& c : cases) {
        CRect orig(10,10,100,100), rev(10,10,100,100);
        { HookDisableGuard guard("Core/CRect/StretchToPoint");
          orig.StretchToPoint(c.px, c.py); }
        rev.StretchToPoint(c.px, c.py);
        EXPECT_NEAR(orig.left, rev.left, 1e-5f);
        EXPECT_NEAR(orig.top, rev.top, 1e-5f);
        EXPECT_NEAR(orig.right, rev.right, 1e-5f);
        EXPECT_NEAR(orig.bottom, rev.bottom, 1e-5f);
    }
}
