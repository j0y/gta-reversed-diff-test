// test_CColLine.cpp — Tests for CColLine.
// Collision line segment with start/end points and sizes.

#include "StdInc.h"
#include "TestFramework.h"
#include "Collision/ColLine.h"

GAME_TEST(CColLine, DefaultConstructor) {
    CColLine line;
    // Default-constructed, fields may be uninitialized but should not crash
    (void)line;
    EXPECT_TRUE(true);
}

GAME_TEST(CColLine, ParameterizedConstructor) {
    CColLine line(CVector(1.0f, 2.0f, 3.0f), CVector(4.0f, 5.0f, 6.0f));
    EXPECT_NEAR(line.m_vecStart.x, 1.0f, 1e-5f);
    EXPECT_NEAR(line.m_vecStart.y, 2.0f, 1e-5f);
    EXPECT_NEAR(line.m_vecStart.z, 3.0f, 1e-5f);
    EXPECT_NEAR(line.m_vecEnd.x, 4.0f, 1e-5f);
    EXPECT_NEAR(line.m_vecEnd.y, 5.0f, 1e-5f);
    EXPECT_NEAR(line.m_vecEnd.z, 6.0f, 1e-5f);
}

GAME_TEST(CColLine, Set) {
    CColLine line;
    line.Set(CVector(10.0f, 20.0f, 30.0f), CVector(40.0f, 50.0f, 60.0f));
    EXPECT_NEAR(line.m_vecStart.x, 10.0f, 1e-5f);
    EXPECT_NEAR(line.m_vecEnd.z, 60.0f, 1e-5f);
}

GAME_TEST(CColLine, IsVertical_True) {
    CColLine line(CVector(5.0f, 5.0f, 0.0f), CVector(5.0f, 5.0f, 100.0f));
    EXPECT_TRUE(line.IsVertical());
}

GAME_TEST(CColLine, IsVertical_False) {
    CColLine line(CVector(0.0f, 0.0f, 0.0f), CVector(10.0f, 0.0f, 0.0f));
    EXPECT_FALSE(line.IsVertical());
}

GAME_TEST(CColLine, IsVertical_Diagonal) {
    CColLine line(CVector(0.0f, 0.0f, 0.0f), CVector(1.0f, 1.0f, 10.0f));
    EXPECT_FALSE(line.IsVertical());
}

GAME_TEST(CColLine, SizeValidation) {
    EXPECT_EQ(sizeof(CColLine), (size_t)0x20);
}
