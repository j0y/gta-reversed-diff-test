// test_CColStore2.cpp — Differential tests for CColStore.
// Hook paths: Collision/CColStore/<fn>
//
// Tests collision store queries. CColStore uses category "Collision".

#include "StdInc.h"
#include "TestFramework.h"
#include "Collision/ColStore.h"

// --- HasCollisionLoaded at various positions ---

GAME_DIFF_TEST(CColStore, HasCollisionLoaded_MultiplePositions) {
    struct TestCase { CVector pos; int32 area; };
    TestCase cases[] = {
        { {2495.f, -1665.f, 13.f}, 0 },   // Grove Street
        { {0.f, 0.f, 0.f}, 0 },            // World origin
        { {-2170.f, 250.f, 30.f}, 0 },     // San Fierro
        { {2027.f, 1008.f, 20.f}, 0 },     // Las Venturas
        { {1000.f, -1000.f, 10.f}, 0 },    // Random LS
    };
    for (auto& tc : cases) {
        bool orig, rev;
        { HookDisableGuard guard("Collision/CColStore/HasCollisionLoaded");
          orig = CColStore::HasCollisionLoaded(tc.pos, tc.area); }
        rev = CColStore::HasCollisionLoaded(tc.pos, tc.area);
        EXPECT_EQ(orig, rev);
    }
}

// --- GetBoundingBox ---

GAME_DIFF_TEST(CColStore, GetBoundingBox_FirstSlots) {
    for (int32 slot = 0; slot < 10; slot++) {
        CRect* orig;
        CRect* rev;
        { HookDisableGuard guard("Collision/CColStore/GetBoundingBox");
          orig = CColStore::GetBoundingBox(slot); }
        rev = CColStore::GetBoundingBox(slot);
        EXPECT_EQ(orig, rev);
    }
}
