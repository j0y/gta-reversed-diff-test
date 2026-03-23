// test_CRadar_DirectCall2.cpp — More direct-call differential tests for CRadar.
// Uses asm sret workaround for CVector2D-returning functions.

#include "StdInc.h"
#include "TestFramework.h"
#include "Radar.h"

static CVector2D CallOrigV2D(uint32 addr, const CVector2D& in) {
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

// --- TransformRadarPointToRealWorldSpace (0x5835A0) ---

GAME_TEST(CRadar_DirectCall2, TransformRadarPointToRealWorldSpace) {
    CVector2D inputs[] = {
        {0.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 1.0f}, {-1.0f, -1.0f},
        {0.5f, 0.5f}, {0.25f, -0.75f},
    };
    for (auto& in : inputs) {
        CVector2D orig = CallOrigV2D(0x5835A0, in);
        CVector2D rev = CRadar::TransformRadarPointToRealWorldSpace(in);
        EXPECT_NEAR(orig.x, rev.x, 1e-1f); // world coords can be large
        EXPECT_NEAR(orig.y, rev.y, 1e-1f);
    }
}

// --- TransformRadarPointToScreenSpace extended inputs ---

GAME_TEST(CRadar_DirectCall2, TransformRadarToScreen_Extended) {
    CVector2D inputs[] = {
        {0.0f, 0.0f}, {1.0f, 1.0f}, {-1.0f, -1.0f},
        {0.1f, 0.1f}, {0.9f, 0.9f}, {-0.9f, 0.9f},
        {0.0f, -1.0f}, {-0.5f, 0.5f},
    };
    for (auto& in : inputs) {
        CVector2D orig = CallOrigV2D(0x583480, in);
        CVector2D rev = CRadar::TransformRadarPointToScreenSpace(in);
        EXPECT_NEAR(orig.x, rev.x, 1e-3f);
        EXPECT_NEAR(orig.y, rev.y, 1e-3f);
    }
}

// --- TransformRealWorldPointToRadarSpace extended ---

GAME_TEST(CRadar_DirectCall2, TransformRealWorldToRadar_Extended) {
    CVector2D inputs[] = {
        {2495.0f, -1665.0f},  // Grove Street
        {-2170.0f, 250.0f},   // SF
        {2027.0f, 1008.0f},   // LV
        {0.0f, 0.0f},         // origin
        {3000.0f, 3000.0f},   // map edge
        {-3000.0f, -3000.0f}, // opposite edge
    };
    for (auto& in : inputs) {
        CVector2D orig = CallOrigV2D(0x583530, in);
        CVector2D rev = CRadar::TransformRealWorldPointToRadarSpace(in);
        EXPECT_NEAR(orig.x, rev.x, 1e-3f);
        EXPECT_NEAR(orig.y, rev.y, 1e-3f);
    }
}
