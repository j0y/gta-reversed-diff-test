// test_CRect.cpp — Tests for CRect.

#include "StdInc.h"
#include "TestFramework.h"
#include "Core/Rect.h"

GAME_TEST(CRect, DefaultConstructor_IsFlipped) {
    CRect rect;
    EXPECT_TRUE(rect.IsFlipped());
}

GAME_TEST(CRect, ParameterizedConstructor) {
    CRect rect(0.0f, 0.0f, 100.0f, 100.0f);
    EXPECT_NEAR(rect.left, 0.0f, 1e-5f);
    EXPECT_NEAR(rect.bottom, 0.0f, 1e-5f);
    EXPECT_NEAR(rect.right, 100.0f, 1e-5f);
    EXPECT_NEAR(rect.top, 100.0f, 1e-5f);
    EXPECT_FALSE(rect.IsFlipped());
}

GAME_TEST(CRect, GetCenter) {
    CRect rect(0.0f, 0.0f, 100.0f, 200.0f);
    CVector2D center = rect.GetCenter();
    EXPECT_NEAR(center.x, 50.0f, 1e-5f);
    EXPECT_NEAR(center.y, 100.0f, 1e-5f);
}

GAME_TEST(CRect, IsPointInside_Inside) {
    CRect rect(0.0f, 0.0f, 100.0f, 100.0f);
    EXPECT_TRUE(rect.IsPointInside(CVector2D(50.0f, 50.0f)));
}

GAME_TEST(CRect, IsPointInside_Outside) {
    CRect rect(0.0f, 0.0f, 100.0f, 100.0f);
    EXPECT_FALSE(rect.IsPointInside(CVector2D(150.0f, 50.0f)));
    EXPECT_FALSE(rect.IsPointInside(CVector2D(50.0f, -10.0f)));
}

GAME_TEST(CRect, FromVector2D) {
    CRect rect(CVector2D(10.0f, 20.0f), CVector2D(50.0f, 80.0f));
    EXPECT_NEAR(rect.left, 10.0f, 1e-5f);
    EXPECT_NEAR(rect.bottom, 20.0f, 1e-5f);
    EXPECT_NEAR(rect.right, 50.0f, 1e-5f);
    EXPECT_NEAR(rect.top, 80.0f, 1e-5f);
}

GAME_TEST(CRect, FromCenterAndRadius) {
    CRect rect(CVector2D(50.0f, 50.0f), 10.0f);
    EXPECT_NEAR(rect.left, 40.0f, 1e-5f);
    EXPECT_NEAR(rect.bottom, 40.0f, 1e-5f);
    EXPECT_NEAR(rect.right, 60.0f, 1e-5f);
    EXPECT_NEAR(rect.top, 60.0f, 1e-5f);
}

GAME_TEST(CRect, GetTopLeft) {
    CRect rect(10.0f, 20.0f, 50.0f, 80.0f);
    auto tl = rect.GetTopLeft();
    EXPECT_NEAR(tl.x, 10.0f, 1e-5f);
    EXPECT_NEAR(tl.y, 80.0f, 1e-5f);
}

GAME_TEST(CRect, GetCorners3D) {
    CRect rect(0.0f, 0.0f, 10.0f, 10.0f);
    auto corners = rect.GetCorners3D(5.0f);
    EXPECT_EQ(corners.size(), (size_t)4);
    EXPECT_NEAR(corners[0].z, 5.0f, 1e-5f);
    EXPECT_NEAR(corners[2].x, 10.0f, 1e-5f);
}

GAME_TEST(CRect, Equality) {
    CRect a(0.0f, 0.0f, 10.0f, 10.0f);
    CRect b(0.0f, 0.0f, 10.0f, 10.0f);
    CRect c(0.0f, 0.0f, 20.0f, 10.0f);
    EXPECT_TRUE(a == b);
    EXPECT_TRUE(a != c);
}

GAME_TEST(CRect, SizeValidation) {
    EXPECT_EQ(sizeof(CRect), (size_t)0x10);
}

// CRect::IsPointInside and Restrict are not hooked — behavior tests only

GAME_TEST(CRect, IsPointInside_Behavior) {
    CRect rect(0.f, 0.f, 100.f, 100.f);
    EXPECT_TRUE(rect.IsPointInside(CVector2D(50.f, 50.f)));
    EXPECT_TRUE(rect.IsPointInside(CVector2D(0.f, 0.f)));
    EXPECT_FALSE(rect.IsPointInside(CVector2D(-1.f, 50.f)));
    EXPECT_FALSE(rect.IsPointInside(CVector2D(50.f, 101.f)));
}

GAME_TEST(CRect, IsPointInside_Tolerance_Behavior) {
    CRect rect(0.f, 0.f, 100.f, 100.f);
    EXPECT_TRUE(rect.IsPointInside(CVector2D(-5.f, 50.f), 10.f));
    EXPECT_FALSE(rect.IsPointInside(CVector2D(-15.f, 50.f), 10.f));
}

// CRect::Restrict not hooked — behavior test
GAME_TEST(CRect, Restrict_Behavior) {
    CRect rect(10.f, 10.f, 50.f, 50.f);
    CRect bounds(0.f, 0.f, 40.f, 40.f);
    rect.Restrict(bounds);
    EXPECT_TRUE(rect.right <= 40.f);
    EXPECT_TRUE(rect.bottom <= 40.f);
}

GAME_TEST(CRect, StretchToPoint) {
    CRect rect(10.0f, 10.0f, 20.0f, 20.0f);
    rect.StretchToPoint(25.0f, 5.0f);
    EXPECT_NEAR(rect.right, 25.0f, 1e-5f);
    EXPECT_NEAR(rect.bottom, 5.0f, 1e-5f);
}
