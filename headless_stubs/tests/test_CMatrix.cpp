// test_CMatrix.cpp — Behavior tests for CMatrix.
//
// CMatrix hooks are commented out in InjectHooksMain.cpp (inlined everywhere),
// so differential testing is not possible. These are pure math validation tests.

#include "StdInc.h"
#include "TestFramework.h"
#include "Core/Matrix.h"
#include "Quaternion.h"

static void ExpectMatrixNear(const CMatrix& a, const CMatrix& b, float eps) {
    EXPECT_NEAR(a.GetRight().x,   b.GetRight().x,   eps);
    EXPECT_NEAR(a.GetRight().y,   b.GetRight().y,   eps);
    EXPECT_NEAR(a.GetRight().z,   b.GetRight().z,   eps);
    EXPECT_NEAR(a.GetForward().x, b.GetForward().x, eps);
    EXPECT_NEAR(a.GetForward().y, b.GetForward().y, eps);
    EXPECT_NEAR(a.GetForward().z, b.GetForward().z, eps);
    EXPECT_NEAR(a.GetUp().x,     b.GetUp().x,      eps);
    EXPECT_NEAR(a.GetUp().y,     b.GetUp().y,      eps);
    EXPECT_NEAR(a.GetUp().z,     b.GetUp().z,      eps);
    EXPECT_NEAR(a.GetPosition().x, b.GetPosition().x, eps);
    EXPECT_NEAR(a.GetPosition().y, b.GetPosition().y, eps);
    EXPECT_NEAR(a.GetPosition().z, b.GetPosition().z, eps);
}

static CMatrix MakeTestMatrix() {
    CMatrix m;
    m.SetRotate(0.3f, 0.7f, 1.1f);
    m.GetPosition() = CVector(10.0f, -5.0f, 3.0f);
    return m;
}

// ---------------------------------------------------------------------------
// Identity / Unity
// ---------------------------------------------------------------------------

GAME_TEST(CMatrix, SetUnity_IsIdentity) {
    CMatrix m = MakeTestMatrix();
    m.SetUnity();
    EXPECT_NEAR(m.GetRight().x, 1.0f, 1e-6f);
    EXPECT_NEAR(m.GetRight().y, 0.0f, 1e-6f);
    EXPECT_NEAR(m.GetRight().z, 0.0f, 1e-6f);
    EXPECT_NEAR(m.GetForward().y, 1.0f, 1e-6f);
    EXPECT_NEAR(m.GetUp().z, 1.0f, 1e-6f);
    EXPECT_NEAR(m.GetPosition().x, 0.0f, 1e-6f);
    EXPECT_NEAR(m.GetPosition().y, 0.0f, 1e-6f);
    EXPECT_NEAR(m.GetPosition().z, 0.0f, 1e-6f);
}

// ---------------------------------------------------------------------------
// Scale
// ---------------------------------------------------------------------------

GAME_TEST(CMatrix, SetScale_Uniform) {
    CMatrix m;
    m.SetScale(2.0f);
    EXPECT_NEAR(m.GetRight().x, 2.0f, 1e-6f);
    EXPECT_NEAR(m.GetForward().y, 2.0f, 1e-6f);
    EXPECT_NEAR(m.GetUp().z, 2.0f, 1e-6f);
    // Off-diagonals zero
    EXPECT_NEAR(m.GetRight().y, 0.0f, 1e-6f);
    EXPECT_NEAR(m.GetRight().z, 0.0f, 1e-6f);
    EXPECT_NEAR(m.GetPosition().x, 0.0f, 1e-6f);
}

GAME_TEST(CMatrix, SetScale_NonUniform) {
    CMatrix m;
    m.SetScale(1.0f, 2.0f, 3.0f);
    EXPECT_NEAR(m.GetRight().x, 1.0f, 1e-6f);
    EXPECT_NEAR(m.GetForward().y, 2.0f, 1e-6f);
    EXPECT_NEAR(m.GetUp().z, 3.0f, 1e-6f);
}

GAME_TEST(CMatrix, Scale_MultipliesAxes) {
    CMatrix m;
    m.SetRotate(0.5f, 0.3f, 0.7f);
    CMatrix before = m;
    m.Scale(2.0f);
    EXPECT_NEAR(m.GetRight().x, before.GetRight().x * 2.0f, 1e-5f);
    EXPECT_NEAR(m.GetForward().y, before.GetForward().y * 2.0f, 1e-5f);
    EXPECT_NEAR(m.GetUp().z, before.GetUp().z * 2.0f, 1e-5f);
}

// ---------------------------------------------------------------------------
// Rotation — single axis
// ---------------------------------------------------------------------------

GAME_TEST(CMatrix, SetRotateX_90deg) {
    CMatrix m;
    m.SetRotateX(1.5707963f); // pi/2
    // Right unchanged (rotation around X)
    EXPECT_NEAR(m.GetRight().x, 1.0f, 1e-5f);
    EXPECT_NEAR(m.GetRight().y, 0.0f, 1e-5f);
    // Forward -> Up direction
    EXPECT_NEAR(m.GetForward().y, 0.0f, 1e-5f);
    EXPECT_NEAR(m.GetForward().z, 1.0f, 1e-5f);
    EXPECT_NEAR(m.GetPosition().x, 0.0f, 1e-6f);
}

GAME_TEST(CMatrix, SetRotateY_90deg) {
    CMatrix m;
    m.SetRotateY(1.5707963f);
    EXPECT_NEAR(m.GetForward().y, 1.0f, 1e-5f);
    EXPECT_NEAR(m.GetRight().x, 0.0f, 1e-5f);
    EXPECT_NEAR(m.GetRight().z, -1.0f, 1e-5f);
}

GAME_TEST(CMatrix, SetRotateZ_90deg) {
    CMatrix m;
    m.SetRotateZ(1.5707963f);
    EXPECT_NEAR(m.GetUp().z, 1.0f, 1e-5f);
    EXPECT_NEAR(m.GetRight().x, 0.0f, 1e-5f);
    EXPECT_NEAR(m.GetRight().y, 1.0f, 1e-5f);
}

// ---------------------------------------------------------------------------
// SetRotate(x,y,z) — combined rotation
// ---------------------------------------------------------------------------

GAME_TEST(CMatrix, SetRotate_Identity) {
    CMatrix m;
    m.SetRotate(0.0f, 0.0f, 0.0f);
    EXPECT_NEAR(m.GetRight().x, 1.0f, 1e-6f);
    EXPECT_NEAR(m.GetForward().y, 1.0f, 1e-6f);
    EXPECT_NEAR(m.GetUp().z, 1.0f, 1e-6f);
}

GAME_TEST(CMatrix, SetRotate_OrthogonalAxes) {
    // Any rotation matrix should have orthogonal basis vectors
    float combos[][3] = {
        {0.3f,0.7f,1.1f}, {-1,-2,-3}, {3.14f,1.57f,0.5f}, {5.0f,5.0f,5.0f}
    };
    for (auto& c : combos) {
        CMatrix m;
        m.SetRotate(c[0], c[1], c[2]);
        float dot_rf = m.GetRight().x * m.GetForward().x +
                       m.GetRight().y * m.GetForward().y +
                       m.GetRight().z * m.GetForward().z;
        float dot_ru = m.GetRight().x * m.GetUp().x +
                       m.GetRight().y * m.GetUp().y +
                       m.GetRight().z * m.GetUp().z;
        float dot_fu = m.GetForward().x * m.GetUp().x +
                       m.GetForward().y * m.GetUp().y +
                       m.GetForward().z * m.GetUp().z;
        EXPECT_NEAR(dot_rf, 0.0f, 1e-4f);
        EXPECT_NEAR(dot_ru, 0.0f, 1e-4f);
        EXPECT_NEAR(dot_fu, 0.0f, 1e-4f);
    }
}

// ---------------------------------------------------------------------------
// SetRotate(quat)
// ---------------------------------------------------------------------------

GAME_TEST(CMatrix, SetRotate_Quat_Identity) {
    CMatrix m;
    m.SetRotate(CQuaternion(0, 0, 0, 1.0f));
    EXPECT_NEAR(m.GetRight().x, 1.0f, 1e-5f);
    EXPECT_NEAR(m.GetForward().y, 1.0f, 1e-5f);
    EXPECT_NEAR(m.GetUp().z, 1.0f, 1e-5f);
}

GAME_TEST(CMatrix, SetRotate_Quat_180X) {
    CMatrix m;
    m.SetRotate(CQuaternion(1, 0, 0, 0));  // 180 around X
    EXPECT_NEAR(m.GetRight().x, 1.0f, 1e-4f);
    EXPECT_NEAR(m.GetForward().y, -1.0f, 1e-4f);
    EXPECT_NEAR(m.GetUp().z, -1.0f, 1e-4f);
}

// ---------------------------------------------------------------------------
// Euler angle round-trip
// ---------------------------------------------------------------------------

GAME_TEST(CMatrix, EulerRoundTrip_TaitBryan) {
    uint32 orders[] = { ORDER_XYZ, ORDER_XZY, ORDER_YZX, ORDER_YXZ, ORDER_ZXY, ORDER_ZYX };
    for (uint32 order : orders) {
        float x = 0.3f, y = 0.5f, z = 0.7f;
        CMatrix m;
        m.ConvertFromEulerAngles(x, y, z, order);
        float rx, ry, rz;
        m.ConvertToEulerAngles(&rx, &ry, &rz, order);
        EXPECT_NEAR(rx, x, 1e-4f);
        EXPECT_NEAR(ry, y, 1e-4f);
        EXPECT_NEAR(rz, z, 1e-4f);
    }
}

// ---------------------------------------------------------------------------
// Reorthogonalise
// ---------------------------------------------------------------------------

GAME_TEST(CMatrix, Reorthogonalise_ProducesOrthonormal) {
    CMatrix m = MakeTestMatrix();
    m.GetRight().x += 0.05f;
    m.Reorthogonalise();
    float dot_rf = m.GetRight().x * m.GetForward().x +
                   m.GetRight().y * m.GetForward().y +
                   m.GetRight().z * m.GetForward().z;
    float dot_ru = m.GetRight().x * m.GetUp().x +
                   m.GetRight().y * m.GetUp().y +
                   m.GetRight().z * m.GetUp().z;
    EXPECT_NEAR(dot_rf, 0.0f, 1e-4f);
    EXPECT_NEAR(dot_ru, 0.0f, 1e-4f);
    // Basis vectors should be unit length
    float len_r = sqrtf(m.GetRight().x*m.GetRight().x + m.GetRight().y*m.GetRight().y + m.GetRight().z*m.GetRight().z);
    float len_f = sqrtf(m.GetForward().x*m.GetForward().x + m.GetForward().y*m.GetForward().y + m.GetForward().z*m.GetForward().z);
    float len_u = sqrtf(m.GetUp().x*m.GetUp().x + m.GetUp().y*m.GetUp().y + m.GetUp().z*m.GetUp().z);
    EXPECT_NEAR(len_r, 1.0f, 1e-4f);
    EXPECT_NEAR(len_f, 1.0f, 1e-4f);
    EXPECT_NEAR(len_u, 1.0f, 1e-4f);
}

// ---------------------------------------------------------------------------
// ForceUpVector
// ---------------------------------------------------------------------------

GAME_TEST(CMatrix, ForceUpVector_SetsUp) {
    CMatrix m = MakeTestMatrix();
    CVector up(0, 0, 1);
    m.ForceUpVector(up);
    EXPECT_NEAR(m.GetUp().x, 0.0f, 1e-5f);
    EXPECT_NEAR(m.GetUp().y, 0.0f, 1e-5f);
    EXPECT_NEAR(m.GetUp().z, 1.0f, 1e-5f);
    // Right and Forward should be perpendicular to Up
    float dot_ru = m.GetRight().x * up.x + m.GetRight().y * up.y + m.GetRight().z * up.z;
    float dot_fu = m.GetForward().x * up.x + m.GetForward().y * up.y + m.GetForward().z * up.z;
    EXPECT_NEAR(dot_ru, 0.0f, 1e-4f);
    EXPECT_NEAR(dot_fu, 0.0f, 1e-4f);
}

// ---------------------------------------------------------------------------
// Translate
// ---------------------------------------------------------------------------

GAME_TEST(CMatrix, SetTranslate_ResetsOrientation) {
    CMatrix m = MakeTestMatrix();
    m.SetTranslate(CVector(5, 6, 7));
    EXPECT_NEAR(m.GetRight().x, 1.0f, 1e-6f);
    EXPECT_NEAR(m.GetForward().y, 1.0f, 1e-6f);
    EXPECT_NEAR(m.GetUp().z, 1.0f, 1e-6f);
    EXPECT_NEAR(m.GetPosition().x, 5.0f, 1e-6f);
    EXPECT_NEAR(m.GetPosition().y, 6.0f, 1e-6f);
    EXPECT_NEAR(m.GetPosition().z, 7.0f, 1e-6f);
}

GAME_TEST(CMatrix, SetTranslateOnly_KeepsOrientation) {
    CMatrix m = MakeTestMatrix();
    float origRight = m.GetRight().x;
    m.SetTranslateOnly(CVector(99, 88, 77));
    EXPECT_NEAR(m.GetRight().x, origRight, 1e-6f);
    EXPECT_NEAR(m.GetPosition().x, 99.0f, 1e-6f);
}

// ---------------------------------------------------------------------------
// Rotate (applies rotation to existing matrix)
// ---------------------------------------------------------------------------

GAME_TEST(CMatrix, Rotate_ZeroIsIdentity) {
    CMatrix m = MakeTestMatrix();
    CMatrix before = m;
    m.Rotate(CVector(0, 0, 0));
    ExpectMatrixNear(m, before, 1e-5f);
}

// ---------------------------------------------------------------------------
// Operator +=
// ---------------------------------------------------------------------------

GAME_TEST(CMatrix, OperatorPlusEquals_Adds) {
    CMatrix a;
    a.SetScale(1.0f);
    a.GetPosition() = CVector(1, 2, 3);
    CMatrix b;
    b.SetScale(2.0f);
    b.GetPosition() = CVector(4, 5, 6);
    a += b;
    EXPECT_NEAR(a.GetRight().x, 3.0f, 1e-6f);
    EXPECT_NEAR(a.GetPosition().x, 5.0f, 1e-6f);
    EXPECT_NEAR(a.GetPosition().y, 7.0f, 1e-6f);
}

// ---------------------------------------------------------------------------
// Invert
// ---------------------------------------------------------------------------

GAME_TEST(CMatrix, Invert_TimesOriginal_IsIdentity) {
    CMatrix m = MakeTestMatrix();
    CMatrix inv = Invert(m);
    // m * inv should be ~identity
    CMatrix result = m;
    result *= inv;
    EXPECT_NEAR(result.GetRight().x, 1.0f, 1e-3f);
    EXPECT_NEAR(result.GetForward().y, 1.0f, 1e-3f);
    EXPECT_NEAR(result.GetUp().z, 1.0f, 1e-3f);
    EXPECT_NEAR(result.GetRight().y, 0.0f, 1e-3f);
    EXPECT_NEAR(result.GetPosition().x, 0.0f, 1e-2f);
    EXPECT_NEAR(result.GetPosition().y, 0.0f, 1e-2f);
    EXPECT_NEAR(result.GetPosition().z, 0.0f, 1e-2f);
}

// ---------------------------------------------------------------------------
// TransformPoint / InverseTransformPoint round-trip
// ---------------------------------------------------------------------------

GAME_TEST(CMatrix, TransformInverseRoundTrip) {
    CMatrix m = MakeTestMatrix();
    CVector pt(1.0f, 2.0f, 3.0f);
    CVector transformed = m.TransformPoint(pt);
    CVector recovered = m.InverseTransformPoint(transformed);
    EXPECT_NEAR(recovered.x, pt.x, 1e-3f);
    EXPECT_NEAR(recovered.y, pt.y, 1e-3f);
    EXPECT_NEAR(recovered.z, pt.z, 1e-3f);
}
