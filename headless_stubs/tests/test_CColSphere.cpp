// test_CColSphere.cpp — Differential tests for CColSphere.
// Hook paths: Collision/CColSphere/<fn>
//
// Pure geometry: ray-sphere, edge-sphere, point-in-sphere, sphere-sphere.
// No global state, no crash risk.

#include "StdInc.h"
#include "TestFramework.h"
#include "Collision/ColSphere.h"

// --- Set ---

GAME_DIFF_TEST(CColSphere, Set) {
    CVector centers[] = { {0,0,0}, {10,20,30}, {-5,0,100} };
    float radii[] = { 1.0f, 5.0f, 0.1f, 100.0f };
    for (auto& c : centers) {
        for (float r : radii) {
            CColSphere orig{}, rev{};
            { HookDisableGuard guard("Collision/CColSphere/Set");
              orig.Set(r, c, SURFACE_DEFAULT, 0, tColLighting{}); }
            rev.Set(r, c, SURFACE_DEFAULT, 0, tColLighting{});
            EXPECT_NEAR(orig.m_fRadius, rev.m_fRadius, 1e-6f);
            EXPECT_NEAR(orig.m_vecCenter.x, rev.m_vecCenter.x, 1e-6f);
            EXPECT_NEAR(orig.m_vecCenter.y, rev.m_vecCenter.y, 1e-6f);
            EXPECT_NEAR(orig.m_vecCenter.z, rev.m_vecCenter.z, 1e-6f);
        }
    }
}

// --- IntersectPoint ---

GAME_DIFF_TEST(CColSphere, IntersectPoint) {
    CColSphere sphere{};
    sphere.m_vecCenter = CVector(0, 0, 0);
    sphere.m_fRadius = 5.0f;

    CVector points[] = {
        {0,0,0},       // center — inside
        {3,0,0},       // inside
        {5,0,0},       // on surface (borderline)
        {6,0,0},       // outside
        {-3,4,0},      // inside (distance 5)
        {10,10,10},    // far outside
        {0.001f,0,0},  // barely inside
    };
    for (auto& p : points) {
        bool orig, rev;
        { HookDisableGuard guard("Collision/CColSphere/IntersectPoint");
          orig = sphere.IntersectPoint(p); }
        rev = sphere.IntersectPoint(p);
        EXPECT_EQ(orig, rev);
    }
}

// --- IntersectSphere ---

GAME_DIFF_TEST(CColSphere, IntersectSphere) {
    struct Case { CVector c1; float r1; CVector c2; float r2; };
    Case cases[] = {
        { {0,0,0}, 5, {0,0,0}, 3 },       // overlapping (concentric)
        { {0,0,0}, 5, {8,0,0}, 3 },       // touching
        { {0,0,0}, 5, {20,0,0}, 3 },      // far apart
        { {0,0,0}, 1, {1.5f,0,0}, 1 },    // overlapping
        { {0,0,0}, 1, {3,0,0}, 1 },       // not touching
        { {10,10,10}, 2, {12,10,10}, 2 },  // touching at distance 2
    };
    for (auto& c : cases) {
        CColSphere s1{}, s2{};
        s1.m_vecCenter = c.c1; s1.m_fRadius = c.r1;
        s2.m_vecCenter = c.c2; s2.m_fRadius = c.r2;
        bool orig, rev;
        { HookDisableGuard guard("Collision/CColSphere/IntersectSphere");
          orig = s1.IntersectSphere(s2); }
        rev = s1.IntersectSphere(s2);
        EXPECT_EQ(orig, rev);
    }
}

// --- IntersectRay ---

GAME_DIFF_TEST(CColSphere, IntersectRay) {
    CColSphere sphere{};
    sphere.m_vecCenter = CVector(0, 0, 0);
    sphere.m_fRadius = 5.0f;

    struct Case { CVector origin, dir; };
    Case cases[] = {
        { {-10,0,0}, {1,0,0} },    // hit through center
        { {0,-10,0}, {0,1,0} },    // hit through center Y
        { {-10,6,0}, {1,0,0} },    // miss (above)
        { {-10,0,0}, {0,1,0} },    // miss (perpendicular)
        { {3,0,0}, {1,0,0} },      // origin inside sphere
    };
    for (auto& c : cases) {
        CVector origP1, origP2, revP1, revP2;
        bool origHit, revHit;
        { HookDisableGuard guard("Collision/CColSphere/IntersectRay");
          origHit = sphere.IntersectRay(c.origin, c.dir, origP1, origP2); }
        revHit = sphere.IntersectRay(c.origin, c.dir, revP1, revP2);
        EXPECT_EQ(origHit, revHit);
        if (origHit && revHit) {
            EXPECT_NEAR(origP1.x, revP1.x, 1e-3f);
            EXPECT_NEAR(origP1.y, revP1.y, 1e-3f);
            EXPECT_NEAR(origP1.z, revP1.z, 1e-3f);
            EXPECT_NEAR(origP2.x, revP2.x, 1e-3f);
            EXPECT_NEAR(origP2.y, revP2.y, 1e-3f);
            EXPECT_NEAR(origP2.z, revP2.z, 1e-3f);
        }
    }
}

// --- IntersectEdge ---

GAME_DIFF_TEST(CColSphere, IntersectEdge) {
    CColSphere sphere{};
    sphere.m_vecCenter = CVector(0, 0, 0);
    sphere.m_fRadius = 5.0f;

    struct Case { CVector start, end; };
    Case cases[] = {
        { {-10,0,0}, {10,0,0} },   // line through center
        { {-10,6,0}, {10,6,0} },   // line misses
        { {-10,4,0}, {10,4,0} },   // line grazes
        { {-3,0,0}, {3,0,0} },     // both ends inside
        { {-10,0,0}, {-6,0,0} },   // line too short to reach
        { {0,0,0}, {1,0,0} },      // starts at center
    };
    for (auto& c : cases) {
        CVector origP1, origP2, revP1, revP2;
        bool origHit, revHit;
        { HookDisableGuard guard("Collision/CColSphere/IntersectEdge");
          origHit = sphere.IntersectEdge(c.start, c.end, origP1, origP2); }
        revHit = sphere.IntersectEdge(c.start, c.end, revP1, revP2);
        EXPECT_EQ(origHit, revHit);
        if (origHit && revHit) {
            EXPECT_NEAR(origP1.x, revP1.x, 1e-2f);
            EXPECT_NEAR(origP1.y, revP1.y, 1e-2f);
            EXPECT_NEAR(origP2.x, revP2.x, 1e-2f);
            EXPECT_NEAR(origP2.y, revP2.y, 1e-2f);
        }
    }
}
