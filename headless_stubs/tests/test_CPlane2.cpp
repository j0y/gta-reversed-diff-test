// test_CPlane2.cpp — Differential tests for CPlane static functions.
// Hook paths: Vehicle/CPlane/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "Entity/Vehicle/Plane.h"

GAME_DIFF_TEST(CPlane2, CountPlanesAndHelis) {
    int32 orig, rev;
    { HookDisableGuard guard("Vehicle/CPlane/CountPlanesAndHelis");
      orig = CPlane::CountPlanesAndHelis(); }
    rev = CPlane::CountPlanesAndHelis();
    EXPECT_EQ(orig, rev);
}

// AreWeInNoPlaneZone / AreWeInNoBigPlaneZone take no args (check player pos internally)

GAME_DIFF_TEST(CPlane2, AreWeInNoPlaneZone) {
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CPlane/AreWeInNoPlaneZone");
      orig = CPlane::AreWeInNoPlaneZone(); }
    rev = CPlane::AreWeInNoPlaneZone();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CPlane2, AreWeInNoBigPlaneZone) {
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CPlane/AreWeInNoBigPlaneZone");
      orig = CPlane::AreWeInNoBigPlaneZone(); }
    rev = CPlane::AreWeInNoBigPlaneZone();
    EXPECT_EQ(orig, rev);
}
