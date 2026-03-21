// test_CCompressedMatrixNotAligned.cpp — Differential + behavior tests.
// Hook paths: Global/CCompressedMatrixNotAligned/<fn>
//
// Compress/decompress 3x4 matrices using int8 fixed-point vectors.
// Lossy compression (right/forward vectors quantized to int8/127),
// up vector reconstructed via cross product + reorthogonalization.
// Position stored as full float CVector (no loss).

#include "StdInc.h"
#include "TestFramework.h"
#include "CompressedMatrixNotAligned.h"

// --- Differential tests ---

GAME_DIFF_TEST(CCompressedMatrixNotAligned, CompressFromFullMatrix) {
    // Test with several known matrices
    CMatrix matrices[] = {
        CMatrix{}, // identity-ish
    };
    // Set up identity
    matrices[0].GetRight()    = CVector(1.0f, 0.0f, 0.0f);
    matrices[0].GetForward()  = CVector(0.0f, 1.0f, 0.0f);
    matrices[0].GetUp()       = CVector(0.0f, 0.0f, 1.0f);
    matrices[0].GetPosition() = CVector(100.0f, 200.0f, 10.0f);

    for (auto& mat : matrices) {
        CCompressedMatrixNotAligned revComp{};
        revComp.CompressFromFullMatrix(mat);
        CVector revPos = revComp.m_vecPos;

        CCompressedMatrixNotAligned origComp{};
        { HookDisableGuard guard("Global/CCompressedMatrixNotAligned/CompressFromFullMatrix");
          origComp.CompressFromFullMatrix(mat); }

        // Position should match exactly (stored as float)
        EXPECT_NEAR(revPos.x, origComp.m_vecPos.x, 1e-5f);
        EXPECT_NEAR(revPos.y, origComp.m_vecPos.y, 1e-5f);
        EXPECT_NEAR(revPos.z, origComp.m_vecPos.z, 1e-5f);
        // Compressed vectors (int8) should match exactly
        EXPECT_EQ(memcmp(&revComp.m_vecRight, &origComp.m_vecRight, sizeof(revComp.m_vecRight)), 0);
        EXPECT_EQ(memcmp(&revComp.m_vecForward, &origComp.m_vecForward, sizeof(revComp.m_vecForward)), 0);
    }
}

GAME_DIFF_TEST(CCompressedMatrixNotAligned, DecompressIntoFullMatrix) {
    // Compress a known matrix, then decompress and compare
    CMatrix source{};
    source.GetRight()    = CVector(0.707f, 0.707f, 0.0f);
    source.GetForward()  = CVector(-0.707f, 0.707f, 0.0f);
    source.GetUp()       = CVector(0.0f, 0.0f, 1.0f);
    source.GetPosition() = CVector(50.0f, -30.0f, 5.0f);

    CCompressedMatrixNotAligned comp{};
    comp.CompressFromFullMatrix(source);

    // Reversed decompress
    CMatrix revMat{};
    comp.DecompressIntoFullMatrix(revMat);

    // Original decompress
    CMatrix origMat{};
    { HookDisableGuard guard("Global/CCompressedMatrixNotAligned/DecompressIntoFullMatrix");
      comp.DecompressIntoFullMatrix(origMat); }

    // Position should match exactly
    EXPECT_NEAR(revMat.GetPosition().x, origMat.GetPosition().x, 1e-5f);
    EXPECT_NEAR(revMat.GetPosition().y, origMat.GetPosition().y, 1e-5f);
    EXPECT_NEAR(revMat.GetPosition().z, origMat.GetPosition().z, 1e-5f);

    // Right/forward/up vectors should match (both go through same reorthogonalization)
    EXPECT_NEAR(revMat.GetRight().x, origMat.GetRight().x, 1e-4f);
    EXPECT_NEAR(revMat.GetRight().y, origMat.GetRight().y, 1e-4f);
    EXPECT_NEAR(revMat.GetRight().z, origMat.GetRight().z, 1e-4f);

    EXPECT_NEAR(revMat.GetForward().x, origMat.GetForward().x, 1e-4f);
    EXPECT_NEAR(revMat.GetForward().y, origMat.GetForward().y, 1e-4f);
    EXPECT_NEAR(revMat.GetForward().z, origMat.GetForward().z, 1e-4f);

    EXPECT_NEAR(revMat.GetUp().x, origMat.GetUp().x, 1e-4f);
    EXPECT_NEAR(revMat.GetUp().y, origMat.GetUp().y, 1e-4f);
    EXPECT_NEAR(revMat.GetUp().z, origMat.GetUp().z, 1e-4f);
}

GAME_DIFF_TEST(CCompressedMatrixNotAligned, RoundTrip_MultipleOrientations) {
    // Test compress→decompress round-trip for various orientations.
    // Compare reversed round-trip vs original round-trip.
    struct TestCase { CVector right, forward, up, pos; };
    TestCase cases[] = {
        // Identity
        { {1, 0, 0}, {0, 1, 0}, {0, 0, 1}, {0, 0, 0} },
        // 90° yaw
        { {0, 1, 0}, {-1, 0, 0}, {0, 0, 1}, {10, 20, 30} },
        // 45° yaw
        { {0.707f, 0.707f, 0}, {-0.707f, 0.707f, 0}, {0, 0, 1}, {-100, 50, 0} },
        // Pitched up 45°
        { {1, 0, 0}, {0, 0.707f, 0.707f}, {0, -0.707f, 0.707f}, {0, 0, 100} },
        // Arbitrary rotation
        { {0.5f, 0.5f, 0.707f}, {-0.707f, 0.707f, 0.0f}, {-0.5f, -0.5f, 0.707f}, {999, -999, 0.5f} },
    };

    for (auto& tc : cases) {
        CMatrix source{};
        source.GetRight()    = tc.right;
        source.GetForward()  = tc.forward;
        source.GetUp()       = tc.up;
        source.GetPosition() = tc.pos;

        // Reversed round-trip
        CCompressedMatrixNotAligned revComp{};
        revComp.CompressFromFullMatrix(source);
        CMatrix revResult{};
        revComp.DecompressIntoFullMatrix(revResult);

        // Original round-trip
        CCompressedMatrixNotAligned origComp{};
        { HookDisableGuard guard("Global/CCompressedMatrixNotAligned/CompressFromFullMatrix");
          origComp.CompressFromFullMatrix(source); }
        CMatrix origResult{};
        { HookDisableGuard guard("Global/CCompressedMatrixNotAligned/DecompressIntoFullMatrix");
          origComp.DecompressIntoFullMatrix(origResult); }

        // Position should be exact (float, no quantization)
        EXPECT_NEAR(revResult.GetPosition().x, origResult.GetPosition().x, 1e-5f);
        EXPECT_NEAR(revResult.GetPosition().y, origResult.GetPosition().y, 1e-5f);
        EXPECT_NEAR(revResult.GetPosition().z, origResult.GetPosition().z, 1e-5f);

        // Orientation vectors — both go through int8 quantization + reorthogonalization
        EXPECT_NEAR(revResult.GetRight().x, origResult.GetRight().x, 1e-4f);
        EXPECT_NEAR(revResult.GetRight().y, origResult.GetRight().y, 1e-4f);
        EXPECT_NEAR(revResult.GetRight().z, origResult.GetRight().z, 1e-4f);
        EXPECT_NEAR(revResult.GetForward().x, origResult.GetForward().x, 1e-4f);
        EXPECT_NEAR(revResult.GetForward().y, origResult.GetForward().y, 1e-4f);
        EXPECT_NEAR(revResult.GetForward().z, origResult.GetForward().z, 1e-4f);
    }
}

// --- Behavior tests ---

GAME_TEST(CCompressedMatrixNotAligned, PositionPreserved) {
    // Position should survive round-trip exactly (stored as float CVector)
    CMatrix source{};
    source.GetRight()    = CVector(1, 0, 0);
    source.GetForward()  = CVector(0, 1, 0);
    source.GetUp()       = CVector(0, 0, 1);
    source.GetPosition() = CVector(123.456f, -789.012f, 0.001f);

    auto comp = CCompressedMatrixNotAligned::Compress(source);
    auto result = CCompressedMatrixNotAligned::Decompress(comp);

    EXPECT_NEAR(result.GetPosition().x, 123.456f, 1e-3f);
    EXPECT_NEAR(result.GetPosition().y, -789.012f, 1e-3f);
    EXPECT_NEAR(result.GetPosition().z, 0.001f, 1e-3f);
}

GAME_TEST(CCompressedMatrixNotAligned, OrthogonalAfterDecompress) {
    // After decompress + reorthogonalization, right·forward ≈ 0, etc.
    CMatrix source{};
    source.GetRight()    = CVector(0.6f, 0.8f, 0.0f);
    source.GetForward()  = CVector(-0.8f, 0.6f, 0.0f);
    source.GetUp()       = CVector(0.0f, 0.0f, 1.0f);
    source.GetPosition() = CVector(0, 0, 0);

    auto comp = CCompressedMatrixNotAligned::Compress(source);
    auto result = CCompressedMatrixNotAligned::Decompress(comp);

    float dotRF = result.GetRight().x * result.GetForward().x +
                  result.GetRight().y * result.GetForward().y +
                  result.GetRight().z * result.GetForward().z;
    float dotRU = result.GetRight().x * result.GetUp().x +
                  result.GetRight().y * result.GetUp().y +
                  result.GetRight().z * result.GetUp().z;
    // Should be approximately orthogonal
    EXPECT_NEAR(dotRF, 0.0f, 0.05f);
    EXPECT_NEAR(dotRU, 0.0f, 0.05f);
}

GAME_TEST(CCompressedMatrixNotAligned, SizeIs20Bytes) {
    EXPECT_EQ(sizeof(CCompressedMatrixNotAligned), (size_t)20);
}
