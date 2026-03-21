// test_CWaterLevel.cpp — Differential tests for CWaterLevel water height lookups.
// Hook paths: Global/CWaterLevel/<fn>
//
// Water level queries are critical for vehicle buoyancy, swimming, and
// boat physics. The main GetWaterLevel function is tested at several
// known locations (ocean, river, land).

#include "StdInc.h"
#include "TestFramework.h"
#include "WaterLevel.h"

// GetWaterLevel overload registered with empty suffix => hook name "GetWaterLevel-"
// Signature: bool GetWaterLevel(float x, float y, float z, float& outLevel, uint8 touching, CVector* normals)

GAME_DIFF_TEST(CWaterLevel, GetWaterLevel_Ocean) {
    // Santa Maria Beach — known ocean location
    float origLevel = 0.f, revLevel = 0.f;
    bool orig, rev;
    { HookDisableGuard guard("Global/CWaterLevel/GetWaterLevel-");
      orig = CWaterLevel::GetWaterLevel(370.f, -1840.f, 0.f, origLevel, 0, nullptr); }
    rev = CWaterLevel::GetWaterLevel(370.f, -1840.f, 0.f, revLevel, 0, nullptr);
    EXPECT_EQ(orig, rev);
    if (orig && rev) {
        EXPECT_NEAR(origLevel, revLevel, 1e-3f);
    }
}

GAME_DIFF_TEST(CWaterLevel, GetWaterLevel_River) {
    // Flint County river area
    float origLevel = 0.f, revLevel = 0.f;
    bool orig, rev;
    { HookDisableGuard guard("Global/CWaterLevel/GetWaterLevel-");
      orig = CWaterLevel::GetWaterLevel(-800.f, -1100.f, 0.f, origLevel, 0, nullptr); }
    rev = CWaterLevel::GetWaterLevel(-800.f, -1100.f, 0.f, revLevel, 0, nullptr);
    EXPECT_EQ(orig, rev);
    if (orig && rev) {
        EXPECT_NEAR(origLevel, revLevel, 1e-3f);
    }
}

GAME_DIFF_TEST(CWaterLevel, GetWaterLevel_Land) {
    // Grove Street — inland, no water expected
    float origLevel = 0.f, revLevel = 0.f;
    bool orig, rev;
    { HookDisableGuard guard("Global/CWaterLevel/GetWaterLevel-");
      orig = CWaterLevel::GetWaterLevel(2488.f, -1666.f, 13.f, origLevel, 0, nullptr); }
    rev = CWaterLevel::GetWaterLevel(2488.f, -1666.f, 13.f, revLevel, 0, nullptr);
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CWaterLevel, GetWaterLevel_WithNormals) {
    // Ocean with normals output
    float origLevel = 0.f, revLevel = 0.f;
    CVector origNorm{}, revNorm{};
    bool orig, rev;
    { HookDisableGuard guard("Global/CWaterLevel/GetWaterLevel-");
      orig = CWaterLevel::GetWaterLevel(370.f, -1840.f, 0.f, origLevel, 1, &origNorm); }
    rev = CWaterLevel::GetWaterLevel(370.f, -1840.f, 0.f, revLevel, 1, &revNorm);
    EXPECT_EQ(orig, rev);
    if (orig && rev) {
        EXPECT_NEAR(origLevel, revLevel, 1e-3f);
        EXPECT_VEC_NEAR(origNorm, revNorm, 1e-3f);
    }
}

GAME_DIFF_TEST(CWaterLevel, GetWaterLevel_DeepOcean) {
    // Far out at sea — edge of map
    float origLevel = 0.f, revLevel = 0.f;
    bool orig, rev;
    { HookDisableGuard guard("Global/CWaterLevel/GetWaterLevel-");
      orig = CWaterLevel::GetWaterLevel(-2500.f, 2500.f, 0.f, origLevel, 0, nullptr); }
    rev = CWaterLevel::GetWaterLevel(-2500.f, 2500.f, 0.f, revLevel, 0, nullptr);
    EXPECT_EQ(orig, rev);
    if (orig && rev) {
        EXPECT_NEAR(origLevel, revLevel, 1e-3f);
    }
}
