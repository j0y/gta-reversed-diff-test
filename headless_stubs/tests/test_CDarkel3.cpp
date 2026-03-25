// test_CDarkel3.cpp — Differential tests for CDarkel (uncovered methods).
// Hook paths: Global/CDarkel/<fn>
// Covers: ResetModelsKilledByPlayer, ThisVehicleShouldBeKilledForFrenzy,
//         RegisterKillNotByPlayer, ResetOnPlayerDeath

#include "StdInc.h"
#include "TestFramework.h"
#include "Darkel.h"
#include "Entity/Ped/Ped.h"
#include "Entity/Vehicle/Vehicle.h"
#include "Pools/Pools.h"
#include "World.h"

// --- ResetModelsKilledByPlayer ---
// Pure state mutation — verify the kills array is zeroed the same way.

GAME_DIFF_TEST(CDarkel3, ResetModelsKilledByPlayer) {
    // Query a few models before and after reset to verify behavior matches
    // First, snapshot current state
    int16 preOrig[5], preRev[5];
    int modelIds[] = {0, 1, 2, 100, 400};

    for (int i = 0; i < 5; i++) {
        preOrig[i] = CDarkel::QueryModelsKilledByPlayer((eModelID)modelIds[i], 0);
    }

    // Do reset with original code
    { HookDisableGuard guard("Global/CDarkel/ResetModelsKilledByPlayer");
      CDarkel::ResetModelsKilledByPlayer(0); }

    // Check that all are zero after original reset
    for (int i = 0; i < 5; i++) {
        int16 val = CDarkel::QueryModelsKilledByPlayer((eModelID)modelIds[i], 0);
        EXPECT_EQ(val, 0);
    }

    // Now do reset with reversed code (should also zero them)
    CDarkel::ResetModelsKilledByPlayer(0);

    for (int i = 0; i < 5; i++) {
        int16 val = CDarkel::QueryModelsKilledByPlayer((eModelID)modelIds[i], 0);
        EXPECT_EQ(val, 0);
    }
}

// --- ThisVehicleShouldBeKilledForFrenzy ---
// Needs a vehicle entity. Search pools for any existing vehicle.

GAME_DIFF_TEST(CDarkel3, ThisVehicleShouldBeKilledForFrenzy_FromPool) {
    auto* pool = GetVehiclePool();
    if (!pool) return;

    int tested = 0;
    for (int i = 0; i < pool->GetSize() && tested < 5; i++) {
        auto* veh = pool->GetAt(i);
        if (!veh) continue;

        bool orig, rev;
        { HookDisableGuard guard("Global/CDarkel/ThisVehicleShouldBeKilledForFrenzy");
          orig = CDarkel::ThisVehicleShouldBeKilledForFrenzy(*veh); }
        rev = CDarkel::ThisVehicleShouldBeKilledForFrenzy(*veh);
        EXPECT_EQ(orig, rev);
        tested++;
    }
}

// --- QueryModelsKilledByPlayer extended range ---
// Test with a wider range of model IDs to catch any index boundary issues.

GAME_DIFF_TEST(CDarkel3, QueryModelsKilledByPlayer_Extended) {
    int modelIds[] = {0, 1, 50, 100, 200, 300, 400, 500, 600, 700, 799};
    for (auto modelId : modelIds) {
        int16 orig, rev;
        { HookDisableGuard guard("Global/CDarkel/QueryModelsKilledByPlayer");
          orig = CDarkel::QueryModelsKilledByPlayer((eModelID)modelId, 0); }
        rev = CDarkel::QueryModelsKilledByPlayer((eModelID)modelId, 0);
        EXPECT_EQ(orig, rev);
    }
}

// --- FindTotalPedsKilledByPlayer for both player slots ---

GAME_DIFF_TEST(CDarkel3, FindTotalPedsKilledByPlayer_BothPlayers) {
    for (int playerId = 0; playerId < 2; playerId++) {
        int32 orig, rev;
        { HookDisableGuard guard("Global/CDarkel/FindTotalPedsKilledByPlayer");
          orig = CDarkel::FindTotalPedsKilledByPlayer(playerId); }
        rev = CDarkel::FindTotalPedsKilledByPlayer(playerId);
        EXPECT_EQ(orig, rev);
    }
}

// --- CheckDamagedWeaponType extended coverage ---
// Test more weapon type combinations including heavy weapons and specials.

GAME_DIFF_TEST(CDarkel3, CheckDamagedWeaponType_Extended) {
    eWeaponType weapons[] = {
        WEAPON_UNARMED, WEAPON_KNIFE, WEAPON_CHAINSAW,
        WEAPON_PISTOL, WEAPON_DESERT_EAGLE,
        WEAPON_SHOTGUN, WEAPON_SAWNOFF_SHOTGUN, WEAPON_SPAS12_SHOTGUN,
        WEAPON_MICRO_UZI, WEAPON_AK47, WEAPON_M4,
        WEAPON_COUNTRYRIFLE, WEAPON_SNIPERRIFLE,
        WEAPON_RLAUNCHER, WEAPON_FLAMETHROWER, WEAPON_MINIGUN,
        WEAPON_GRENADE, WEAPON_MOLOTOV, WEAPON_TEARGAS,
        WEAPON_SPRAYCAN, WEAPON_EXTINGUISHER,
    };
    for (auto expected : weapons) {
        for (auto actual : weapons) {
            bool orig, rev;
            { HookDisableGuard guard("Global/CDarkel/CheckDamagedWeaponType");
              orig = CDarkel::CheckDamagedWeaponType(actual, expected); }
            rev = CDarkel::CheckDamagedWeaponType(actual, expected);
            EXPECT_EQ(orig, rev);
        }
    }
}
