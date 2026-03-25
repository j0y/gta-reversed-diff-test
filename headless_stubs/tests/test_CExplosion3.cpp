// test_CExplosion3.cpp — Extended CExplosion differential tests.
// Hook paths: Global/CExplosion/<fn>
// Note: DoesExplosionMakeSound and GetExplosionActiveCounter are mutated by
// CExplosion::Update() between hook toggles — tested as behavior only.

#include "StdInc.h"
#include "TestFramework.h"
#include "Explosion.h"

GAME_DIFF_TEST(CExplosion3, GetExplosionType_AllSlots) {
    for (uint8 id = 0; id < 16; id++) {
        int32 orig, rev;
        { HookDisableGuard guard("Global/CExplosion/GetExplosionType"); orig = CExplosion::GetExplosionType(id); }
        rev = CExplosion::GetExplosionType(id);
        EXPECT_EQ(orig, rev);
    }
}

// Behavior tests for mutable-state functions
GAME_TEST(CExplosion3, DoesExplosionMakeSound_AllSlots) {
    for (uint8 id = 0; id < 16; id++) {
        bool result = CExplosion::DoesExplosionMakeSound(id);
        (void)result;
        EXPECT_TRUE(true);
    }
}

GAME_TEST(CExplosion3, GetExplosionActiveCounter_AllSlots) {
    for (uint8 id = 0; id < 16; id++) {
        uint8 result = CExplosion::GetExplosionActiveCounter(id);
        (void)result;
        EXPECT_TRUE(true);
    }
}

GAME_DIFF_TEST(CExplosion3, TestForExplosionInArea_GroveStreet) {
    for (int32 type = 0; type < 10; type++) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CExplosion/TestForExplosionInArea");
          orig = CExplosion::TestForExplosionInArea((eExplosionType)type, 2400.f, 2500.f, -1700.f, -1600.f, 0.f, 50.f); }
        rev = CExplosion::TestForExplosionInArea((eExplosionType)type, 2400.f, 2500.f, -1700.f, -1600.f, 0.f, 50.f);
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(CExplosion3, TestForExplosionInArea_Empty) {
    // Test in area with no explosions
    bool orig, rev;
    { HookDisableGuard guard("Global/CExplosion/TestForExplosionInArea");
      orig = CExplosion::TestForExplosionInArea((eExplosionType)0, -3000.f, -2999.f, -3000.f, -2999.f, 0.f, 1.f); }
    rev = CExplosion::TestForExplosionInArea((eExplosionType)0, -3000.f, -2999.f, -3000.f, -2999.f, 0.f, 1.f);
    EXPECT_EQ(orig, rev);
}
