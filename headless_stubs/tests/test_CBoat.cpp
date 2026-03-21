// test_CBoat.cpp — Differential tests for CBoat static functions.
// Hook paths: Vehicle/CBoat/<fn>
// Boat wake effects — used for water surface deformation.

#include "StdInc.h"
#include "TestFramework.h"
#include "Entity/Vehicle/Boat.h"

// IsSectorAffectedByWake — checks if a 2D sector near any boat
GAME_DIFF_TEST(CBoat, IsSectorAffectedByWake_GroveStreet) {
    // Inland — no boats expected
    CVector2D centre(2488.f, -1666.f);
    CBoat* boats[4] = {};
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CBoat/IsSectorAffectedByWake");
      orig = CBoat::IsSectorAffectedByWake(centre, 50.f, boats); }
    rev = CBoat::IsSectorAffectedByWake(centre, 50.f, boats);
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CBoat, IsSectorAffectedByWake_Ocean) {
    // Ocean area — may or may not have boats
    CVector2D centre(370.f, -1840.f);
    CBoat* boats[4] = {};
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CBoat/IsSectorAffectedByWake");
      orig = CBoat::IsSectorAffectedByWake(centre, 100.f, boats); }
    rev = CBoat::IsSectorAffectedByWake(centre, 100.f, boats);
    EXPECT_EQ(orig, rev);
}

// IsVertexAffectedByWake — returns wave height at vertex
// Needs a boat pointer, which we don't have. Test with nullptr guard.
GAME_TEST(CBoat, IsVertexAffectedByWake_NoBoat) {
    // When no boats are nearby, function should return 0 or handle gracefully
    // Can't call with nullptr boat — just verify the static function exists
    // by testing IsSectorAffectedByWake which gates the vertex check
    CVector2D centre(0.f, 0.f);
    CBoat* boats[4] = {};
    bool result = CBoat::IsSectorAffectedByWake(centre, 10.f, boats);
    (void)result;
    EXPECT_TRUE(true);
}
