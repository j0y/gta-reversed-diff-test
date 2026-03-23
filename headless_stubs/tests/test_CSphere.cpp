// test_CSphere.cpp — Tests for CSphere (base collision sphere).

#include "StdInc.h"
#include "TestFramework.h"
#include "Collision/Sphere.h"

GAME_TEST(CSphere, DefaultConstructor) {
    CSphere sphere;
    EXPECT_NEAR(sphere.m_fRadius, 0.0f, 1e-5f);
}

GAME_TEST(CSphere, ParameterizedConstructor) {
    CSphere sphere(CVector(1.0f, 2.0f, 3.0f), 5.0f);
    EXPECT_NEAR(sphere.m_vecCenter.x, 1.0f, 1e-5f);
    EXPECT_NEAR(sphere.m_vecCenter.y, 2.0f, 1e-5f);
    EXPECT_NEAR(sphere.m_vecCenter.z, 3.0f, 1e-5f);
    EXPECT_NEAR(sphere.m_fRadius, 5.0f, 1e-5f);
}

GAME_TEST(CSphere, Set) {
    CSphere sphere;
    sphere.Set(10.0f, CVector(4.0f, 5.0f, 6.0f));
    EXPECT_NEAR(sphere.m_fRadius, 10.0f, 1e-5f);
    EXPECT_NEAR(sphere.m_vecCenter.x, 4.0f, 1e-5f);
    EXPECT_NEAR(sphere.m_vecCenter.y, 5.0f, 1e-5f);
    EXPECT_NEAR(sphere.m_vecCenter.z, 6.0f, 1e-5f);
}

GAME_TEST(CSphere, IsPointWithin_Inside) {
    CSphere sphere(CVector(0.0f, 0.0f, 0.0f), 10.0f);
    EXPECT_TRUE(sphere.IsPointWithin(CVector(5.0f, 0.0f, 0.0f)));
    EXPECT_TRUE(sphere.IsPointWithin(CVector(0.0f, 0.0f, 0.0f))); // center
}

GAME_TEST(CSphere, IsPointWithin_Outside) {
    CSphere sphere(CVector(0.0f, 0.0f, 0.0f), 10.0f);
    EXPECT_FALSE(sphere.IsPointWithin(CVector(15.0f, 0.0f, 0.0f)));
    EXPECT_FALSE(sphere.IsPointWithin(CVector(0.0f, 11.0f, 0.0f)));
}

GAME_TEST(CSphere, IsPointWithin_OnBoundary) {
    CSphere sphere(CVector(0.0f, 0.0f, 0.0f), 10.0f);
    // Exactly on boundary — should be within (radius² >= dist²)
    EXPECT_TRUE(sphere.IsPointWithin(CVector(10.0f, 0.0f, 0.0f)));
}

GAME_TEST(CSphere, DifferentRadii) {
    float radii[] = { 0.0f, 0.5f, 1.0f, 10.0f, 100.0f };
    for (float r : radii) {
        CSphere sphere(CVector(0, 0, 0), r);
        EXPECT_NEAR(sphere.m_fRadius, r, 1e-5f);
    }
}

GAME_TEST(CSphere, SizeValidation) {
    EXPECT_EQ(sizeof(CSphere), (size_t)0x10);
}
