// test_CVector.cpp — Behavior tests for CVector/CVector2D.
// CVector hooks are disabled upstream (InjectHooksMain.cpp) so no differential tests.

#include "StdInc.h"
#include "TestFramework.h"

// --- CVector ---

GAME_TEST(CVector, Constructor_Default) {
    CVector v{};
    EXPECT_VEC_NEAR(v, CVector(0, 0, 0), 1e-7f);
}

GAME_TEST(CVector, Constructor_XYZ) {
    CVector v(1.0f, 2.0f, 3.0f);
    EXPECT_VEC_NEAR(v, CVector(1, 2, 3), 1e-7f);
}

GAME_TEST(CVector, Magnitude_345) {
    EXPECT_NEAR(CVector(3, 4, 0).Magnitude(), 5.0f, 1e-5f);
}

GAME_TEST(CVector, Magnitude_Zero) {
    EXPECT_NEAR(CVector(0, 0, 0).Magnitude(), 0.0f, 1e-7f);
}

GAME_TEST(CVector, Magnitude2D) {
    EXPECT_NEAR(CVector(3, 4, 999).Magnitude2D(), 5.0f, 1e-5f);
}

GAME_TEST(CVector, Normalise_UnitLength) {
    CVector v(3.0f, 4.0f, 0.0f);
    v.Normalise();
    EXPECT_NEAR(v.Magnitude(), 1.0f, 1e-5f);
    EXPECT_NEAR(v.x, 0.6f, 1e-5f);
    EXPECT_NEAR(v.y, 0.8f, 1e-5f);
}

GAME_TEST(CVector, Normalise_AlreadyUnit) {
    CVector v(1, 0, 0);
    v.Normalise();
    EXPECT_VEC_NEAR(v, CVector(1, 0, 0), 1e-5f);
}

GAME_TEST(CVector, Addition) {
    EXPECT_VEC_NEAR(CVector(1,2,3) + CVector(4,5,6), CVector(5,7,9), 1e-7f);
}

GAME_TEST(CVector, Subtraction) {
    EXPECT_VEC_NEAR(CVector(4,5,6) - CVector(1,2,3), CVector(3,3,3), 1e-7f);
}

GAME_TEST(CVector, ScalarMultiply) {
    EXPECT_VEC_NEAR(CVector(1,2,3) * 2.0f, CVector(2,4,6), 1e-7f);
}

// --- CVector2D ---

GAME_TEST(CVector2D, Magnitude) {
    EXPECT_NEAR(CVector2D(3, 4).Magnitude(), 5.0f, 1e-5f);
}

GAME_TEST(CVector2D, Normalise) {
    CVector2D v(3, 4);
    v.Normalise();
    EXPECT_NEAR(v.Magnitude(), 1.0f, 1e-5f);
}
