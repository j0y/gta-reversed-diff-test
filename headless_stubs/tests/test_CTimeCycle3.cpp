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
    float orig, rev;
    { HookDisableGuard guard("Global/CCamera/Find3rdPersonQuickAimPitch");
      orig = TheCamera.Find3rdPersonQuickAimPitch(); }
    rev = TheCamera.Find3rdPersonQuickAimPitch();
    EXPECT_NEAR(orig, rev, 1e-3f);
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
