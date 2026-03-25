// test_CPickups4.cpp — More differential tests for CPickups.
// Hook paths: Global/CPickups/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "Pickups.h"

// --- TestForPickupsInBubble (0x456450) ---

GAME_DIFF_TEST(CPickups, TestForPickupsInBubble_Extended) {
    struct TestCase { CVector pos; float radius; };
    TestCase cases[] = {
        { CVector(2495.0f, -1665.0f, 13.5f), 10.0f },   // near default player pos
        { CVector(2495.0f, -1665.0f, 13.5f), 100.0f },   // wider radius
        { CVector(-2000.0f, 2000.0f, 50.0f), 5.0f },     // empty area
        { CVector(0.0f, 0.0f, 0.0f), 1.0f },             // world origin, small radius
        { CVector(0.0f, 0.0f, 0.0f), 10000.0f },         // world origin, huge radius
    };
    for (auto& tc : cases) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CPickups/TestForPickupsInBubble");
          orig = CPickups::TestForPickupsInBubble(tc.pos, tc.radius); }
        rev = CPickups::TestForPickupsInBubble(tc.pos, tc.radius);
        EXPECT_EQ(orig, rev);
    }
}

// --- WeaponForModel (0x454AE0) ---

GAME_DIFF_TEST(CPickups, WeaponForModel_Extended) {
    // Known weapon model IDs + some invalid ones
    int32 models[] = {
        331, 333, 334, 335, 336, 337, 338, 339, 341, 342,
        346, 347, 348, 349, 350, 351, 352, 353, 355, 356,
        357, 358, 359, 361, 362, 363, 364, 365, 366, 367,
        368, 369, 371, 372,
        0  // invalid model (skip extreme values that crash original)
    };
    for (int32 m : models) {
        int32 orig, rev;
        { HookDisableGuard guard("Global/CPickups/WeaponForModel");
          orig = (int32)CPickups::WeaponForModel(m); }
        rev = (int32)CPickups::WeaponForModel(m);
        EXPECT_EQ(orig, rev);
    }
}

// --- GivePlayerGoodiesWithPickUpMI (0x4564F0) ---

GAME_DIFF_TEST(CPickups, GivePlayerGoodiesWithPickUpMI_Basic) {
    struct TestCase { uint16 modelId; int32 playerId; };
    TestCase cases[] = {
        { 0, 0 },
        { 1240, 0 },   // health pickup model
        { 1242, 0 },   // armour pickup model
        { 1247, 0 },   // adrenaline
    };
    for (auto& tc : cases) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CPickups/GivePlayerGoodiesWithPickUpMI");
          orig = CPickups::GivePlayerGoodiesWithPickUpMI(tc.modelId, tc.playerId); }
        rev = CPickups::GivePlayerGoodiesWithPickUpMI(tc.modelId, tc.playerId);
        EXPECT_EQ(orig, rev);
    }
}

// --- FindPickUpForThisObject (0x4551C0) ---

GAME_DIFF_TEST(CPickups, FindPickUpForThisObject_Nullptr) {
    CPickup* orig;
    CPickup* rev;
    { HookDisableGuard guard("Global/CPickups/FindPickUpForThisObject");
      orig = CPickups::FindPickUpForThisObject(nullptr); }
    rev = CPickups::FindPickUpForThisObject(nullptr);
    EXPECT_EQ(orig, rev);
}
