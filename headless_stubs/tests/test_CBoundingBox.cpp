// test_CBoundingBox.cpp — Tests for CBox and CBoundingBox.
// Hook paths: CBox/<fn>, CBoundingBox/<fn>
//
// Tests geometric operations: containment checks, size queries,
// center computation, and stretch-to-point.

#include "StdInc.h"
#include "TestFramework.h"
#include "Collision/BoundingBox.h"

// --- CBox basic operations ---

GAME_TEST(CBox, DefaultConstructor) {
    CBox box;
    // Default is zero-initialized
    EXPECT_NEAR(box.m_vecMin.x, 0.0f, 1e-5f);
    EXPECT_NEAR(box.m_vecMax.x, 0.0f, 1e-5f);
}

GAME_TEST(CBox, ParameterizedConstructor) {
    CBox box(CVector(-1, -2, -3), CVector(4, 5, 6));
    EXPECT_NEAR(box.m_vecMin.x, -1.0f, 1e-5f);
    EXPECT_NEAR(box.m_vecMin.y, -2.0f, 1e-5f);
    EXPECT_NEAR(box.m_vecMin.z, -3.0f, 1e-5f);
    EXPECT_NEAR(box.m_vecMax.x, 4.0f, 1e-5f);
    EXPECT_NEAR(box.m_vecMax.y, 5.0f, 1e-5f);
    EXPECT_NEAR(box.m_vecMax.z, 6.0f, 1e-5f);
}

GAME_TEST(CBox, GetSize) {
    CBox box(CVector(0, 0, 0), CVector(10, 20, 30));
    CVector size = box.GetSize();
    EXPECT_NEAR(size.x, 10.0f, 1e-5f);
    EXPECT_NEAR(size.y, 20.0f, 1e-5f);
    EXPECT_NEAR(size.z, 30.0f, 1e-5f);
}

GAME_TEST(CBox, GetWidthLengthHeight) {
    CBox box(CVector(1, 2, 3), CVector(5, 8, 15));
    EXPECT_NEAR(box.GetWidth(), 4.0f, 1e-5f);
    EXPECT_NEAR(box.GetLength(), 6.0f, 1e-5f);
    EXPECT_NEAR(box.GetHeight(), 12.0f, 1e-5f);
}

GAME_TEST(CBox, GetCenter) {
    CBox box(CVector(0, 0, 0), CVector(10, 20, 30));
    CVector center = box.GetCenter();
    EXPECT_NEAR(center.x, 5.0f, 1e-5f);
    EXPECT_NEAR(center.y, 10.0f, 1e-5f);
    EXPECT_NEAR(center.z, 15.0f, 1e-5f);
}

GAME_TEST(CBox, GetCenter_Negative) {
    CBox box(CVector(-10, -20, -30), CVector(10, 20, 30));
    CVector center = box.GetCenter();
    EXPECT_NEAR(center.x, 0.0f, 1e-5f);
    EXPECT_NEAR(center.y, 0.0f, 1e-5f);
    EXPECT_NEAR(center.z, 0.0f, 1e-5f);
}

GAME_TEST(CBox, IsPointInside_Inside) {
    CBox box(CVector(0, 0, 0), CVector(10, 10, 10));
    EXPECT_TRUE(box.IsPointInside(CVector(5, 5, 5)));
}

GAME_TEST(CBox, IsPointInside_Outside) {
    CBox box(CVector(0, 0, 0), CVector(10, 10, 10));
    EXPECT_FALSE(box.IsPointInside(CVector(15, 5, 5)));
    EXPECT_FALSE(box.IsPointInside(CVector(5, -1, 5)));
    EXPECT_FALSE(box.IsPointInside(CVector(5, 5, 11)));
}

GAME_TEST(CBox, Set) {
    CBox box;
    box.Set(CVector(1, 2, 3), CVector(4, 5, 6));
    EXPECT_NEAR(box.m_vecMin.x, 1.0f, 1e-5f);
    EXPECT_NEAR(box.m_vecMax.z, 6.0f, 1e-5f);
}

GAME_TEST(CBox, StretchToPoint) {
    CBox box(CVector(0, 0, 0), CVector(10, 10, 10));
    box.StretchToPoint(CVector(15, -5, 5));
    EXPECT_NEAR(box.m_vecMax.x, 15.0f, 1e-5f);
    EXPECT_NEAR(box.m_vecMin.y, -5.0f, 1e-5f);
    // Z should remain unchanged
    EXPECT_NEAR(box.m_vecMin.z, 0.0f, 1e-5f);
    EXPECT_NEAR(box.m_vecMax.z, 10.0f, 1e-5f);
}

// --- CBoundingBox ---

GAME_TEST(CBoundingBox, DefaultConstructor) {
    CBoundingBox bb;
    // Default: min=1, max=-1 (inverted)
    EXPECT_NEAR(bb.m_vecMin.x, 1.0f, 1e-5f);
    EXPECT_NEAR(bb.m_vecMax.x, -1.0f, 1e-5f);
}

GAME_TEST(CBoundingBox, ParameterizedConstructor) {
    CBoundingBox bb(CVector(-5, -5, -5), CVector(5, 5, 5));
    EXPECT_NEAR(bb.m_vecMin.x, -5.0f, 1e-5f);
    EXPECT_NEAR(bb.m_vecMax.x, 5.0f, 1e-5f);
}

GAME_TEST(CBoundingBox, IsPointWithin_Inside) {
    CBoundingBox bb(CVector(0, 0, 0), CVector(10, 10, 10));
    EXPECT_TRUE(bb.IsPointWithin(CVector(5, 5, 5)));
}

GAME_TEST(CBoundingBox, IsPointWithin_Outside) {
    CBoundingBox bb(CVector(0, 0, 0), CVector(10, 10, 10));
    EXPECT_FALSE(bb.IsPointWithin(CVector(15, 5, 5)));
}

// --- CBox GetShortestVectorDistToPt ---

GAME_TEST(CBox, GetShortestVectorDistToPt_Inside) {
    CBox box(CVector(0, 0, 0), CVector(10, 10, 10));
    CVector dist = box.GetShortestVectorDistToPt(CVector(5, 5, 5));
    // Inside: distance is 0
    EXPECT_NEAR(dist.x, 0.0f, 1e-5f);
    EXPECT_NEAR(dist.y, 0.0f, 1e-5f);
    EXPECT_NEAR(dist.z, 0.0f, 1e-5f);
}

GAME_TEST(CBox, GetShortestVectorDistToPt_Outside) {
    CBox box(CVector(0, 0, 0), CVector(10, 10, 10));
    CVector dist = box.GetShortestVectorDistToPt(CVector(15, 5, 5));
    // Only X is outside (by 5 units)
    EXPECT_NEAR(dist.x, 5.0f, 1e-5f);
    EXPECT_NEAR(dist.y, 0.0f, 1e-5f);
    EXPECT_NEAR(dist.z, 0.0f, 1e-5f);
}
