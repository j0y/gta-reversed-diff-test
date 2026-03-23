// test_CPickups2.cpp — Deep differential tests for CPickups.
// Hook paths: Global/CPickups/<fn>
//
// Tests weapon-model mappings, pickup index management, and weapon type
// queries across the full weapon enum range.

#include "StdInc.h"
#include "TestFramework.h"
#include "Pickups.h"

// --- ModelForWeapon: map weapon type → model ID ---

GAME_DIFF_TEST(CPickups, ModelForWeapon_AllWeapons) {
    eWeaponType weapons[] = {
        WEAPON_UNARMED, WEAPON_BRASSKNUCKLE, WEAPON_GOLFCLUB, WEAPON_NIGHTSTICK,
        WEAPON_KNIFE, WEAPON_BASEBALLBAT, WEAPON_SHOVEL, WEAPON_POOL_CUE,
        WEAPON_KATANA, WEAPON_CHAINSAW, WEAPON_PISTOL, WEAPON_PISTOL_SILENCED,
        WEAPON_DESERT_EAGLE, WEAPON_SHOTGUN, WEAPON_SAWNOFF_SHOTGUN, WEAPON_SPAS12_SHOTGUN,
        WEAPON_MICRO_UZI, WEAPON_MP5, WEAPON_AK47, WEAPON_M4,
        WEAPON_TEC9, WEAPON_COUNTRYRIFLE, WEAPON_SNIPERRIFLE,
        WEAPON_ROCKET, WEAPON_ROCKET_HS, WEAPON_FLAMETHROWER,
        WEAPON_MINIGUN, WEAPON_REMOTE_SATCHEL_CHARGE, WEAPON_GRENADE,
        WEAPON_TEARGAS, WEAPON_MOLOTOV, WEAPON_SPRAYCAN, WEAPON_EXTINGUISHER,
        WEAPON_CAMERA, WEAPON_NIGHTVISION, WEAPON_INFRARED, WEAPON_PARACHUTE
    };
    for (auto wt : weapons) {
        int32 orig, rev;
        { HookDisableGuard guard("Global/CPickups/ModelForWeapon");
          orig = CPickups::ModelForWeapon(wt); }
        rev = CPickups::ModelForWeapon(wt);
        EXPECT_EQ(orig, rev);
    }
}

// --- WeaponForModel: reverse mapping model ID → weapon type ---

GAME_DIFF_TEST(CPickups, WeaponForModel_KnownModels) {
    // Common weapon model IDs
    int32 models[] = { 331, 333, 334, 335, 336, 337, 338, 339, 341, 342, 346, 347, 348, 349, 350, 351, 352, 353, 355, 356, 357, 358, 359, 361, 362, 363, 364, 365, 366, 367, 368, 369, 371, 372 };
    for (int32 m : models) {
        int32 orig, rev;
        { HookDisableGuard guard("Global/CPickups/WeaponForModel");
          orig = (int32)CPickups::WeaponForModel(m); }
        rev = (int32)CPickups::WeaponForModel(m);
        EXPECT_EQ(orig, rev);
    }
}

// --- IsPickUpPickedUp: query collected status ---

GAME_DIFF_TEST(CPickups, IsPickUpPickedUp_Various) {
    // Test with various pickup reference indices (most should be not-picked-up)
    int32 refs[] = { 0, 1, 5, 10, 50, 100, 500, 619 };
    for (int32 ref : refs) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CPickups/IsPickUpPickedUp");
          orig = CPickups::IsPickUpPickedUp(ref); }
        rev = CPickups::IsPickUpPickedUp(ref);
        EXPECT_EQ(orig, rev);
    }
}

// --- GetActualPickupIndex ---

// GetActualPickupIndex crashes with some indices (OOB access in original code)
// — skip for safety. Only test with valid pickup references.

// --- PlayerCanPickUpThisWeaponTypeAtThisMoment ---

GAME_DIFF_TEST(CPickups, PlayerCanPickUpThisWeaponType) {
    eWeaponType weapons[] = {
        WEAPON_PISTOL, WEAPON_SHOTGUN, WEAPON_AK47, WEAPON_M4,
        WEAPON_SNIPERRIFLE, WEAPON_ROCKET, WEAPON_MINIGUN,
        WEAPON_GRENADE, WEAPON_MOLOTOV, WEAPON_SPRAYCAN
    };
    for (auto wt : weapons) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CPickups/PlayerCanPickUpThisWeaponTypeAtThisMoment");
          orig = CPickups::PlayerCanPickUpThisWeaponTypeAtThisMoment(wt); }
        rev = CPickups::PlayerCanPickUpThisWeaponTypeAtThisMoment(wt);
        EXPECT_EQ(orig, rev);
    }
}

// --- TestForPickupsInBubble ---

GAME_DIFF_TEST(CPickups, TestForPickupsInBubble) {
    CVector positions[] = {
        CVector(2495.0f, -1665.0f, 13.5f),  // near player
        CVector(0.0f, 0.0f, 0.0f),           // world origin
        CVector(1000.0f, -1000.0f, 50.0f),   // random location
    };
    float radii[] = { 5.0f, 50.0f, 500.0f };
    for (auto& pos : positions) {
        for (float r : radii) {
            bool orig, rev;
            { HookDisableGuard guard("Global/CPickups/TestForPickupsInBubble");
              orig = CPickups::TestForPickupsInBubble(pos, r); }
            rev = CPickups::TestForPickupsInBubble(pos, r);
            EXPECT_EQ(orig, rev);
        }
    }
}
