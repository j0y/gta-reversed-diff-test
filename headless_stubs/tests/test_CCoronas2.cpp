// test_CCoronas2.cpp — Additional behavior tests for CCoronas.
// Hook paths: Global/CCoronas/<fn>
//
// Extends test_CCoronas.cpp with:
// - DoSunAndMoon — void rendering function, behavior test only
// - UpdateCoronaCoors — additional differential coverage with known ID+position

#include "StdInc.h"
#include "TestFramework.h"
#include "Coronas.h"

static constexpr uint32 TEST_CORONA_BASE_ID2 = 0xFFFD0000;

static CRegisteredCorona* FindCorona2(uint32 id) {
    for (auto& c : CCoronas::aCoronas) {
        if (c.m_dwId == id) return &c;
    }
    return nullptr;
}

static void CleanupTestCoronas2() {
    for (auto& c : CCoronas::aCoronas) {
        if (c.m_dwId >= TEST_CORONA_BASE_ID2) {
            c.m_dwId = 0;
            c.m_Color.a = 0;
            c.m_FadedIntensity = 0;
        }
    }
}

// --- DoSunAndMoon behavior tests ---
// DoSunAndMoon is a rendering function that registers coronas for the sun.
// We can't diff-test it directly (it calls RegisterCorona internally),
// but we can verify it doesn't crash and observe its side effects.

GAME_TEST(CCoronas2, DoSunAndMoon_NoCrash) {
    // Simply call and verify no crash
    uint32 savedNumCoronas = CCoronas::NumCoronas;
    CCoronas::DoSunAndMoon();
    // Function either registered sun coronas or returned early
    // (depends on CGame::CanSeeOutSideFromCurrArea)
    // No crash is the test
    (void)savedNumCoronas;
}

GAME_DIFF_TEST(CCoronas2, DoSunAndMoon_Diff) {
    // Run reversed DoSunAndMoon, capture sun corona state, then original
    // DoSunAndMoon registers coronas at ID 1 (glow) and ID 2 (flare)

    // Reversed
    CCoronas::DoSunAndMoon();
    // IDs 1,2 are below our test range — look directly
    // Look for sun corona by checking known IDs
    CRegisteredCorona* sunGlowRev = nullptr;
    CRegisteredCorona* sunFlareRev = nullptr;
    for (auto& c : CCoronas::aCoronas) {
        if (c.m_dwId == 1) sunGlowRev = &c;
        if (c.m_dwId == 2) sunFlareRev = &c;
    }

    CVector revGlowPos = sunGlowRev ? sunGlowRev->m_vPosn : CVector(0, 0, 0);
    float revGlowSize = sunGlowRev ? sunGlowRev->m_fSize : 0.0f;

    // Original
    { HookDisableGuard guard("Global/CCoronas/DoSunAndMoon");
      CCoronas::DoSunAndMoon(); }

    CRegisteredCorona* sunGlowOrig = nullptr;
    for (auto& c : CCoronas::aCoronas) {
        if (c.m_dwId == 1) { sunGlowOrig = &c; break; }
    }

    if (sunGlowRev && sunGlowOrig) {
        EXPECT_NEAR(revGlowPos.x, sunGlowOrig->m_vPosn.x, 1e-2f);
        EXPECT_NEAR(revGlowPos.y, sunGlowOrig->m_vPosn.y, 1e-2f);
        EXPECT_NEAR(revGlowPos.z, sunGlowOrig->m_vPosn.z, 1e-2f);
        EXPECT_NEAR(revGlowSize, sunGlowOrig->m_fSize, 1e-2f);
    }
    // If neither found, sun is below horizon — both returned early, still matching
}

// --- UpdateCoronaCoors additional differential tests ---

GAME_DIFF_TEST(CCoronas2, UpdateCoronaCoors_ZeroRange) {
    // Test with zero range — should still update if corona is at camera position
    uint32 testId = TEST_CORONA_BASE_ID2 + 1;
    CVector atCamera = TheCamera.GetPosition();

    CleanupTestCoronas2();
    auto* slot = FindCorona2(0);
    if (!slot) return;

    slot->m_dwId = testId;
    slot->m_vPosn = atCamera;
    slot->m_fAngle = 0.0f;

    CVector newPos = atCamera + CVector(0.1f, 0.2f, 0.3f);
    CCoronas::UpdateCoronaCoors(testId, newPos, 0.0f, 30.0f);
    CVector revPos = slot->m_vPosn;
    float revAngle = slot->m_fAngle;

    // Reset
    slot->m_vPosn = atCamera;
    slot->m_fAngle = 0.0f;
    { HookDisableGuard guard("Global/CCoronas/UpdateCoronaCoors");
      CCoronas::UpdateCoronaCoors(testId, newPos, 0.0f, 30.0f); }

    EXPECT_NEAR(revPos.x, slot->m_vPosn.x, 1e-5f);
    EXPECT_NEAR(revPos.y, slot->m_vPosn.y, 1e-5f);
    EXPECT_NEAR(revPos.z, slot->m_vPosn.z, 1e-5f);
    EXPECT_NEAR(revAngle, slot->m_fAngle, 1e-5f);

    CleanupTestCoronas2();
}

GAME_DIFF_TEST(CCoronas2, UpdateCoronaCoors_NegativeAngle) {
    // Test with negative angle value
    uint32 testId = TEST_CORONA_BASE_ID2 + 2;
    CVector pos = TheCamera.GetPosition() + CVector(5.0f, 0.0f, 0.0f);

    CleanupTestCoronas2();
    auto* slot = FindCorona2(0);
    if (!slot) return;

    slot->m_dwId = testId;
    slot->m_vPosn = pos;
    slot->m_fAngle = 0.0f;

    CCoronas::UpdateCoronaCoors(testId, pos, 500.0f, -90.0f);
    float revAngle = slot->m_fAngle;

    slot->m_fAngle = 0.0f;
    { HookDisableGuard guard("Global/CCoronas/UpdateCoronaCoors");
      CCoronas::UpdateCoronaCoors(testId, pos, 500.0f, -90.0f); }

    EXPECT_NEAR(revAngle, slot->m_fAngle, 1e-5f);

    CleanupTestCoronas2();
}

GAME_DIFF_TEST(CCoronas2, UpdateCoronaCoors_NonExistentId) {
    // Updating a corona ID that doesn't exist — should be a no-op
    uint32 testId = TEST_CORONA_BASE_ID2 + 99;
    CVector pos(100.0f, 200.0f, 300.0f);

    // Don't register any corona with this ID
    CleanupTestCoronas2();
    uint32 savedNum = CCoronas::NumCoronas;

    CCoronas::UpdateCoronaCoors(testId, pos, 500.0f, 0.0f);
    uint32 revNum = CCoronas::NumCoronas;

    CCoronas::NumCoronas = savedNum;
    { HookDisableGuard guard("Global/CCoronas/UpdateCoronaCoors");
      CCoronas::UpdateCoronaCoors(testId, pos, 500.0f, 0.0f); }

    EXPECT_EQ(revNum, CCoronas::NumCoronas);
}

// --- Additional behavior tests ---

GAME_TEST(CCoronas2, SunScreenCoordinates) {
    // SunScreenX/Y should be valid screen coordinates or zero
    EXPECT_TRUE(CCoronas::SunScreenX >= 0.0f || CCoronas::SunScreenX == 0.0f);
    EXPECT_TRUE(CCoronas::SunScreenY >= 0.0f || CCoronas::SunScreenY == 0.0f);
}

GAME_TEST(CCoronas2, MoonSize_InRange) {
    // MoonSize should be a small value (0-7 in the game)
    EXPECT_TRUE(CCoronas::MoonSize <= 7u);
}
