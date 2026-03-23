// test_CRadar_DirectCall.cpp — Differential tests using direct asm calls
// to work around the sret calling convention issue.
//
// These call original functions directly at their game addresses with
// explicit sret pointer, then compare to reversed code output.

#include "StdInc.h"
#include "TestFramework.h"
#include "Radar.h"

// Helper: call original CVector2D-returning function at a game address
static CVector2D CallOriginalV2D(uint32 addr, const CVector2D& in) {
    CVector2D result = {-999.0f, -999.0f};
    const CVector2D* pIn = &in;
    CVector2D* pOut = &result;
    __asm {
        mov eax, pIn
        push eax
        mov eax, pOut
        push eax
        mov eax, addr
        call eax
        add esp, 8
    }
    return result;
}

// --- TransformRadarPointToScreenSpace (0x583480) ---
// Previously showed sret issue with HookDisableGuard — test via direct call

GAME_TEST(CRadar_DirectCall, TransformRadarPointToScreenSpace) {
    CVector2D inputs[] = {
        {0.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 1.0f}, {-1.0f, 0.0f},
        {0.5f, 0.5f}, {-0.5f, -0.5f}, {0.25f, 0.75f},
    };
    for (auto& in : inputs) {
        CVector2D orig = CallOriginalV2D(0x583480, in);
        CVector2D rev = CRadar::TransformRadarPointToScreenSpace(in);
        EXPECT_NEAR(orig.x, rev.x, 1e-3f);
        EXPECT_NEAR(orig.y, rev.y, 1e-3f);
    }
}

// --- TransformRealWorldPointToRadarSpace (0x583530) ---
// Verify the HookDisableGuard result matches direct call

GAME_TEST(CRadar_DirectCall, TransformRealWorldPointToRadarSpace) {
    CVector2D inputs[] = {
        {2495.0f, -1665.0f}, {0.0f, 0.0f}, {-2170.0f, 250.0f}, {2027.0f, 1008.0f},
    };
    for (auto& in : inputs) {
        CVector2D orig = CallOriginalV2D(0x583530, in);
        CVector2D rev = CRadar::TransformRealWorldPointToRadarSpace(in);
        EXPECT_NEAR(orig.x, rev.x, 1e-3f);
        EXPECT_NEAR(orig.y, rev.y, 1e-3f);
    }
}
