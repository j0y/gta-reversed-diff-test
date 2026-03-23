// test_CColPoint.cpp — Tests for CColPoint struct.
// CColPoint stores collision result data: contact point, normal,
// surface types, lighting, and depth.

#include "StdInc.h"
#include "TestFramework.h"
#include "Collision/ColPoint.h"

// --- Construction & assignment ---

GAME_TEST(CColPoint, DefaultConstruction) {
    CColPoint point{};
    // Zero-initialized fields
    EXPECT_NEAR(point.m_vecPoint.x, 0.0f, 1e-5f);
    EXPECT_NEAR(point.m_vecPoint.y, 0.0f, 1e-5f);
    EXPECT_NEAR(point.m_vecPoint.z, 0.0f, 1e-5f);
    EXPECT_NEAR(point.m_vecNormal.x, 0.0f, 1e-5f);
    EXPECT_NEAR(point.m_fDepth, 0.0f, 1e-5f);
}

GAME_TEST(CColPoint, Assignment) {
    CColPoint a{};
    a.m_vecPoint = CVector(1.0f, 2.0f, 3.0f);
    a.m_vecNormal = CVector(0.0f, 0.0f, 1.0f);
    a.m_fDepth = 5.0f;
    a.m_nSurfaceTypeA = SURFACE_DEFAULT;
    a.m_nPieceTypeA = 7;

    CColPoint b{};
    b = a;
    EXPECT_NEAR(b.m_vecPoint.x, 1.0f, 1e-5f);
    EXPECT_NEAR(b.m_vecPoint.y, 2.0f, 1e-5f);
    EXPECT_NEAR(b.m_vecPoint.z, 3.0f, 1e-5f);
    EXPECT_NEAR(b.m_vecNormal.z, 1.0f, 1e-5f);
    EXPECT_NEAR(b.m_fDepth, 5.0f, 1e-5f);
    EXPECT_EQ((int32)b.m_nSurfaceTypeA, (int32)SURFACE_DEFAULT);
    EXPECT_EQ(b.m_nPieceTypeA, (uint8)7);
}

// --- Lighting ---

GAME_TEST(CColPoint, Lighting_DayNight) {
    tColLighting lighting(0xAB);
    // Lower nibble = day, upper nibble = night
    EXPECT_EQ(lighting.day, (uint8)0xB);
    EXPECT_EQ(lighting.night, (uint8)0xA);
}

GAME_TEST(CColPoint, Lighting_Zero) {
    tColLighting lighting(0x00);
    EXPECT_EQ(lighting.day, (uint8)0);
    EXPECT_EQ(lighting.night, (uint8)0);
}

GAME_TEST(CColPoint, Lighting_Max) {
    tColLighting lighting(0xFF);
    EXPECT_EQ(lighting.day, (uint8)0xF);
    EXPECT_EQ(lighting.night, (uint8)0xF);
}

// --- Surface types for both shapes ---

GAME_TEST(CColPoint, SurfaceTypes) {
    CColPoint point{};
    point.m_nSurfaceTypeA = SURFACE_TARMAC;
    point.m_nSurfaceTypeB = SURFACE_GRASS_SHORT_LUSH;
    EXPECT_EQ((int32)point.m_nSurfaceTypeA, (int32)SURFACE_TARMAC);
    EXPECT_EQ((int32)point.m_nSurfaceTypeB, (int32)SURFACE_GRASS_SHORT_LUSH);
}

// --- Size validation ---

GAME_TEST(CColPoint, SizeIs0x2C) {
    EXPECT_EQ(sizeof(CColPoint), (size_t)0x2C);
}
