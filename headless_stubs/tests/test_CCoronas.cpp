// test_CCoronas.cpp — Differential + behavior tests for CCoronas.
// Hook paths: Global/CCoronas/<fn>
//
// Corona registration, lookup, and update functions.
// Rendering functions (Render, RenderReflections, RenderSunReflection,
// DoSunAndMoon, Init, Shutdown) use RenderWare and will crash headlessly.
// We test the data management side: RegisterCorona, UpdateCoronaCoors, Update.

#include "StdInc.h"
#include "TestFramework.h"
#include "Coronas.h"

// Use a high ID range unlikely to collide with game coronas
static constexpr uint32 TEST_CORONA_BASE_ID = 0xFFFE0000;

// Helper: find corona by ID in the array
static CRegisteredCorona* FindCorona(uint32 id) {
    for (auto& c : CCoronas::aCoronas) {
        if (c.m_dwId == id) return &c;
    }
    return nullptr;
}

// Helper: clean up test coronas
static void CleanupTestCoronas() {
    for (auto& c : CCoronas::aCoronas) {
        if (c.m_dwId >= TEST_CORONA_BASE_ID) {
            c.m_dwId = 0;
            c.m_Color.a = 0;
            c.m_FadedIntensity = 0;
        }
    }
}

// --- Differential tests ---

GAME_DIFF_TEST(CCoronas, UpdateCoronaCoors) {
    // Register a test corona, then update its coordinates.
    // Compare reversed vs original UpdateCoronaCoors.
    uint32 testId = TEST_CORONA_BASE_ID + 1;
    CVector nearCamera = TheCamera.GetPosition() + CVector(10.0f, 0.0f, 0.0f);

    // Manually insert a corona entry
    CleanupTestCoronas();
    auto* freeSlot = FindCorona(0); // find unused slot
    if (!freeSlot) return;

    freeSlot->m_dwId = testId;
    freeSlot->m_vPosn = nearCamera;
    freeSlot->m_fAngle = 0.0f;

    // Reversed UpdateCoronaCoors
    CVector newPos = nearCamera + CVector(1.0f, 2.0f, 3.0f);
    CCoronas::UpdateCoronaCoors(testId, newPos, 500.0f, 45.0f);
    CVector revPos = freeSlot->m_vPosn;
    float revAngle = freeSlot->m_fAngle;

    // Reset and run original
    freeSlot->m_vPosn = nearCamera;
    freeSlot->m_fAngle = 0.0f;
    { HookDisableGuard guard("Global/CCoronas/UpdateCoronaCoors");
      CCoronas::UpdateCoronaCoors(testId, newPos, 500.0f, 45.0f); }
    EXPECT_NEAR(revPos.x, freeSlot->m_vPosn.x, 1e-5f);
    EXPECT_NEAR(revPos.y, freeSlot->m_vPosn.y, 1e-5f);
    EXPECT_NEAR(revPos.z, freeSlot->m_vPosn.z, 1e-5f);
    EXPECT_NEAR(revAngle, freeSlot->m_fAngle, 1e-5f);

    CleanupTestCoronas();
}

GAME_DIFF_TEST(CCoronas, UpdateCoronaCoors_OutOfRange) {
    // Corona beyond range should not be updated
    uint32 testId = TEST_CORONA_BASE_ID + 2;
    CVector farAway = TheCamera.GetPosition() + CVector(99999.0f, 0.0f, 0.0f);

    CleanupTestCoronas();
    auto* freeSlot = FindCorona(0);
    if (!freeSlot) return;

    freeSlot->m_dwId = testId;
    freeSlot->m_vPosn = farAway;

    CVector newPos = farAway + CVector(1.0f, 0.0f, 0.0f);
    CCoronas::UpdateCoronaCoors(testId, newPos, 100.0f, 0.0f); // range=100, way too short
    CVector revPos = freeSlot->m_vPosn;

    freeSlot->m_vPosn = farAway;
    { HookDisableGuard guard("Global/CCoronas/UpdateCoronaCoors");
      CCoronas::UpdateCoronaCoors(testId, newPos, 100.0f, 0.0f); }
    EXPECT_NEAR(revPos.x, freeSlot->m_vPosn.x, 1e-5f);
    EXPECT_NEAR(revPos.y, freeSlot->m_vPosn.y, 1e-5f);

    CleanupTestCoronas();
}

GAME_DIFF_TEST(CCoronas, RegisterCorona_Type) {
    // Register a corona using the type overload, compare slot state.
    uint32 testId = TEST_CORONA_BASE_ID + 3;
    CVector pos = TheCamera.GetPosition() + CVector(5.0f, 5.0f, 0.0f);

    // Reversed
    CleanupTestCoronas();
    uint32 savedNumCoronas = CCoronas::NumCoronas;
    CCoronas::RegisterCorona(testId, nullptr, 255, 128, 64, 200,
        pos, 2.0f, 500.0f,
        CORONATYPE_SHINYSTAR, FLARETYPE_NONE, CORREFL_NONE,
        LOSCHECK_OFF, TRAIL_OFF, 0.0f, false, 0.0f, false, 15.0f, false, false);
    auto* revCorona = FindCorona(testId);
    uint8 revR = 0, revG = 0, revB = 0, revA = 0;
    float revSize = 0, revRange = 0;
    bool revFound = (revCorona != nullptr);
    if (revCorona) {
        revR = revCorona->m_Color.r;
        revG = revCorona->m_Color.g;
        revB = revCorona->m_Color.b;
        revA = revCorona->m_Color.a;
        revSize = revCorona->m_fSize;
        revRange = revCorona->m_fFarClip;
    }

    // Reset and run original
    CleanupTestCoronas();
    CCoronas::NumCoronas = savedNumCoronas;
    { HookDisableGuard guard("Global/CCoronas/RegisterCorona-texture");
      CCoronas::RegisterCorona(testId, nullptr, 255, 128, 64, 200,
          pos, 2.0f, 500.0f,
          CORONATYPE_SHINYSTAR, FLARETYPE_NONE, CORREFL_NONE,
          LOSCHECK_OFF, TRAIL_OFF, 0.0f, false, 0.0f, false, 15.0f, false, false); }
    auto* origCorona = FindCorona(testId);
    bool origFound = (origCorona != nullptr);

    EXPECT_EQ(revFound, origFound);
    if (revFound && origFound) {
        EXPECT_EQ(revR, origCorona->m_Color.r);
        EXPECT_EQ(revG, origCorona->m_Color.g);
        EXPECT_EQ(revB, origCorona->m_Color.b);
        EXPECT_EQ(revA, origCorona->m_Color.a);
        EXPECT_NEAR(revSize, origCorona->m_fSize, 1e-5f);
        EXPECT_NEAR(revRange, origCorona->m_fFarClip, 1e-5f);
    }

    CleanupTestCoronas();
    CCoronas::NumCoronas = savedNumCoronas;
}

GAME_DIFF_TEST(CCoronas, RegisterCorona_Texture) {
    // Register using the texture overload (RwTexture* version)
    uint32 testId = TEST_CORONA_BASE_ID + 4;
    CVector pos = TheCamera.GetPosition() + CVector(-5.0f, 5.0f, 0.0f);

    CleanupTestCoronas();
    uint32 savedNumCoronas = CCoronas::NumCoronas;
    CCoronas::RegisterCorona(testId, nullptr, 100, 200, 50, 180,
        pos, 3.0f, 300.0f,
        (RwTexture*)nullptr, FLARETYPE_NONE, CORREFL_NONE,
        LOSCHECK_OFF, TRAIL_OFF, 0.0f, false, 0.0f, true, 10.0f, false, false);
    auto* revCorona = FindCorona(testId);
    bool revFound = (revCorona != nullptr);
    uint8 revFaded = revCorona ? revCorona->m_FadedIntensity : 0;

    CleanupTestCoronas();
    CCoronas::NumCoronas = savedNumCoronas;
    { HookDisableGuard guard("Global/CCoronas/RegisterCorona-type");
      CCoronas::RegisterCorona(testId, nullptr, 100, 200, 50, 180,
          pos, 3.0f, 300.0f,
          (RwTexture*)nullptr, FLARETYPE_NONE, CORREFL_NONE,
          LOSCHECK_OFF, TRAIL_OFF, 0.0f, false, 0.0f, true, 10.0f, false, false); }
    auto* origCorona = FindCorona(testId);
    bool origFound = (origCorona != nullptr);

    EXPECT_EQ(revFound, origFound);
    if (revFound && origFound) {
        // fullBrightAtStart=true → FadedIntensity should be 255
        EXPECT_EQ(revFaded, origCorona->m_FadedIntensity);
    }

    CleanupTestCoronas();
    CCoronas::NumCoronas = savedNumCoronas;
}

// --- Behavior tests ---

GAME_TEST(CCoronas, ArraySize) {
    EXPECT_EQ(CCoronas::aCoronas.size(), (size_t)MAX_NUM_CORONAS);
}

GAME_TEST(CCoronas, LightsMultPositive) {
    EXPECT_TRUE(CCoronas::LightsMult >= 0.0f);
    EXPECT_TRUE(CCoronas::LightsMult <= 1.0f);
}

GAME_TEST(CCoronas, RegisterAndFind) {
    uint32 testId = TEST_CORONA_BASE_ID + 10;
    CVector pos = TheCamera.GetPosition() + CVector(10.0f, 0.0f, 0.0f);
    uint32 savedNum = CCoronas::NumCoronas;

    CleanupTestCoronas();
    CCoronas::RegisterCorona(testId, nullptr, 255, 0, 0, 200,
        pos, 1.0f, 500.0f,
        CORONATYPE_SHINYSTAR, FLARETYPE_NONE, CORREFL_NONE,
        LOSCHECK_OFF, TRAIL_OFF, 0.0f, false, 0.0f, false, 15.0f, false, false);

    auto* c = FindCorona(testId);
    EXPECT_TRUE(c != nullptr);
    if (c) {
        EXPECT_EQ(c->m_Color.r, (uint8)255);
        EXPECT_EQ(c->m_Color.g, (uint8)0);
        EXPECT_EQ(c->m_Color.b, (uint8)0);
        EXPECT_NEAR(c->m_fSize, 1.0f, 1e-5f);
    }

    CleanupTestCoronas();
    CCoronas::NumCoronas = savedNum;
}

GAME_TEST(CCoronas, UpdateCoronaCoors_MovesPosition) {
    uint32 testId = TEST_CORONA_BASE_ID + 11;
    CVector pos = TheCamera.GetPosition() + CVector(10.0f, 0.0f, 0.0f);
    uint32 savedNum = CCoronas::NumCoronas;

    CleanupTestCoronas();
    CCoronas::RegisterCorona(testId, nullptr, 255, 255, 255, 200,
        pos, 1.0f, 500.0f,
        CORONATYPE_SHINYSTAR, FLARETYPE_NONE, CORREFL_NONE,
        LOSCHECK_OFF, TRAIL_OFF, 0.0f, false, 0.0f, false, 15.0f, false, false);

    CVector newPos = pos + CVector(5.0f, 5.0f, 5.0f);
    CCoronas::UpdateCoronaCoors(testId, newPos, 500.0f, 90.0f);

    auto* c = FindCorona(testId);
    EXPECT_TRUE(c != nullptr);
    if (c) {
        EXPECT_NEAR(c->m_vPosn.x, newPos.x, 1e-5f);
        EXPECT_NEAR(c->m_vPosn.y, newPos.y, 1e-5f);
        EXPECT_NEAR(c->m_vPosn.z, newPos.z, 1e-5f);
        EXPECT_NEAR(c->m_fAngle, 90.0f, 1e-5f);
    }

    CleanupTestCoronas();
    CCoronas::NumCoronas = savedNum;
}
