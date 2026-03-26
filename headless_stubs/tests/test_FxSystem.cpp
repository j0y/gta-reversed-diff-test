// test_FxSystem.cpp — Differential tests for FxSystem_c.
// Hook paths: Fx/FxSystem_c/<fn>
//
// FxSystem_c manages particle effect instances. We create live systems
// via g_fxMan.CreateFxSystem() and test queries on them.
// Systems are cleaned up via Kill() to avoid leaks.

#include "StdInc.h"
#include "TestFramework.h"
#include "Fx/FxSystem.h"
#include "Fx/FxManager.h"

// RAII wrapper for FxSystem lifecycle
struct TestFxSystem {
    FxSystem_c* sys = nullptr;

    TestFxSystem(const char* name, CVector pos = {0.f, 0.f, 10.f}) {
        sys = g_fxMan.CreateFxSystem(name, pos, nullptr, true);
    }
    ~TestFxSystem() {
        if (sys) sys->Kill();
    }
    operator bool() const { return sys != nullptr; }
    FxSystem_c* operator->() const { return sys; }
    FxSystem_c* get() const { return sys; }
};

// Known FX names that should exist in the default game data.
// Picked from common gameplay effects.
static const char* FX_NAMES[] = {
    "blood_heli", "gunflash", "exhale", "cigsmoke",
    "water_splash", "prt_smoke_huge", "prt_blood",
};
static constexpr int NUM_FX = sizeof(FX_NAMES) / sizeof(FX_NAMES[0]);

// Find the first FX name that can be created
static const char* FindWorkingFxName() {
    for (auto* name : FX_NAMES) {
        if (g_fxMan.FindFxSystemBP(name)) return name;
    }
    return nullptr;
}

// ======================================================================
// GetPlayStatus — newly created system should be stopped
// ======================================================================

GAME_DIFF_TEST(FxSystem, GetPlayStatus_NewSystem) {
    auto* fxName = FindWorkingFxName();
    if (!fxName) return;

    TestFxSystem fx(fxName);
    if (!fx) return;

    eFxSystemPlayStatus orig, rev;
    { HookDisableGuard guard("Fx/FxSystem_c/GetPlayStatus");
      orig = fx->GetPlayStatus(); }
    rev = fx->GetPlayStatus();
    EXPECT_EQ((int)orig, (int)rev);
}

// ======================================================================
// Play + GetPlayStatus — after Play(), status should be PLAYING
// ======================================================================

GAME_DIFF_TEST(FxSystem, GetPlayStatus_AfterPlay) {
    auto* fxName = FindWorkingFxName();
    if (!fxName) return;

    // Test with original Play
    {
        TestFxSystem fx(fxName);
        if (!fx) return;
        { HookDisableGuard guard("Fx/FxSystem_c/Play");
          fx->Play(); }
        eFxSystemPlayStatus orig;
        { HookDisableGuard guard("Fx/FxSystem_c/GetPlayStatus");
          orig = fx->GetPlayStatus(); }

        // Create fresh system and test with reversed Play
        TestFxSystem fx2(fxName);
        if (!fx2) return;
        fx2->Play();
        eFxSystemPlayStatus rev = fx2->GetPlayStatus();

        EXPECT_EQ((int)orig, (int)rev);
    }
}

// ======================================================================
// Stop + GetPlayStatus
// ======================================================================

GAME_DIFF_TEST(FxSystem, GetPlayStatus_AfterStop) {
    auto* fxName = FindWorkingFxName();
    if (!fxName) return;

    {
        TestFxSystem fx(fxName);
        if (!fx) return;
        fx->Play();
        { HookDisableGuard guard("Fx/FxSystem_c/Stop");
          fx->Stop(); }
        eFxSystemPlayStatus orig;
        { HookDisableGuard guard("Fx/FxSystem_c/GetPlayStatus");
          orig = fx->GetPlayStatus(); }

        TestFxSystem fx2(fxName);
        if (!fx2) return;
        fx2->Play();
        fx2->Stop();
        eFxSystemPlayStatus rev = fx2->GetPlayStatus();

        EXPECT_EQ((int)orig, (int)rev);
    }
}

// ======================================================================
// Pause + GetPlayStatus
// ======================================================================

GAME_DIFF_TEST(FxSystem, GetPlayStatus_AfterPause) {
    auto* fxName = FindWorkingFxName();
    if (!fxName) return;

    {
        TestFxSystem fx(fxName);
        if (!fx) return;
        fx->Play();
        { HookDisableGuard guard("Fx/FxSystem_c/Pause");
          fx->Pause(); }
        eFxSystemPlayStatus orig;
        { HookDisableGuard guard("Fx/FxSystem_c/GetPlayStatus");
          orig = fx->GetPlayStatus(); }

        TestFxSystem fx2(fxName);
        if (!fx2) return;
        fx2->Play();
        fx2->Pause();
        eFxSystemPlayStatus rev = fx2->GetPlayStatus();

        EXPECT_EQ((int)orig, (int)rev);
    }
}

// ======================================================================
// IsVisible
// ======================================================================

GAME_DIFF_TEST(FxSystem, IsVisible) {
    auto* fxName = FindWorkingFxName();
    if (!fxName) return;

    TestFxSystem fx(fxName);
    if (!fx) return;

    bool orig, rev;
    { HookDisableGuard guard("Fx/FxSystem_c/IsVisible");
      orig = fx->IsVisible(); }
    rev = fx->IsVisible();
    EXPECT_EQ(orig, rev);
}

// ======================================================================
// GetBoundingSphereLcl
// ======================================================================

GAME_DIFF_TEST(FxSystem, GetBoundingSphereLcl) {
    auto* fxName = FindWorkingFxName();
    if (!fxName) return;

    TestFxSystem fx(fxName);
    if (!fx) return;

    FxSphere_c origSphere(CVector(0,0,0), 0.f);
    FxSphere_c revSphere(CVector(0,0,0), 0.f);

    bool origOk, revOk;
    { HookDisableGuard guard("Fx/FxSystem_c/GetBoundingSphereLcl");
      origOk = fx->GetBoundingSphereLcl(&origSphere); }
    revOk = fx->GetBoundingSphereLcl(&revSphere);

    EXPECT_EQ(origOk, revOk);
    if (origOk && revOk) {
        EXPECT_NEAR(origSphere.m_vecCenter.x, revSphere.m_vecCenter.x, 1e-5f);
        EXPECT_NEAR(origSphere.m_vecCenter.y, revSphere.m_vecCenter.y, 1e-5f);
        EXPECT_NEAR(origSphere.m_vecCenter.z, revSphere.m_vecCenter.z, 1e-5f);
        EXPECT_NEAR(origSphere.m_fRadius, revSphere.m_fRadius, 1e-5f);
    }
}

// ======================================================================
// GetBoundingSphereWld
// ======================================================================

GAME_DIFF_TEST(FxSystem, GetBoundingSphereWld) {
    auto* fxName = FindWorkingFxName();
    if (!fxName) return;

    TestFxSystem fx(fxName);
    if (!fx) return;

    FxSphere_c origSphere(CVector(0,0,0), 0.f);
    FxSphere_c revSphere(CVector(0,0,0), 0.f);

    bool origOk, revOk;
    { HookDisableGuard guard("Fx/FxSystem_c/GetBoundingSphereWld");
      origOk = fx->GetBoundingSphereWld(&origSphere); }
    revOk = fx->GetBoundingSphereWld(&revSphere);

    EXPECT_EQ(origOk, revOk);
    if (origOk && revOk) {
        EXPECT_NEAR(origSphere.m_vecCenter.x, revSphere.m_vecCenter.x, 1e-5f);
        EXPECT_NEAR(origSphere.m_vecCenter.y, revSphere.m_vecCenter.y, 1e-5f);
        EXPECT_NEAR(origSphere.m_vecCenter.z, revSphere.m_vecCenter.z, 1e-5f);
        EXPECT_NEAR(origSphere.m_fRadius, revSphere.m_fRadius, 1e-5f);
    }
}

// ======================================================================
// GetCompositeMatrix
// ======================================================================

GAME_DIFF_TEST(FxSystem, GetCompositeMatrix) {
    auto* fxName = FindWorkingFxName();
    if (!fxName) return;

    TestFxSystem fx(fxName);
    if (!fx) return;

    RwMatrix origMat{}, revMat{};
    { HookDisableGuard guard("Fx/FxSystem_c/GetCompositeMatrix");
      fx->GetCompositeMatrix(&origMat); }
    fx->GetCompositeMatrix(&revMat);

    // Compare right/up/at/pos vectors
    EXPECT_NEAR(origMat.right.x, revMat.right.x, 1e-5f);
    EXPECT_NEAR(origMat.right.y, revMat.right.y, 1e-5f);
    EXPECT_NEAR(origMat.right.z, revMat.right.z, 1e-5f);
    EXPECT_NEAR(origMat.up.x, revMat.up.x, 1e-5f);
    EXPECT_NEAR(origMat.up.y, revMat.up.y, 1e-5f);
    EXPECT_NEAR(origMat.up.z, revMat.up.z, 1e-5f);
    EXPECT_NEAR(origMat.at.x, revMat.at.x, 1e-5f);
    EXPECT_NEAR(origMat.at.y, revMat.at.y, 1e-5f);
    EXPECT_NEAR(origMat.at.z, revMat.at.z, 1e-5f);
    EXPECT_NEAR(origMat.pos.x, revMat.pos.x, 1e-5f);
    EXPECT_NEAR(origMat.pos.y, revMat.pos.y, 1e-5f);
    EXPECT_NEAR(origMat.pos.z, revMat.pos.z, 1e-5f);
}

// ======================================================================
// SetOffsetPos + GetCompositeMatrix — verify offset is applied
// ======================================================================

GAME_DIFF_TEST(FxSystem, SetOffsetPos_AffectsMatrix) {
    auto* fxName = FindWorkingFxName();
    if (!fxName) return;

    CVector offset(5.f, 10.f, 15.f);

    // Original SetOffsetPos
    TestFxSystem fx1(fxName);
    if (!fx1) return;
    { HookDisableGuard guard("Fx/FxSystem_c/SetOffsetPos");
      fx1->SetOffsetPos(offset); }
    RwMatrix origMat{};
    fx1->GetCompositeMatrix(&origMat);

    // Reversed SetOffsetPos
    TestFxSystem fx2(fxName);
    if (!fx2) return;
    fx2->SetOffsetPos(offset);
    RwMatrix revMat{};
    fx2->GetCompositeMatrix(&revMat);

    EXPECT_NEAR(origMat.pos.x, revMat.pos.x, 1e-3f);
    EXPECT_NEAR(origMat.pos.y, revMat.pos.y, 1e-3f);
    EXPECT_NEAR(origMat.pos.z, revMat.pos.z, 1e-3f);
}

// ======================================================================
// SetRateMult — compare via raw memory (members are packed uint16s)
// ======================================================================

GAME_DIFF_TEST(FxSystem, SetRateMult) {
    auto* fxName = FindWorkingFxName();
    if (!fxName) return;

    float mults[] = { 0.0f, 0.5f, 1.0f, 2.0f, 10.0f };
    for (float m : mults) {
        TestFxSystem fx1(fxName);
        if (!fx1) return;
        { HookDisableGuard guard("Fx/FxSystem_c/SetRateMult");
          fx1->SetRateMult(m); }
        uint16 origRate = fx1.sys->m_nRateMult;

        TestFxSystem fx2(fxName);
        if (!fx2) return;
        fx2->SetRateMult(m);
        uint16 revRate = fx2.sys->m_nRateMult;

        EXPECT_EQ(origRate, revRate);
    }
}

GAME_DIFF_TEST(FxSystem, SetTimeMult) {
    auto* fxName = FindWorkingFxName();
    if (!fxName) return;

    float mults[] = { 0.0f, 0.5f, 1.0f, 2.0f };
    for (float m : mults) {
        TestFxSystem fx1(fxName);
        if (!fx1) return;
        { HookDisableGuard guard("Fx/FxSystem_c/SetTimeMult");
          fx1->SetTimeMult(m); }
        uint16 origTime = fx1.sys->m_nTimeMult;

        TestFxSystem fx2(fxName);
        if (!fx2) return;
        fx2->SetTimeMult(m);
        uint16 revTime = fx2.sys->m_nTimeMult;

        EXPECT_EQ(origTime, revTime);
    }
}

// ======================================================================
// SetLocalParticles / SetMustCreatePrts — bitfield setters
// ======================================================================

GAME_DIFF_TEST(FxSystem, SetLocalParticles) {
    auto* fxName = FindWorkingFxName();
    if (!fxName) return;

    for (bool val : {true, false}) {
        TestFxSystem fx1(fxName);
        if (!fx1) return;
        { HookDisableGuard guard("Fx/FxSystem_c/SetLocalParticles");
          fx1->SetLocalParticles(val); }
        bool origVal = fx1.sys->m_createLocal;

        TestFxSystem fx2(fxName);
        if (!fx2) return;
        fx2->SetLocalParticles(val);
        bool revVal = fx2.sys->m_createLocal;

        EXPECT_EQ(origVal, revVal);
    }
}

GAME_DIFF_TEST(FxSystem, SetMustCreatePrts) {
    auto* fxName = FindWorkingFxName();
    if (!fxName) return;

    for (bool val : {true, false}) {
        TestFxSystem fx1(fxName);
        if (!fx1) return;
        { HookDisableGuard guard("Fx/FxSystem_c/SetMustCreatePrts");
          fx1->SetMustCreatePrts(val); }
        bool origVal = fx1.sys->m_MustCreateParticles;

        TestFxSystem fx2(fxName);
        if (!fx2) return;
        fx2->SetMustCreatePrts(val);
        bool revVal = fx2.sys->m_MustCreateParticles;

        EXPECT_EQ(origVal, revVal);
    }
}

// ======================================================================
// SetConstTime
// ======================================================================

GAME_DIFF_TEST(FxSystem, SetConstTime) {
    auto* fxName = FindWorkingFxName();
    if (!fxName) return;

    struct Case { bool on; float time; };
    Case cases[] = { {false, 0.f}, {true, 0.5f}, {true, 1.0f}, {true, 0.0f} };

    for (auto& c : cases) {
        TestFxSystem fx1(fxName);
        if (!fx1) return;
        { HookDisableGuard guard("Fx/FxSystem_c/SetConstTime");
          fx1->SetConstTime(c.on, c.time); }
        bool origOn = fx1.sys->m_UseConstTime;
        uint16 origTime = fx1.sys->m_nConstTime;

        TestFxSystem fx2(fxName);
        if (!fx2) return;
        fx2->SetConstTime(c.on, c.time);
        bool revOn = fx2.sys->m_UseConstTime;
        uint16 revTime = fx2.sys->m_nConstTime;

        EXPECT_EQ(origOn, revOn);
        EXPECT_EQ(origTime, revTime);
    }
}

// ======================================================================
// Multiple FX names — test queries across different effect types
// ======================================================================

GAME_DIFF_TEST(FxSystem, GetPlayStatus_AllFxNames) {
    for (auto* name : FX_NAMES) {
        if (!g_fxMan.FindFxSystemBP(name)) continue;

        TestFxSystem fx(name);
        if (!fx) continue;

        eFxSystemPlayStatus orig, rev;
        { HookDisableGuard guard("Fx/FxSystem_c/GetPlayStatus");
          orig = fx->GetPlayStatus(); }
        rev = fx->GetPlayStatus();
        EXPECT_EQ((int)orig, (int)rev);
    }
}

GAME_DIFF_TEST(FxSystem, IsVisible_AllFxNames) {
    for (auto* name : FX_NAMES) {
        if (!g_fxMan.FindFxSystemBP(name)) continue;

        TestFxSystem fx(name);
        if (!fx) continue;

        bool orig, rev;
        { HookDisableGuard guard("Fx/FxSystem_c/IsVisible");
          orig = fx->IsVisible(); }
        rev = fx->IsVisible();
        EXPECT_EQ(orig, rev);
    }
}

// ======================================================================
// Play → Pause → Play lifecycle
// ======================================================================

GAME_DIFF_TEST(FxSystem, PlayPausePlay_Lifecycle) {
    auto* fxName = FindWorkingFxName();
    if (!fxName) return;

    // Original lifecycle
    TestFxSystem fx1(fxName);
    if (!fx1) return;
    { HookDisableGuard guard("Fx/FxSystem_c/Play");
      fx1->Play(); }
    eFxSystemPlayStatus origAfterPlay;
    { HookDisableGuard guard("Fx/FxSystem_c/GetPlayStatus");
      origAfterPlay = fx1->GetPlayStatus(); }

    { HookDisableGuard guard("Fx/FxSystem_c/Pause");
      fx1->Pause(); }
    eFxSystemPlayStatus origAfterPause;
    { HookDisableGuard guard("Fx/FxSystem_c/GetPlayStatus");
      origAfterPause = fx1->GetPlayStatus(); }

    { HookDisableGuard guard("Fx/FxSystem_c/Play");
      fx1->Play(); }
    eFxSystemPlayStatus origAfterResume;
    { HookDisableGuard guard("Fx/FxSystem_c/GetPlayStatus");
      origAfterResume = fx1->GetPlayStatus(); }

    // Reversed lifecycle
    TestFxSystem fx2(fxName);
    if (!fx2) return;
    fx2->Play();
    eFxSystemPlayStatus revAfterPlay = fx2->GetPlayStatus();
    fx2->Pause();
    eFxSystemPlayStatus revAfterPause = fx2->GetPlayStatus();
    fx2->Play();
    eFxSystemPlayStatus revAfterResume = fx2->GetPlayStatus();

    EXPECT_EQ((int)origAfterPlay, (int)revAfterPlay);
    EXPECT_EQ((int)origAfterPause, (int)revAfterPause);
    EXPECT_EQ((int)origAfterResume, (int)revAfterResume);
}

// ======================================================================
// Behavior: sanity checks
// ======================================================================

GAME_TEST(FxSystem, CreateAndKill_NoLeak) {
    auto* fxName = FindWorkingFxName();
    if (!fxName) return;

    // Create and kill 10 systems — should not leak
    for (int i = 0; i < 10; i++) {
        auto* sys = g_fxMan.CreateFxSystem(fxName, CVector(0, 0, 10), nullptr, true);
        EXPECT_TRUE(sys != nullptr);
        if (sys) sys->Kill();
    }
}

GAME_TEST(FxSystem, NewSystem_IsStopped) {
    auto* fxName = FindWorkingFxName();
    if (!fxName) return;

    TestFxSystem fx(fxName);
    if (!fx) return;

    EXPECT_EQ((int)fx->GetPlayStatus(), (int)eFxSystemPlayStatus::FX_STOPPED);
}
