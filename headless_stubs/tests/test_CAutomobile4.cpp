// test_CAutomobile4.cpp — Differential tests for CAutomobile query functions.
// Hook paths: Vehicle/CAutomobile/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "Entity/Vehicle/Automobile.h"
#include "ScenarioHelpers.h"

static Scenario::TestVehicle* s_auto4 = nullptr;
static bool s_triedAuto4 = false;

static CAutomobile* GetAuto4() {
    if (!s_triedAuto4) {
        s_triedAuto4 = true;
        int32 model = Scenario::FindLoadedVehicleModel();
        if (model >= 0)
            s_auto4 = new Scenario::TestVehicle(model, CVector(2396.f, -1664.f, 13.5f));
    }
    if (!s_auto4) return nullptr;
    auto* veh = s_auto4->AsVehicle();
    // Only test if it's actually an automobile (not bike/boat/etc)
    if (veh && veh->IsAutomobile())
        return static_cast<CAutomobile*>(veh);
    return nullptr;
}

// --- GetCarPitch ---

GAME_DIFF_TEST(CAutomobile4, GetCarPitch) {
    auto* car = GetAuto4();
    if (!car) return;
    float orig, rev;
    { HookDisableGuard guard("Vehicle/CAutomobile/GetCarPitch");
      orig = car->GetCarPitch(); }
    rev = car->GetCarPitch();
    EXPECT_NEAR(orig, rev, 1e-5f);
}
