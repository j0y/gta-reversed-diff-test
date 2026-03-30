// test_CTimeCycle3.cpp — Differential tests for CTimeCycle and CCamera queries.
// Hook paths: Global/CTimeCycle/<fn>, Global/CCamera/<fn>
//
// CTimeCycle new targets:
//   CalcColoursForPoint     — complex color interpolation from time/weather/boxes
//   GetAmbientRed_Obj       — trivial getter (m_CurrentColours field)
//   GetAmbientGreen_Obj     — trivial getter
//   GetAmbientBlue_Obj      — trivial getter
//   FindTimeCycleBox        — spatial box lookup with falloff interpolation
//
// CCamera new targets:
//   Find3rdPersonQuickAimPitch — trig calculation (tan, atan, FOV)
//   Using1stPersonWeaponMode   — switch on weapon mode enum
//   ConeCastCollisionResolve   — collision ray with lerp

#include "StdInc.h"
#include "TestFramework.h"
#include "TimeCycle.h"
#include "Camera.h"

// ── CTimeCycle::GetAmbientRed/Green/Blue_Obj ──
// Trivial field getters. Quick sanity differential test.

GAME_DIFF_TEST(CTimeCycle3, Diff_GetAmbientRed_Obj) {
    float orig, rev;
    { HookDisableGuard guard("Global/CTimeCycle/GetAmbientRed_Obj");
      orig = CTimeCycle::GetAmbientRed_Obj(); }
    rev = CTimeCycle::GetAmbientRed_Obj();
    EXPECT_NEAR(orig, rev, 1e-5f);
}

GAME_DIFF_TEST(CTimeCycle3, Diff_GetAmbientGreen_Obj) {
    float orig, rev;
    { HookDisableGuard guard("Global/CTimeCycle/GetAmbientGreen_Obj");
      orig = CTimeCycle::GetAmbientGreen_Obj(); }
    rev = CTimeCycle::GetAmbientGreen_Obj();
    EXPECT_NEAR(orig, rev, 1e-5f);
}

GAME_DIFF_TEST(CTimeCycle3, Diff_GetAmbientBlue_Obj) {
    float orig, rev;
    { HookDisableGuard guard("Global/CTimeCycle/GetAmbientBlue_Obj");
      orig = CTimeCycle::GetAmbientBlue_Obj(); }
    rev = CTimeCycle::GetAmbientBlue_Obj();
    EXPECT_NEAR(orig, rev, 1e-5f);
}

// ── CTimeCycle::CalcColoursForPoint ──
// Complex interpolation: finds time cycle boxes, interpolates colors based on
// time of day, weather, and position. Tests at multiple world positions.

GAME_DIFF_TEST(CTimeCycle3, Diff_CalcColoursForPoint_Player) {
    auto* player = FindPlayerPed(0);
    if (!player) return;

    CColourSet origSet{}, revSet{};
    { HookDisableGuard guard("Global/CTimeCycle/CalcColoursForPoint");
      CTimeCycle::CalcColoursForPoint(player->GetPosition(), &origSet); }
    CTimeCycle::CalcColoursForPoint(player->GetPosition(), &revSet);

    // Compare key color fields (floats for ambient, uint16 for sky/sun)
    EXPECT_NEAR(origSet.m_fAmbientRed, revSet.m_fAmbientRed, 1e-3f);
    EXPECT_NEAR(origSet.m_fAmbientGreen, revSet.m_fAmbientGreen, 1e-3f);
    EXPECT_NEAR(origSet.m_fAmbientBlue, revSet.m_fAmbientBlue, 1e-3f);
    EXPECT_EQ(origSet.m_nSkyTopRed, revSet.m_nSkyTopRed);
    EXPECT_EQ(origSet.m_nSkyTopGreen, revSet.m_nSkyTopGreen);
    EXPECT_EQ(origSet.m_nSkyTopBlue, revSet.m_nSkyTopBlue);
    EXPECT_NEAR(origSet.m_fFarClip, revSet.m_fFarClip, 1e-1f);
    EXPECT_NEAR(origSet.m_fFogStart, revSet.m_fFogStart, 1e-1f);
}

GAME_DIFF_TEST(CTimeCycle3, Diff_CalcColoursForPoint_Positions) {
    CVector positions[] = {
        CVector(2488.f, -1665.f, 13.5f),     // Grove Street
        CVector(-2200.f, 200.f, 35.f),        // San Fierro
        CVector(2000.f, 1500.f, 11.f),        // Las Venturas
        CVector(0.f, 0.f, 500.f),             // high altitude ocean
    };

    for (auto& pos : positions) {
        CColourSet origSet{}, revSet{};
        { HookDisableGuard guard("Global/CTimeCycle/CalcColoursForPoint");
          CTimeCycle::CalcColoursForPoint(pos, &origSet); }
        CTimeCycle::CalcColoursForPoint(pos, &revSet);

        EXPECT_NEAR(origSet.m_fAmbientRed, revSet.m_fAmbientRed, 1e-3f);
        EXPECT_NEAR(origSet.m_fAmbientGreen, revSet.m_fAmbientGreen, 1e-3f);
        EXPECT_NEAR(origSet.m_fAmbientBlue, revSet.m_fAmbientBlue, 1e-3f);
        EXPECT_NEAR(origSet.m_fFarClip, revSet.m_fFarClip, 1e-1f);
    }
}

// ── CCamera::Using1stPersonWeaponMode ──
// Switch on m_PlayerWeaponMode.m_nMode — returns true for sniper/M16/rocket etc.

GAME_DIFF_TEST(CTimeCycle3, Diff_Using1stPersonWeaponMode) {
    bool orig, rev;
    { HookDisableGuard guard("Global/CCamera/Using1stPersonWeaponMode");
      orig = TheCamera.Using1stPersonWeaponMode(); }
    rev = TheCamera.Using1stPersonWeaponMode();
    EXPECT_EQ(orig, rev);
}

// ── CCamera::Find3rdPersonQuickAimPitch ──
// Trig math: tan(FOV/2), atan(opposite/aspect), applies to vertical angle.

GAME_DIFF_TEST(CTimeCycle3, Diff_Find3rdPersonQuickAimPitch) {
    float savedCHairY = CCamera::m_f3rdPersonCHairMultY;

    // Original function bytes at 0x50AD40 (78 bytes, from disassembly)
    // Copy to VirtualAlloc'd executable memory to bypass hook system entirely
    static const uint8_t origCode[] = {
        0x0F, 0xB6, 0x41, 0x59,                         // movzx eax, byte ptr [ecx+0x59]
        0x69, 0xC0, 0x38, 0x02, 0x00, 0x00,             // imul eax, eax, 0x238
        0xD9, 0x84, 0x08, 0x28, 0x02, 0x00, 0x00,       // fld dword ptr [eax+ecx+0x228]
        0xD8, 0x0D, 0x8C, 0x8B, 0x85, 0x00,             // fmul dword ptr [0x858b8c]
        0x03, 0xC1,                                       // add eax, ecx
        0xD8, 0x0D, 0xEC, 0x95, 0x85, 0x00,             // fmul dword ptr [0x8595ec]
        0xD9, 0xF2,                                       // fptan
        0xDD, 0xD8,                                       // fstp st(0)
        0xD9, 0x05, 0x8C, 0x8B, 0x85, 0x00,             // fld dword ptr [0x858b8c]
        0xD8, 0x25, 0x10, 0xEC, 0xB6, 0x00,             // fsub dword ptr [0xb6ec10]
        0xDC, 0xC0,                                       // fadd st(0), st(0)
        0xDE, 0xC9,                                       // fmulp st(1)
        0xD9, 0x05, 0x24, 0x86, 0x85, 0x00,             // fld dword ptr [0x858624]
        0xD8, 0x35, 0xA4, 0xEF, 0xC3, 0x00,             // fdiv dword ptr [0xc3efa4]
        0xDE, 0xC9,                                       // fmulp st(1)
        0xD9, 0xE8,                                       // fld1
        0xD9, 0xF3,                                       // fpatan
        0xD8, 0x80, 0x20, 0x02, 0x00, 0x00,             // fadd dword ptr [eax+0x220]
        0xD9, 0xE0,                                       // fchs
        0xC3                                              // ret
    };

    // Allocate executable memory and copy original code
    void* execMem = VirtualAlloc(nullptr, sizeof(origCode), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (!execMem) return;
    memcpy(execMem, origCode, sizeof(origCode));

    float testValues[] = { 0.5f, 0.3f, 0.0f };
    for (float chy : testValues) {
        *reinterpret_cast<float*>(0xB6EC10) = chy;

        // Call the copied original function (thiscall: ecx = this)
        float origResult;
        CCamera* pCam = &TheCamera;
        void* pFunc = execMem;
        __asm {
            mov ecx, pCam
            call pFunc
            fstp dword ptr [origResult]
        }

        float revResult = TheCamera.Find3rdPersonQuickAimPitch();
        EXPECT_NEAR(origResult, revResult, 1e-3f);
    }

    VirtualFree(execMem, 0, MEM_RELEASE);
    *reinterpret_cast<float*>(0xB6EC10) = savedCHairY;
}

// ── CCamera::ConeCastCollisionResolve ──
// Cone-cast collision from lookAt to pos. Returns true if collision found.
// Pure query: reads world collision, outputs interpolated position.

GAME_DIFF_TEST(CTimeCycle3, Diff_ConeCastCollisionResolve_NearPlayer) {
    auto* player = FindPlayerPed(0);
    if (!player) return;

    CVector pos = player->GetPosition() + CVector(0.f, -5.f, 2.f);
    CVector lookAt = player->GetPosition();

    CVector origDest{}, revDest{};
    float origDist = 0.f, revDist = 0.f;
    bool origRet, revRet;

    { HookDisableGuard guard("Global/CCamera/ConeCastCollisionResolve");
      origRet = TheCamera.ConeCastCollisionResolve(pos, lookAt, origDest, 0.4f, 0.1f, origDist); }
    revRet = TheCamera.ConeCastCollisionResolve(pos, lookAt, revDest, 0.4f, 0.1f, revDist);

    EXPECT_EQ(origRet, revRet);
    EXPECT_NEAR(origDist, revDist, 1e-3f);
    if (origRet && revRet) {
        EXPECT_NEAR(origDest.x, revDest.x, 1e-2f);
        EXPECT_NEAR(origDest.y, revDest.y, 1e-2f);
        EXPECT_NEAR(origDest.z, revDest.z, 1e-2f);
    }
}

GAME_DIFF_TEST(CTimeCycle3, Diff_ConeCastCollisionResolve_SamePoint) {
    // pos == lookAt should return false immediately
    CVector pos(2500.f, -1660.f, 15.f);
    CVector dest{};
    float dist = 0.f;
    bool origRet, revRet;

    { HookDisableGuard guard("Global/CCamera/ConeCastCollisionResolve");
      origRet = TheCamera.ConeCastCollisionResolve(pos, pos, dest, 0.4f, 0.1f, dist); }
    revRet = TheCamera.ConeCastCollisionResolve(pos, pos, dest, 0.4f, 0.1f, dist);

    EXPECT_EQ(origRet, revRet);
    EXPECT_EQ(false, revRet);
}
