// test_CColTriangle.cpp — Tests for CColTriangle.

#include "StdInc.h"
#include "TestFramework.h"
#include "Collision/ColTriangle.h"

GAME_TEST(CColTriangle, DefaultConstructor) {
    CColTriangle tri;
    (void)tri;
    EXPECT_TRUE(true); // no crash
}

GAME_TEST(CColTriangle, ParameterizedConstructor) {
    CColTriangle tri(0, 1, 2, SURFACE_TARMAC, tColLighting(0x55));
    EXPECT_EQ(tri.vA, (uint16)0);
    EXPECT_EQ(tri.vB, (uint16)1);
    EXPECT_EQ(tri.vC, (uint16)2);
    EXPECT_EQ((int32)tri.m_nMaterial, (int32)SURFACE_TARMAC);
    EXPECT_EQ(tri.m_nLight.day, (uint8)5);
    EXPECT_EQ(tri.m_nLight.night, (uint8)5);
}

GAME_TEST(CColTriangle, VertexIndices_Array) {
    CColTriangle tri(10, 20, 30, SURFACE_DEFAULT, tColLighting(0));
    EXPECT_EQ(tri.m_vertIndices[0], (uint16)10);
    EXPECT_EQ(tri.m_vertIndices[1], (uint16)20);
    EXPECT_EQ(tri.m_vertIndices[2], (uint16)30);
}

GAME_TEST(CColTriangle, GetSurfaceType) {
    CColTriangle tri(0, 1, 2, SURFACE_GRASS_SHORT_LUSH, tColLighting(0));
    EXPECT_EQ((int32)tri.GetSurfaceType(), (int32)SURFACE_GRASS_SHORT_LUSH);
}

GAME_TEST(CColTriangle, DifferentSurfaces) {
    eSurfaceType surfaces[] = { SURFACE_DEFAULT, SURFACE_TARMAC, SURFACE_GRASS_SHORT_LUSH, SURFACE_SAND_DEEP };
    for (auto s : surfaces) {
        CColTriangle tri(0, 1, 2, s, tColLighting(0));
        EXPECT_EQ((int32)tri.GetSurfaceType(), (int32)s);
    }
}

GAME_TEST(CColTriangle, SizeValidation) {
    EXPECT_EQ(sizeof(CColTriangle), (size_t)0x8);
}

GAME_TEST(CColTriangle, Lighting_Values) {
    CColTriangle tri(0, 1, 2, SURFACE_DEFAULT, tColLighting(0xAB));
    EXPECT_EQ(tri.m_nLight.day, (uint8)0xB);
    EXPECT_EQ(tri.m_nLight.night, (uint8)0xA);
}
