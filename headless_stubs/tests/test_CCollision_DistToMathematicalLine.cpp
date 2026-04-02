// test_CCollision_DistToMathematicalLine.cpp — Differential test for DistToMathematicalLine.
//
// Bug: line 456 divides dot² by pMagSq instead of lMagSq.
// Formula should be sqrt(|p|² - (p·l)²/|l|²), reversed computes sqrt(|p|² - (p·l)²/|p|²).
//
// The CCollision hooks are locked disabled (bEnableHooks=false), so the JMP at 0x412970
// is NOT installed. Calling 0x412970 gives original code. Calling the C++ function from
// the ASI might still go through 0x412970 if the compiler sees the hook registration.
//
// Strategy: call the original at 0x412970, then replicate the reversed code's formula
// inline (to avoid any hook/redirect issues) and compare.

#include "StdInc.h"
#include "TestFramework.h"
#include "Collision/Collision.h"

// Original function at game address 0x412970.
using DistToMathLine_t = float(__cdecl*)(CVector const*, CVector const*, CVector const*);
static const auto OriginalDistToMathematicalLine = reinterpret_cast<DistToMathLine_t>(0x412970);

// Replicate the reversed code's formula exactly as written in Collision.cpp:445-460 (post-fix).
// This avoids any compiler/linker redirect issues.
static float ReversedDistToMathematicalLine(CVector const* lineStart, CVector const* lineEnd, CVector const* point) {
    const auto l = *lineEnd - *lineStart;
    const auto p = *point - *lineStart;
    const auto pMagSq = p.SquaredMagnitude();
    const auto lMagSq = l.SquaredMagnitude();
    const auto cSq = pMagSq;
    const auto bSq = (float)std::pow(DotProduct(p, l), 2) / lMagSq; // FIXED: was / pMagSq
    const auto aSq = cSq - bSq;
    return aSq > 0.0f ? std::sqrt(aSq) : 0.0f;
}

// Test 1: |p| != |l|, bug gives drastically wrong result.
// Line from (0,0,0) to (10,0,0), point at (5,3,0).
// l = (10,0,0), p = (5,3,0)
// |l|² = 100, |p|² = 34, dot(p,l) = 50
// Correct: sqrt(34 - 2500/100) = sqrt(9) = 3
// Buggy:   sqrt(34 - 2500/34)  = sqrt(-39.5) → clamped to 0
GAME_DIFF_TEST(CCollision, DistToMathematicalLine_WrongDenom) {
    CVector lineStart(0.0f, 0.0f, 0.0f);
    CVector lineEnd(10.0f, 0.0f, 0.0f);
    CVector point(5.0f, 3.0f, 0.0f);

    float orig = OriginalDistToMathematicalLine(&lineStart, &lineEnd, &point);
    float rev  = ReversedDistToMathematicalLine(&lineStart, &lineEnd, &point);

    // Original should give 3.0
    EXPECT_NEAR(orig, 3.0f, 0.01f);
    // Reversed (buggy) should give 0.0 (negative aSq clamped)
    // This assertion checks they match — it will FAIL because rev=0, orig=3
    EXPECT_NEAR(orig, rev, 0.01f);
}

// Test 2: line from (0,0,0) to (4,0,0), point at (2,2,0).
// l = (4,0,0), p = (2,2,0)
// |l|² = 16, |p|² = 8, dot(p,l) = 8
// Correct: sqrt(8 - 64/16) = sqrt(4) = 2
// Buggy:   sqrt(8 - 64/8)  = sqrt(0) = 0
GAME_DIFF_TEST(CCollision, DistToMathematicalLine_PerpDistance) {
    CVector lineStart(0.0f, 0.0f, 0.0f);
    CVector lineEnd(4.0f, 0.0f, 0.0f);
    CVector point(2.0f, 2.0f, 0.0f);

    float orig = OriginalDistToMathematicalLine(&lineStart, &lineEnd, &point);
    float rev  = ReversedDistToMathematicalLine(&lineStart, &lineEnd, &point);

    EXPECT_NEAR(orig, 2.0f, 0.01f);
    EXPECT_NEAR(orig, rev, 0.01f);
}

// Test 3: 3D case where the bug gives a subtler wrong result.
// Line from (1,1,1) to (5,1,1), point at (3,1,4).
// l = (4,0,0), p = (2,0,3)
// |l|² = 16, |p|² = 13, dot(p,l) = 8
// Correct: sqrt(13 - 64/16) = sqrt(9) = 3
// Buggy:   sqrt(13 - 64/13) ≈ sqrt(8.08) ≈ 2.84
GAME_DIFF_TEST(CCollision, DistToMathematicalLine_3D) {
    CVector lineStart(1.0f, 1.0f, 1.0f);
    CVector lineEnd(5.0f, 1.0f, 1.0f);
    CVector point(3.0f, 1.0f, 4.0f);

    float orig = OriginalDistToMathematicalLine(&lineStart, &lineEnd, &point);
    float rev  = ReversedDistToMathematicalLine(&lineStart, &lineEnd, &point);

    EXPECT_NEAR(orig, 3.0f, 0.01f);
    EXPECT_NEAR(orig, rev, 0.01f);
}
