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

// --- Differential tests for overloaded hooks ---

GAME_DIFF_TEST(CRect, IsPointInside_Diff) {
    CRect rect(0.f, 0.f, 100.f, 100.f);
    CVector2D points[] = { {50.f,50.f}, {0.f,0.f}, {100.f,100.f}, {-1.f,50.f}, {50.f,101.f} };
    for (auto& pt : points) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CRect/IsPointInside-");
          orig = rect.IsPointInside(pt); }
        rev = rect.IsPointInside(pt);
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(CRect, IsPointInside_Tolerance_Diff) {
    CRect rect(0.f, 0.f, 100.f, 100.f);
    CVector2D points[] = { {-5.f,50.f}, {50.f,-5.f}, {105.f,50.f}, {50.f,105.f} };
    for (auto& pt : points) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CRect/IsPointInside-Tolerance");
          orig = rect.IsPointInside(pt, 10.f); }
        rev = rect.IsPointInside(pt, 10.f);
        EXPECT_EQ(orig, rev);
    }
}

// Resize takes different args. Skipped.

GAME_DIFF_TEST(CRect, Restrict_Diff) {
    CRect rect(10.f, 10.f, 50.f, 50.f);
    CRect bounds(0.f, 0.f, 40.f, 40.f);
    CRect origRect = rect, revRect = rect;
    { HookDisableGuard guard("Global/CRect/Restrict");
      origRect.Restrict(bounds); }
    revRect.Restrict(bounds);
    EXPECT_NEAR(origRect.left, revRect.left, 1e-5f);
    EXPECT_NEAR(origRect.right, revRect.right, 1e-5f);
    EXPECT_NEAR(origRect.top, revRect.top, 1e-5f);
    EXPECT_NEAR(origRect.bottom, revRect.bottom, 1e-5f);
}

GAME_TEST(CRect, StretchToPoint) {
    CRect rect(10.0f, 10.0f, 20.0f, 20.0f);
    rect.StretchToPoint(25.0f, 5.0f);
    EXPECT_NEAR(rect.right, 25.0f, 1e-5f);
    EXPECT_NEAR(rect.bottom, 5.0f, 1e-5f);
}
