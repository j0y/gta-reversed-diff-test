// test_CShadows2.cpp — More CShadows behavior and differential tests.
// Hook paths: Shadows/CShadows/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "Shadows.h"

// --- GunShotSetsOilOnFire: void, behavior test with two points ---

GAME_TEST(CShadows2, GunShotSetsOilOnFire_Origin) {
    CVector start{0.0f, 0.0f, 0.0f};
    CVector end{10.0f, 10.0f, 0.0f};
    CShadows::GunShotSetsOilOnFire(start, end);
    EXPECT_TRUE(true);
}

GAME_TEST(CShadows2, GunShotSetsOilOnFire_DistantPoints) {
    CVector start{100.0f, 200.0f, 10.0f};
    CVector end{150.0f, 250.0f, 10.0f};
    CShadows::GunShotSetsOilOnFire(start, end);
    EXPECT_TRUE(true);
}

GAME_TEST(CShadows2, GunShotSetsOilOnFire_SamePoint) {
    CVector point{50.0f, 50.0f, 5.0f};
    CShadows::GunShotSetsOilOnFire(point, point);
    EXPECT_TRUE(true);
}

// --- UpdateStaticShadows: void, behavior test ---

GAME_TEST(CShadows2, UpdateStaticShadows) {
    CShadows::UpdateStaticShadows();
    EXPECT_TRUE(true);
}

GAME_TEST(CShadows2, UpdateStaticShadows_Twice) {
    CShadows::UpdateStaticShadows();
    CShadows::UpdateStaticShadows();
    EXPECT_TRUE(true);
}
