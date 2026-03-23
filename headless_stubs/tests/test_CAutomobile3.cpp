// test_CAutomobile3.cpp — Deep differential tests for CAutomobile.
// Hook paths: Vehicle/CAutomobile/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "Entity/Vehicle/Automobile.h"
#include "ScenarioHelpers.h"

static Scenario::TestVehicle* s_autoVeh3 = nullptr;
static bool s_triedAutoVeh3 = false;

static CAutomobile* GetAutoTestVehicle3() {
    if (!s_triedAutoVeh3) {
        s_triedAutoVeh3 = true;
        int32 model = Scenario::FindLoadedVehicleModel();
        if (model >= 0)
            s_autoVeh3 = new Scenario::TestVehicle(model, CVector(2414.0f, -1668.0f, 13.5f));
    }
    return s_autoVeh3 ? s_autoVeh3->ptr : nullptr;
}

#define AUTO3_GUARD() if (!GetAutoTestVehicle3()) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No automobile"); return; }

// --- GetAllWheelsOffGround ---

GAME_DIFF_TEST(CAutomobile3, GetAllWheelsOffGround) {
    AUTO3_GUARD()
    auto* car = GetAutoTestVehicle3();
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CAutomobile/GetAllWheelsOffGround"); orig = car->GetAllWheelsOffGround(); }
    rev = car->GetAllWheelsOffGround();
    EXPECT_EQ(orig, rev);
}

// --- GetCarRoll ---

GAME_DIFF_TEST(CAutomobile3, GetCarRoll) {
    AUTO3_GUARD()
    auto* car = GetAutoTestVehicle3();
    float orig, rev;
    { HookDisableGuard guard("Vehicle/CAutomobile/GetCarRoll"); orig = car->GetCarRoll(); }
    rev = car->GetCarRoll();
    EXPECT_NEAR(orig, rev, 1e-3f);
}

// --- GetCarPitch ---

GAME_DIFF_TEST(CAutomobile3, GetCarPitch) {
    AUTO3_GUARD()
    auto* car = GetAutoTestVehicle3();
    float orig, rev;
    { HookDisableGuard guard("Vehicle/CAutomobile/GetCarPitch"); orig = car->GetCarPitch(); }
    rev = car->GetCarPitch();
    EXPECT_NEAR(orig, rev, 1e-3f);
}

// --- IsInAir ---

GAME_DIFF_TEST(CAutomobile3, IsInAir) {
    AUTO3_GUARD()
    auto* car = GetAutoTestVehicle3();
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CAutomobile/IsInAir"); orig = car->IsInAir(); }
    rev = car->IsInAir();
    EXPECT_EQ(orig, rev);
}

// --- GetMovingCollisionOffset ---

GAME_DIFF_TEST(CAutomobile3, GetMovingCollisionOffset) {
    AUTO3_GUARD()
    auto* car = GetAutoTestVehicle3();
    float orig, rev;
    { HookDisableGuard guard("Vehicle/CAutomobile/GetMovingCollisionOffset"); orig = car->GetMovingCollisionOffset(); }
    rev = car->GetMovingCollisionOffset();
    EXPECT_NEAR(orig, rev, 1e-3f);
}
