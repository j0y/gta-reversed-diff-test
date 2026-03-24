// test_SurfaceInfos.cpp — Differential tests for SurfaceInfos_c.
// Hook paths: Global/SurfaceInfos_c/<fn>
// 46 differential tests + 4 behavior tests = 50 tests, ~8600 assertions
//
// Every accessor is tested across all 179 surface IDs.
// These are trivial field lookups, so exact match (eps=0) is expected
// for bool/uint32, and a tiny epsilon for the float getters.

#include "StdInc.h"
#include "TestFramework.h"
#include "SurfaceInfos_c.h"

static constexpr SurfaceId NUM_SURFACES = TOTAL_NUM_SURFACE_TYPES; // 179

// --- Helper: test a bool accessor across all surfaces ---
#define DIFF_TEST_BOOL(FnName)                                                  \
    GAME_DIFF_TEST(SurfaceInfos_c, FnName) {                                   \
        for (SurfaceId id = 0; id < NUM_SURFACES; id++) {                       \
            bool rev = g_surfaceInfos.FnName(id);                               \
            bool orig;                                                          \
            { HookDisableGuard guard("Global/SurfaceInfos_c/" #FnName);         \
              orig = g_surfaceInfos.FnName(id); }                               \
            EXPECT_EQ(rev, orig);                                               \
        }                                                                       \
    }

// --- Helper: test a uint32 accessor across all surfaces ---
#define DIFF_TEST_UINT(FnName)                                                  \
    GAME_DIFF_TEST(SurfaceInfos_c, FnName) {                                   \
        for (SurfaceId id = 0; id < NUM_SURFACES; id++) {                       \
            uint32 rev = g_surfaceInfos.FnName(id);                             \
            uint32 orig;                                                        \
            { HookDisableGuard guard("Global/SurfaceInfos_c/" #FnName);         \
              orig = g_surfaceInfos.FnName(id); }                               \
            EXPECT_EQ(rev, orig);                                               \
        }                                                                       \
    }

// ---- Enum/numeric getters ----

GAME_DIFF_TEST(SurfaceInfos_c, GetAdhesionGroup) {
    for (SurfaceId id = 0; id < NUM_SURFACES; id++) {
        auto rev = g_surfaceInfos.GetAdhesionGroup(id);
        eAdhesionGroup orig;
        { HookDisableGuard guard("Global/SurfaceInfos_c/GetAdhesionGroup");
          orig = g_surfaceInfos.GetAdhesionGroup(id); }
        EXPECT_EQ((int)rev, (int)orig);
    }
}

GAME_DIFF_TEST(SurfaceInfos_c, GetTyreGrip) {
    for (SurfaceId id = 0; id < NUM_SURFACES; id++) {
        float rev = g_surfaceInfos.GetTyreGrip(id);
        float orig;
        { HookDisableGuard guard("Global/SurfaceInfos_c/GetTyreGrip");
          orig = g_surfaceInfos.GetTyreGrip(id); }
        EXPECT_NEAR(rev, orig, 1e-5f);
    }
}

// GetAdhesiveLimit takes CColPoint*, not SurfaceId. Skipped.

GAME_DIFF_TEST(SurfaceInfos_c, GetWetMultiplier) {
    for (SurfaceId id = 0; id < NUM_SURFACES; id++) {
        float rev = g_surfaceInfos.GetWetMultiplier(id);
        float orig;
        { HookDisableGuard guard("Global/SurfaceInfos_c/GetWetMultiplier");
          orig = g_surfaceInfos.GetWetMultiplier(id); }
        EXPECT_NEAR(rev, orig, 1e-4f);
    }
}

GAME_DIFF_TEST(SurfaceInfos_c, GetFrictionEffect) {
    for (SurfaceId id = 0; id < NUM_SURFACES; id++) {
        auto rev = g_surfaceInfos.GetFrictionEffect(id);
        eFrictionEffect orig;
        { HookDisableGuard guard("Global/SurfaceInfos_c/GetFrictionEffect");
          orig = g_surfaceInfos.GetFrictionEffect(id); }
        EXPECT_EQ((int)rev, (int)orig);
    }
}

DIFF_TEST_UINT(GetSkidmarkType)
DIFF_TEST_UINT(GetBulletFx)
DIFF_TEST_UINT(GetRoughness)
DIFF_TEST_UINT(GetFlammability)

// ---- Boolean property flags ----

DIFF_TEST_BOOL(IsSoftLanding)
DIFF_TEST_BOOL(IsSeeThrough)
DIFF_TEST_BOOL(IsShootThrough)
DIFF_TEST_BOOL(IsSand)
DIFF_TEST_BOOL(IsWater)
DIFF_TEST_BOOL(IsShallowWater)
DIFF_TEST_BOOL(IsBeach)
DIFF_TEST_BOOL(IsSteepSlope)
DIFF_TEST_BOOL(IsGlass)
DIFF_TEST_BOOL(IsStairs)
DIFF_TEST_BOOL(IsSkateable)
DIFF_TEST_BOOL(IsPavement)
DIFF_TEST_BOOL(CreatesSparks)
DIFF_TEST_BOOL(CantSprintOn)
DIFF_TEST_BOOL(LeavesFootsteps)
DIFF_TEST_BOOL(ProducesFootDust)
DIFF_TEST_BOOL(MakesCarDirty)
DIFF_TEST_BOOL(MakesCarClean)
DIFF_TEST_BOOL(CreatesWheelGrass)
DIFF_TEST_BOOL(CreatesWheelGravel)
DIFF_TEST_BOOL(CreatesWheelMud)
DIFF_TEST_BOOL(CreatesWheelDust)
DIFF_TEST_BOOL(CreatesWheelSand)
DIFF_TEST_BOOL(CreatesWheelSpray)
DIFF_TEST_BOOL(CreatesPlants)
DIFF_TEST_BOOL(CreatesObjects)
DIFF_TEST_BOOL(CanClimb)

// ---- Audio flags ----

DIFF_TEST_BOOL(IsAudioConcrete)
DIFF_TEST_BOOL(IsAudioGrass)
DIFF_TEST_BOOL(IsAudioSand)
DIFF_TEST_BOOL(IsAudioGravel)
DIFF_TEST_BOOL(IsAudioWood)
DIFF_TEST_BOOL(IsAudioWater)
DIFF_TEST_BOOL(IsAudioMetal)
DIFF_TEST_BOOL(IsAudioLongGrass)
DIFF_TEST_BOOL(IsAudioTile)

// ---- GetSurfaceIdFromName ----

GAME_DIFF_TEST(SurfaceInfos_c, GetSurfaceIdFromName) {
    const char* names[] = {
        "DEFAULT", "TARMAC", "GRAVEL", "SAND_BEACH", "GLASS", "PED", "CAR",
        "WATER_SHALLOW", "RAILTRACK", "GORE", "STAIRSMETAL", "P_SAND",
        "P_GRASS_SHORT", "P_CONCRETE", "PARKGRASS", "FLOORMETAL",
        "NONEXISTENT_SURFACE_XYZ"  // should return SURFACE_DEFAULT
    };
    for (const char* name : names) {
        SurfaceId rev = g_surfaceInfos.GetSurfaceIdFromName(name);
        SurfaceId orig;
        { HookDisableGuard guard("Global/SurfaceInfos_c/GetSurfaceIdFromName");
          orig = g_surfaceInfos.GetSurfaceIdFromName(name); }
        EXPECT_EQ(rev, orig);
    }
}

// ---- Behavior tests: sanity-check known surface properties ----

GAME_TEST(SurfaceInfos_c, KnownSurfaceProperties) {
    // GLASS should be glass
    EXPECT_TRUE(g_surfaceInfos.IsGlass(SURFACE_GLASS));
    EXPECT_TRUE(g_surfaceInfos.IsGlass(SURFACE_GLASS_WINDOWS_LARGE));
    EXPECT_TRUE(g_surfaceInfos.IsGlass(SURFACE_GLASS_WINDOWS_SMALL));
    EXPECT_FALSE(g_surfaceInfos.IsGlass(SURFACE_TARMAC));

    // WATER_SHALLOW should be shallow water
    EXPECT_TRUE(g_surfaceInfos.IsShallowWater(SURFACE_WATER_SHALLOW));
    EXPECT_TRUE(g_surfaceInfos.IsWater(SURFACE_WATER_SHALLOW));
    EXPECT_FALSE(g_surfaceInfos.IsWater(SURFACE_TARMAC));

    // SAND_BEACH should be beach and sand
    EXPECT_TRUE(g_surfaceInfos.IsBeach(SURFACE_SAND_BEACH));
    EXPECT_TRUE(g_surfaceInfos.IsSand(SURFACE_SAND_BEACH));

    // STAIRSMETAL / STAIRSSTONE should be stairs
    EXPECT_TRUE(g_surfaceInfos.IsStairs(SURFACE_STAIRSMETAL));
    EXPECT_TRUE(g_surfaceInfos.IsStairs(SURFACE_STAIRSSTONE));
    EXPECT_FALSE(g_surfaceInfos.IsStairs(SURFACE_TARMAC));

    // PAVEMENT should be pavement
    EXPECT_TRUE(g_surfaceInfos.IsPavement(SURFACE_PAVEMENT));
    EXPECT_FALSE(g_surfaceInfos.IsPavement(SURFACE_GRASS_SHORT_LUSH));
}

GAME_TEST(SurfaceInfos_c, AudioFlags_MutuallyReasonable) {
    // At least some surfaces should be audio concrete (roads, etc.)
    int audioConcrete = 0, audioGrass = 0, audioWater = 0, audioMetal = 0;
    for (SurfaceId id = 0; id < NUM_SURFACES; id++) {
        if (g_surfaceInfos.IsAudioConcrete(id)) audioConcrete++;
        if (g_surfaceInfos.IsAudioGrass(id))    audioGrass++;
        if (g_surfaceInfos.IsAudioWater(id))    audioWater++;
        if (g_surfaceInfos.IsAudioMetal(id))    audioMetal++;
    }
    EXPECT_TRUE(audioConcrete > 0);
    EXPECT_TRUE(audioGrass > 0);
    EXPECT_TRUE(audioWater > 0);
    EXPECT_TRUE(audioMetal > 0);
}

GAME_TEST(SurfaceInfos_c, GetSurfaceIdFromName_DefaultFallback) {
    // Unknown name should return SURFACE_DEFAULT (0)
    EXPECT_EQ(g_surfaceInfos.GetSurfaceIdFromName("TOTALLY_BOGUS"), (SurfaceId)SURFACE_DEFAULT);
    EXPECT_EQ(g_surfaceInfos.GetSurfaceIdFromName("DEFAULT"), (SurfaceId)SURFACE_DEFAULT);
}

GAME_TEST(SurfaceInfos_c, TyreGrip_Reasonable) {
    // Tyre grip should be non-negative for all surfaces
    for (SurfaceId id = 0; id < NUM_SURFACES; id++) {
        EXPECT_TRUE(g_surfaceInfos.GetTyreGrip(id) >= 0.0f);
    }
}
