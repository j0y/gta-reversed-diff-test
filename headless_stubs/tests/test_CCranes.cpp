// test_CCranes.cpp — Differential tests for CCranes.
// Hook paths: Global/CCranes/<fn>
//
// All crane functions are NOP stubs or simple queries at state 9
// (no cranes active in headless). Tests verify reversed stubs match original.

#include "StdInc.h"
#include "TestFramework.h"
#include "Cranes.h"
#include "ScenarioHelpers.h"

// --- IsThisCarBeingCarriedByAnyCrane ---

static Scenario::TestVehicle* s_craneVeh = nullptr;
static bool s_triedCraneVeh = false;

static CVehicle* GetCraneTestVehicle() {
    if (!s_triedCraneVeh) {
        s_triedCraneVeh = true;
        int32 model = Scenario::FindLoadedVehicleModel();
        if (model >= 0)
            s_craneVeh = new Scenario::TestVehicle(model, CVector(2400.f, -1670.f, 13.5f));
    }
    return s_craneVeh ? s_craneVeh->AsVehicle() : nullptr;
}

GAME_DIFF_TEST(CCranes, IsThisCarBeingCarriedByAnyCrane) {
    auto* veh = GetCraneTestVehicle();
    if (!veh) return;
    bool orig, rev;
    { HookDisableGuard guard("Global/CCranes/IsThisCarBeingCarriedByAnyCrane");
      orig = CCranes::IsThisCarBeingCarriedByAnyCrane(veh); }
    rev = CCranes::IsThisCarBeingCarriedByAnyCrane(veh);
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CCranes, IsThisCarBeingTargettedByAnyCrane) {
    auto* veh = GetCraneTestVehicle();
    if (!veh) return;
    bool orig, rev;
    { HookDisableGuard guard("Global/CCranes/IsThisCarBeingTargettedByAnyCrane");
      orig = CCranes::IsThisCarBeingTargettedByAnyCrane(veh); }
    rev = CCranes::IsThisCarBeingTargettedByAnyCrane(veh);
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CCranes, IsThisCarPickedUp) {
    auto* veh = GetCraneTestVehicle();
    if (!veh) return;
    CVector pos = veh->GetPosition();
    bool orig, rev;
    { HookDisableGuard guard("Global/CCranes/IsThisCarPickedUp");
      orig = CCranes::IsThisCarPickedUp(pos.x, pos.y, veh); }
    rev = CCranes::IsThisCarPickedUp(pos.x, pos.y, veh);
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CCranes, IsThisCarBeingCarriedByAnyCrane_Null) {
    bool orig, rev;
    { HookDisableGuard guard("Global/CCranes/IsThisCarBeingCarriedByAnyCrane");
      orig = CCranes::IsThisCarBeingCarriedByAnyCrane(nullptr); }
    rev = CCranes::IsThisCarBeingCarriedByAnyCrane(nullptr);
    EXPECT_EQ(orig, rev);
}
