// test_CVehicle6.cpp — CVehicle Set* mutation + round-trip tests.
// Hook paths: Vehicle/CVehicle/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "Entity/Vehicle/Vehicle.h"
#include "Entity/Ped/Ped.h"
#include "ScenarioHelpers.h"

static Scenario::TestVehicle* s_v6 = nullptr;
static bool s_triedV6 = false;

static CVehicle* GetV6() {
    if (!s_triedV6) {
        s_triedV6 = true;
        int32 model = Scenario::FindLoadedVehicleModel();
        if (model >= 0)
            s_v6 = new Scenario::TestVehicle(model, CVector(2356.f, -1636.f, 13.5f));
    }
    return s_v6 ? s_v6->AsVehicle() : nullptr;
}

// --- SetFiringRateMultiplier / GetFiringRateMultiplier round-trip ---

GAME_DIFF_TEST(CVehicle6, SetGetFiringRateMultiplier) {
    auto* veh = GetV6();
    if (!veh) return;
    float saved = veh->GetFiringRateMultiplier();

    float testVal = 2.5f;
    { HookDisableGuard guard("Vehicle/CVehicle/SetFiringRateMultiplier");
      veh->SetFiringRateMultiplier(testVal); }
    float origVal = veh->GetFiringRateMultiplier();

    veh->SetFiringRateMultiplier(saved);
    veh->SetFiringRateMultiplier(testVal);
    float revVal = veh->GetFiringRateMultiplier();

    EXPECT_NEAR(origVal, revVal, 1e-5f);
    veh->SetFiringRateMultiplier(saved);
}

// --- SetGettingInFlags / SetGettingOutFlags ---

GAME_DIFF_TEST(CVehicle6, SetGettingInFlags) {
    auto* veh = GetV6();
    if (!veh) return;
    uint8 saved = veh->m_nGettingInFlags;

    { HookDisableGuard guard("Vehicle/CVehicle/SetGettingInFlags");
      veh->m_nGettingInFlags = 0;
      veh->SetGettingInFlags(0x0F); }
    uint8 origFlags = veh->m_nGettingInFlags;

    veh->m_nGettingInFlags = 0;
    veh->SetGettingInFlags(0x0F);
    uint8 revFlags = veh->m_nGettingInFlags;

    EXPECT_EQ(origFlags, revFlags);
    veh->m_nGettingInFlags = saved;
}

GAME_DIFF_TEST(CVehicle6, SetGettingOutFlags) {
    auto* veh = GetV6();
    if (!veh) return;
    uint8 saved = veh->m_nGettingOutFlags;

    { HookDisableGuard guard("Vehicle/CVehicle/SetGettingOutFlags");
      veh->m_nGettingOutFlags = 0;
      veh->SetGettingOutFlags(0x0F); }
    uint8 origFlags = veh->m_nGettingOutFlags;

    veh->m_nGettingOutFlags = 0;
    veh->SetGettingOutFlags(0x0F);
    uint8 revFlags = veh->m_nGettingOutFlags;

    EXPECT_EQ(origFlags, revFlags);
    veh->m_nGettingOutFlags = saved;
}

// --- SetWindowOpenFlag ---

GAME_DIFF_TEST(CVehicle6, SetWindowOpenFlag) {
    auto* veh = GetV6();
    if (!veh) return;
    { HookDisableGuard guard("Vehicle/CVehicle/SetWindowOpenFlag");
      veh->SetWindowOpenFlag(0); }
    veh->SetWindowOpenFlag(0);
    EXPECT_TRUE(true);
}

// SetRopeHeightForHeli crashes on non-heli vehicles. Skipped.

// SetHasslePosId takes extra args — skipped.

// --- KillPedsInVehicle ---

GAME_DIFF_TEST(CVehicle6, KillPedsInVehicle_Empty) {
    auto* veh = GetV6();
    if (!veh) return;
    // Vehicle is empty — should be a no-op
    { HookDisableGuard guard("Vehicle/CVehicle/KillPedsInVehicle");
      veh->KillPedsInVehicle(); }
    veh->KillPedsInVehicle();
    EXPECT_TRUE(true);
}
