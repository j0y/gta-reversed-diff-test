// test_CEntity4.cpp — CEntity mutation tests.
// Hook paths: Entity/CEntity/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "Entity/Entity.h"
#include "Entity/Vehicle/Vehicle.h"
#include "ScenarioHelpers.h"

static Scenario::TestVehicle* s_e4Veh = nullptr;
static bool s_triedE4Veh = false;

static CVehicle* GetE4Veh() {
    if (!s_triedE4Veh) {
        s_triedE4Veh = true;
        int32 model = Scenario::FindLoadedVehicleModel();
        if (model >= 0)
            s_e4Veh = new Scenario::TestVehicle(model, CVector(2372.f, -1644.f, 13.5f));
    }
    return s_e4Veh ? s_e4Veh->AsVehicle() : nullptr;
}

// --- SetIsStatic round-trip ---

GAME_DIFF_TEST(CEntity4, SetIsStatic_RoundTrip) {
    auto* veh = GetE4Veh();
    if (!veh) return;

    bool saved = veh->GetIsStatic();

    { HookDisableGuard guard("Entity/CEntity/SetIsStatic");
      veh->SetIsStatic(true); }
    bool origTrue = veh->GetIsStatic();

    veh->SetIsStatic(false);
    veh->SetIsStatic(true);
    bool revTrue = veh->GetIsStatic();

    EXPECT_EQ(origTrue, revTrue);

    { HookDisableGuard guard("Entity/CEntity/SetIsStatic");
      veh->SetIsStatic(false); }
    bool origFalse = veh->GetIsStatic();

    veh->SetIsStatic(true);
    veh->SetIsStatic(false);
    bool revFalse = veh->GetIsStatic();

    EXPECT_EQ(origFalse, revFalse);

    veh->SetIsStatic(saved);
}

// --- GetBoundRect on vehicle (returns CRect — 16 bytes, potential sret) ---

GAME_DIFF_TEST(CEntity4, GetBoundRect_Vehicle) {
    auto* veh = GetE4Veh();
    if (!veh) return;
    CRect orig, rev;
    { HookDisableGuard guard("Entity/CEntity/GetBoundRect");
      orig = veh->GetBoundRect(); }
    rev = veh->GetBoundRect();
    EXPECT_NEAR(orig.left, rev.left, 1e-1f);
    EXPECT_NEAR(orig.bottom, rev.bottom, 1e-1f);
    EXPECT_NEAR(orig.right, rev.right, 1e-1f);
    EXPECT_NEAR(orig.top, rev.top, 1e-1f);
}
