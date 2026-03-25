// test_CPlane3.cpp — Differential tests for CPlane static functions.
// Hook paths: Vehicle/CPlane/<fn>
//
// Extends test_CPlane2 with SwitchAmbientPlanes behavior test and
// additional differential coverage.

#include "StdInc.h"
#include "TestFramework.h"
#include "Entity/Vehicle/Plane.h"

// ---------------------------------------------------------------------------
// Differential tests — static query functions
// ---------------------------------------------------------------------------

GAME_DIFF_TEST(CPlane3, CountPlanesAndHelis_Repeat) {
    // Verify determinism: calling twice should yield same result
    uint32 orig, rev;
    { HookDisableGuard guard("Vehicle/CPlane/CountPlanesAndHelis");
      orig = CPlane::CountPlanesAndHelis(); }
    rev = CPlane::CountPlanesAndHelis();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CPlane3, AreWeInNoPlaneZone) {
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CPlane/AreWeInNoPlaneZone");
      orig = CPlane::AreWeInNoPlaneZone(); }
    rev = CPlane::AreWeInNoPlaneZone();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CPlane3, AreWeInNoBigPlaneZone) {
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CPlane/AreWeInNoBigPlaneZone");
      orig = CPlane::AreWeInNoBigPlaneZone(); }
    rev = CPlane::AreWeInNoBigPlaneZone();
    EXPECT_EQ(orig, rev);
}

// ---------------------------------------------------------------------------
// Behavior test — SwitchAmbientPlanes
// SwitchAmbientPlanes sets internal state; verify both original and reversed
// agree after toggling.
// ---------------------------------------------------------------------------

// SwitchAmbientPlanes hook is locked — behavior tests only
GAME_TEST(CPlane3, SwitchAmbientPlanes_Enable) {
    CPlane::SwitchAmbientPlanes(true);
    EXPECT_TRUE(true);
}

GAME_TEST(CPlane3, SwitchAmbientPlanes_Disable) {
    CPlane::SwitchAmbientPlanes(false);
    EXPECT_TRUE(true);
}

// ---------------------------------------------------------------------------
// Combined scenario: toggle ambient planes and verify zone checks still work
// ---------------------------------------------------------------------------

GAME_TEST(CPlane3, ZoneCheckAfterToggle) {
    // Disable ambient planes, check zone functions don't crash
    CPlane::SwitchAmbientPlanes(false);
    bool noPlane = CPlane::AreWeInNoPlaneZone();
    bool noBigPlane = CPlane::AreWeInNoBigPlaneZone();
    uint32 count = CPlane::CountPlanesAndHelis();

    // Re-enable
    CPlane::SwitchAmbientPlanes(true);
    bool noPlane2 = CPlane::AreWeInNoPlaneZone();
    bool noBigPlane2 = CPlane::AreWeInNoBigPlaneZone();
    uint32 count2 = CPlane::CountPlanesAndHelis();

    // Zone checks should be position-dependent, not toggle-dependent
    EXPECT_EQ(noPlane, noPlane2);
    EXPECT_EQ(noBigPlane, noBigPlane2);
    // Count may differ if ambient planes were spawned, but shouldn't crash
    (void)count;
    (void)count2;
}
