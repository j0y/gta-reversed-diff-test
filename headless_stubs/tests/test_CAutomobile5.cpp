// test_CAutomobile5.cpp — CAutomobile query functions on spawned vehicle.
// Hook paths: Vehicle/CAutomobile/<fn>
// Note: Door functions (IsDoorReady, IsDoorFullyOpen, IsDoorClosed) crash on
// spawned vehicles due to uninitialized door state — tested via U32 variants instead.

#include "StdInc.h"
#include "TestFramework.h"
#include "Entity/Vehicle/Automobile.h"
#include "ScenarioHelpers.h"

static Scenario::TestVehicle* s_a5 = nullptr;
static bool s_triedA5 = false;

static CAutomobile* GetA5() {
    if (!s_triedA5) {
        s_triedA5 = true;
        int32 model = Scenario::FindLoadedVehicleModel();
        if (model >= 0)
            s_a5 = new Scenario::TestVehicle(model, CVector(2368.f, -1642.f, 13.5f));
    }
    if (!s_a5) return nullptr;
    auto* veh = s_a5->AsVehicle();
    if (veh && veh->IsAutomobile()) return static_cast<CAutomobile*>(veh);
    return nullptr;
}

GAME_DIFF_TEST(CAutomobile5, GetAllWheelsOffGround) {
    auto* car = GetA5();
    if (!car) return;
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CAutomobile/GetAllWheelsOffGround");
      orig = car->GetAllWheelsOffGround(); }
    rev = car->GetAllWheelsOffGround();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CAutomobile5, GetCarRoll) {
    auto* car = GetA5();
    if (!car) return;
    float orig, rev;
    { HookDisableGuard guard("Vehicle/CAutomobile/GetCarRoll");
      orig = car->GetCarRoll(); }
    rev = car->GetCarRoll();
    EXPECT_NEAR(orig, rev, 1e-5f);
}

GAME_DIFF_TEST(CAutomobile5, GetNumContactWheels) {
    auto* car = GetA5();
    if (!car) return;
    int32 orig, rev;
    { HookDisableGuard guard("Vehicle/CAutomobile/GetNumContactWheels");
      orig = car->GetNumContactWheels(); }
    rev = car->GetNumContactWheels();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CAutomobile5, IsOpenTopCar) {
    auto* car = GetA5();
    if (!car) return;
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CAutomobile/IsOpenTopCar");
      orig = car->IsOpenTopCar(); }
    rev = car->IsOpenTopCar();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CAutomobile5, FindWheelWidth) {
    auto* car = GetA5();
    if (!car) return;
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CAutomobile/FindWheelWidth");
      orig = car->FindWheelWidth(false); }
    rev = car->FindWheelWidth(false);
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CAutomobile5, IsRoomForPedToLeaveCar) {
    auto* car = GetA5();
    if (!car) return;
    // Test all 4 doors
    for (uint32 door = 0; door < 4; door++) {
        bool orig, rev;
        { HookDisableGuard guard("Vehicle/CAutomobile/IsRoomForPedToLeaveCar");
          orig = car->IsRoomForPedToLeaveCar(door, nullptr); }
        rev = car->IsRoomForPedToLeaveCar(door, nullptr);
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(CAutomobile5, IsComponentPresent) {
    auto* car = GetA5();
    if (!car) return;
    for (int32 comp = 0; comp < 20; comp++) {
        bool orig, rev;
        { HookDisableGuard guard("Vehicle/CAutomobile/IsComponentPresent");
          orig = car->IsComponentPresent(comp); }
        rev = car->IsComponentPresent(comp);
        EXPECT_EQ(orig, rev);
    }
}

// --- Door queries via U32 variants (safe — take uint32 index, don't access door objects) ---

GAME_DIFF_TEST(CAutomobile5, IsDoorMissingU32_AllDoors) {
    auto* car = GetA5();
    if (!car) return;
    for (uint32 door = 0; door < 6; door++) {
        bool orig, rev;
        { HookDisableGuard guard("Vehicle/CAutomobile/IsDoorMissingU32");
          orig = car->IsDoorMissingU32(door); }
        rev = car->IsDoorMissingU32(door);
        EXPECT_EQ(orig, rev);
    }
}
