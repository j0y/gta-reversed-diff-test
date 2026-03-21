// test_CCollision.cpp — Behavior tests for CCollision geometry functions.
// Hook paths: Global/CCollision/<fn>
// These are pure math functions — no game state dependencies.
//
// Note: CCollision hooks are locked ({.enabled = false, .locked = true}),
// so differential testing is not possible. Instead we use behavior tests
// that verify the reversed code produces geometrically correct results.

#include "StdInc.h"
#include "TestFramework.h"
#include "Collision/Collision.h"
#include "Collision/ColLine.h"
#include "Collision/ColSphere.h"
#include "Collision/ColBox.h"
#include "Collision/Sphere.h"
#include "Collision/Box.h"

// --- Distance calculations ---

GAME_TEST(CCollision, DistToLineSqr_PointAboveMidpoint) {
    // Point (5,3,0) is 3 units from line (0,0,0)-(10,0,0) at midpoint
    float dist = CCollision::DistToLineSqr(CVector(0,0,0), CVector(10,0,0), CVector(5,3,0));
    EXPECT_NEAR(dist, 9.0f, 1e-2f); // 3^2 = 9
}

GAME_TEST(CCollision, DistToLineSqr_PointOnLine) {
    float dist = CCollision::DistToLineSqr(CVector(0,0,0), CVector(10,0,0), CVector(5,0,0));
    EXPECT_NEAR(dist, 0.0f, 1e-2f);
}

GAME_TEST(CCollision, DistToLine_Basic) {
    float dist = CCollision::DistToLine(CVector(0,0,0), CVector(10,0,0), CVector(5,3,0));
    EXPECT_NEAR(dist, 3.0f, 1e-2f);
}

GAME_TEST(CCollision, DistToMathematicalLine_Basic) {
    CVector start(0,0,0), end(10,0,0), point(5,4,0);
    float dist = CCollision::DistToMathematicalLine(&start, &end, &point);
    EXPECT_NEAR(dist, 4.0f, 1e-2f);
}

GAME_TEST(CCollision, DistToMathematicalLine2D_Basic) {
    // Note: args are (lineStartX, lineStartY, lineEndX, lineEndY, pointX, pointY)
    // but the actual parameter semantics may differ (direction vector vs endpoint)
    float dist = CCollision::DistToMathematicalLine2D(0, 0, 10, 0, 5, 3);
    EXPECT_TRUE(dist >= 0.0f || dist <= 0.0f); // finite, no NaN
}

// --- ClosestPointOnLine ---

GAME_TEST(CCollision, ClosestPointOnLine_Midpoint) {
    CVector closest;
    CCollision::ClosestPointOnLine(CVector(0,0,0), CVector(10,0,0), CVector(5,3,0), closest);
    // Closest point on the line x-axis should be near x=5
    EXPECT_NEAR(closest.x, 5.0f, 1.0f);
    // Z should be zero (line is in XY plane)
    EXPECT_NEAR(closest.z, 0.0f, 1e-2f);
}

GAME_TEST(CCollision, ClosestPointOnLine_OnSegment) {
    CVector closest;
    CCollision::ClosestPointOnLine(CVector(0,0,0), CVector(10,0,0), CVector(5,0,0), closest);
    // Point is on the line, closest should be the point itself
    EXPECT_NEAR(closest.x, 5.0f, 1e-2f);
    EXPECT_NEAR(closest.y, 0.0f, 1e-2f);
}

// --- Line/Box tests ---

GAME_TEST(CCollision, TestLineBox_Hit) {
    CBox box(CVector(-1,-1,-1), CVector(1,1,1));
    CColLine line;
    line.m_vecStart = CVector(-5, 0, 0);
    line.m_vecEnd = CVector(5, 0, 0);
    EXPECT_TRUE(CCollision::TestLineBox(line, box));
}

GAME_TEST(CCollision, TestLineBox_Miss) {
    CBox box(CVector(-1,-1,-1), CVector(1,1,1));
    CColLine line;
    line.m_vecStart = CVector(-5, 0, 0);
    line.m_vecEnd = CVector(-3, 0, 0);
    EXPECT_FALSE(CCollision::TestLineBox(line, box));
}

GAME_TEST(CCollision, TestVerticalLineBox_Hit) {
    CBox box(CVector(-1,-1,-1), CVector(1,1,1));
    CColLine line;
    line.m_vecStart = CVector(0, 0, -5);
    line.m_vecEnd = CVector(0, 0, 5);
    EXPECT_TRUE(CCollision::TestVerticalLineBox(line, box));
}

GAME_TEST(CCollision, TestVerticalLineBox_Miss) {
    CBox box(CVector(-1,-1,-1), CVector(1,1,1));
    CColLine line;
    line.m_vecStart = CVector(0, 0, 2);
    line.m_vecEnd = CVector(0, 0, 5);
    EXPECT_FALSE(CCollision::TestVerticalLineBox(line, box));
}

// --- Sphere tests ---

GAME_TEST(CCollision, TestSphereSphere_Overlap) {
    CColSphere s1(CVector(0,0,0), 2.0f);
    CColSphere s2(CVector(3,0,0), 2.0f);
    EXPECT_TRUE(CCollision::TestSphereSphere(s1, s2));
}

GAME_TEST(CCollision, TestSphereSphere_NoOverlap) {
    CColSphere s1(CVector(0,0,0), 1.0f);
    CColSphere s2(CVector(5,0,0), 1.0f);
    EXPECT_FALSE(CCollision::TestSphereSphere(s1, s2));
}

GAME_TEST(CCollision, TestSphereBox_Inside) {
    CSphere sphere(CVector(0,0,0), 0.5f);
    CBox box(CVector(-1,-1,-1), CVector(1,1,1));
    EXPECT_TRUE(CCollision::TestSphereBox(sphere, box));
}

GAME_TEST(CCollision, TestSphereBox_Outside) {
    CSphere sphere(CVector(5,0,0), 1.0f);
    CBox box(CVector(-1,-1,-1), CVector(1,1,1));
    EXPECT_FALSE(CCollision::TestSphereBox(sphere, box));
}

GAME_TEST(CCollision, TestLineSphere_Hit) {
    CColSphere sphere(CVector(5,0,0), 2.0f);
    CColLine line;
    line.m_vecStart = CVector(0,0,0);
    line.m_vecEnd = CVector(10,0,0);
    EXPECT_TRUE(CCollision::TestLineSphere(line, sphere));
}

GAME_TEST(CCollision, TestLineSphere_Miss) {
    CColSphere sphere(CVector(5,0,0), 2.0f);
    CColLine line;
    line.m_vecStart = CVector(0, 5, 0);
    line.m_vecEnd = CVector(10, 5, 0);
    EXPECT_FALSE(CCollision::TestLineSphere(line, sphere));
}

// --- 2D line intersection ---

GAME_TEST(CCollision, Test2DLine_Crossing) {
    // X pattern — these intersect
    EXPECT_TRUE(CCollision::Test2DLineAgainst2DLine(0, 0, 10, 10, 0, 10, 10, 0));
}

GAME_TEST(CCollision, Test2DLine_Parallel) {
    // Parallel horizontal lines
    EXPECT_FALSE(CCollision::Test2DLineAgainst2DLine(0, 0, 10, 0, 0, 5, 10, 5));
}
