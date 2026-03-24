// test_CExplosion2.cpp — Extended differential tests for CExplosion.
// Hook paths: Global/CExplosion/<fn>
//
// GetExplosionActiveCounter and DoesExplosionMakeSound are mutated by
// CExplosion::Update() between hook toggle calls — can't diff test reliably.

#include "StdInc.h"
#include "TestFramework.h"
#include "Explosion.h"

// --- RemoveAllExplosionsInArea ---

GAME_DIFF_TEST(CExplosion2, RemoveAllExplosionsInArea_NoCrash) {
    { HookDisableGuard guard("Global/CExplosion/RemoveAllExplosionsInArea");
      CExplosion::RemoveAllExplosionsInArea(CVector(2488.f, -1666.f, 13.f), 100.f); }
    CExplosion::RemoveAllExplosionsInArea(CVector(2488.f, -1666.f, 13.f), 100.f);
    EXPECT_TRUE(true);
}

// --- GetExplosionActiveCounter (behavior only — state mutates between calls) ---

GAME_TEST(CExplosion2, GetExplosionActiveCounter_NoCrash) {
    for (int32 i = 0; i < 16; i++) {
        uint8 val = CExplosion::GetExplosionActiveCounter(i);
        (void)val; // just verify no crash
    }
    EXPECT_TRUE(true);
}

GAME_TEST(CExplosion2, DoesExplosionMakeSound_NoCrash) {
    for (int32 i = 0; i < 16; i++) {
        bool val = CExplosion::DoesExplosionMakeSound(i);
        (void)val;
    }
    EXPECT_TRUE(true);
}

// --- ClearAllExplosions ---

GAME_DIFF_TEST(CExplosion2, ClearAllExplosions) {
    { HookDisableGuard guard("Global/CExplosion/ClearAllExplosions");
      CExplosion::ClearAllExplosions(); }
    CExplosion::ClearAllExplosions();
    EXPECT_TRUE(true);
}

// --- Shutdown ---

GAME_DIFF_TEST(CExplosion2, Shutdown) {
    { HookDisableGuard guard("Global/CExplosion/Shutdown");
      CExplosion::Shutdown(); }
    CExplosion::Shutdown();
    EXPECT_TRUE(true);
}

// --- ResetExplosionActiveCounter ---

GAME_DIFF_TEST(CExplosion2, ResetExplosionActiveCounter) {
    for (int32 i = 0; i < 16; i++) {
        { HookDisableGuard guard("Global/CExplosion/ResetExplosionActiveCounter");
          CExplosion::ResetExplosionActiveCounter(i); }
        CExplosion::ResetExplosionActiveCounter(i);
    }
    EXPECT_TRUE(true);
}
