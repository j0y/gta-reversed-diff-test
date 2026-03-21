// test_CExplosion.cpp — Differential tests for CExplosion static functions.
// Hook paths: Global/CExplosion/<fn>
// All 16 explosion slots are accessible via index (0-15).
//
// Inspired by gta-reversed issues:
//   #1170 — Crashes from unexpected entity state during explosions
//   General explosion system integrity

#include "StdInc.h"
#include "TestFramework.h"
#include "Explosion.h"

// --- Per-slot queries ---
// ActiveCounter and MakeSound are modified by CExplosion::Update() between
// hook toggles, so differential testing is unreliable. Use behavior tests.

GAME_TEST(CExplosion, GetExplosionActiveCounter_AllSlots) {
    for (uint8 id = 0; id < 16; id++) {
        uint8 counter = CExplosion::GetExplosionActiveCounter(id);
        (void)counter; // just ensure no crash
    }
    EXPECT_TRUE(true);
}

GAME_TEST(CExplosion, DoesExplosionMakeSound_AllSlots) {
    for (uint8 id = 0; id < 16; id++) {
        bool sound = CExplosion::DoesExplosionMakeSound(id);
        (void)sound;
    }
    EXPECT_TRUE(true);
}

GAME_DIFF_TEST(CExplosion, GetExplosionType_AllSlots) {
    for (uint8 id = 0; id < 16; id++) {
        int32 orig, rev;
        { HookDisableGuard guard("Global/CExplosion/GetExplosionType");
          orig = CExplosion::GetExplosionType(id); }
        rev = CExplosion::GetExplosionType(id);
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(CExplosion, GetExplosionPosition_AllSlots) {
    for (uint8 id = 0; id < 16; id++) {
        const CVector* orig;
        const CVector* rev;
        { HookDisableGuard guard("Global/CExplosion/GetExplosionPosition");
          orig = &CExplosion::GetExplosionPosition(id); }
        rev = &CExplosion::GetExplosionPosition(id);
        // Pointers should reference the same static array slot
        EXPECT_EQ(orig, rev);
    }
}

// --- Area test queries ---
// TestForExplosionInArea with various explosion types and areas

GAME_DIFF_TEST(CExplosion, TestForExplosionInArea_GroveStreet) {
    eExplosionType types[] = {
        EXPLOSION_GRENADE, EXPLOSION_CAR, EXPLOSION_ROCKET,
        EXPLOSION_MOLOTOV, EXPLOSION_MINE, EXPLOSION_SMALL
    };
    for (auto type : types) {
        bool orig, rev;
        // Test around Grove Street area
        { HookDisableGuard guard("Global/CExplosion/TestForExplosionInArea");
          orig = CExplosion::TestForExplosionInArea(type, 2400.f, 2550.f, -1750.f, -1600.f, 0.f, 100.f); }
        rev = CExplosion::TestForExplosionInArea(type, 2400.f, 2550.f, -1750.f, -1600.f, 0.f, 100.f);
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(CExplosion, TestForExplosionInArea_WorldCenter) {
    // Large area around world center
    bool orig, rev;
    { HookDisableGuard guard("Global/CExplosion/TestForExplosionInArea");
      orig = CExplosion::TestForExplosionInArea(EXPLOSION_CAR, -3000.f, 3000.f, -3000.f, 3000.f, -100.f, 500.f); }
    rev = CExplosion::TestForExplosionInArea(EXPLOSION_CAR, -3000.f, 3000.f, -3000.f, 3000.f, -100.f, 500.f);
    EXPECT_EQ(orig, rev);
}
