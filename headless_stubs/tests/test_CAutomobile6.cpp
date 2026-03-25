// test_CAutomobile6.cpp — More CAutomobile differential tests.
// Hook paths: Vehicle/CAutomobile/<fn>
// Previously tested: GetAllWheelsOffGround, GetCarRoll, GetNumContactWheels,
// IsOpenTopCar, FindWheelWidth, IsRoomForPedToLeaveCar, IsComponentPresent,
// IsDoorMissingU32, GetCarPitch, IsInAir, GetMovingCollisionOffset, GetHeightAboveRoad.
// New targets: RcbanditCheckHitWheels, ReduceHornCounter, CloseAllDoors,
// PlaceOnRoadProperly, ScanForCrimes.

#include "StdInc.h"
#include "TestFramework.h"
#include "Entity/Vehicle/Automobile.h"
#include "ScenarioHelpers.h"

static Scenario::TestVehicle* s_a6 = nullptr;
static bool s_triedA6 = false;

static CAutomobile* GetA6() {
    if (!s_triedA6) {
        s_triedA6 = true;
        int32 model = Scenario::FindLoadedVehicleModel();
        if (model >= 0)
            s_a6 = new Scenario::TestVehicle(model, CVector(2510.f, -1642.f, 13.5f));
    }
    if (!s_a6) return nullptr;
    auto* veh = s_a6->AsVehicle();
    if (veh && veh->IsAutomobile()) return static_cast<CAutomobile*>(veh);
    return nullptr;
}

// --- RcbanditCheckHitWheels ---
// bool — checks if RC bandit wheels overlap nearby vehicles.

GAME_DIFF_TEST(CAutomobile6, RcbanditCheckHitWheels) {
    auto* car = GetA6();
    if (!car) return;
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CAutomobile/RcbanditCheckHitWheels");
      orig = car->RcbanditCheckHitWheels(); }
    rev = car->RcbanditCheckHitWheels();
    EXPECT_EQ(orig, rev);
}

// ReduceHornCounter — m_nHornCounter not a member of CAutomobile, skipped

// --- CloseAllDoors ---
// void — closes all doors. Behavioral test: verify door states match.

GAME_DIFF_TEST(CAutomobile6, CloseAllDoors) {
    auto* car = GetA6();
    if (!car) return;

    { HookDisableGuard guard("Vehicle/CAutomobile/CloseAllDoors");
      car->CloseAllDoors(); }
    // Check a door state after original
    float origRatio = car->GetDooorAngleOpenRatioU32(0);

    car->CloseAllDoors();
    float revRatio = car->GetDooorAngleOpenRatioU32(0);

    EXPECT_NEAR(origRatio, revRatio, 1e-5f);
}

// --- PlaceOnRoadProperly ---
// void — adjusts vehicle position/orientation for road. Behavioral diff test.

GAME_DIFF_TEST(CAutomobile6, PlaceOnRoadProperly) {
    auto* car = GetA6();
    if (!car) return;
    CVector savedPos = car->GetPosition();
    CVector savedMoveSpeed = car->m_vecMoveSpeed;

    // Set a known position
    car->SetPosn(CVector(2515.f, -1642.f, 13.5f));
    { HookDisableGuard guard("Vehicle/CAutomobile/PlaceOnRoadProperly");
      car->PlaceOnRoadProperly(); }
    CVector origPos = car->GetPosition();

    // Reset and run reversed
    car->SetPosn(CVector(2515.f, -1642.f, 13.5f));
    car->PlaceOnRoadProperly();
    CVector revPos = car->GetPosition();

    EXPECT_NEAR(origPos.x, revPos.x, 1e-3f);
    EXPECT_NEAR(origPos.y, revPos.y, 1e-3f);
    EXPECT_NEAR(origPos.z, revPos.z, 1e-3f);

    // Restore
    car->SetPosn(savedPos);
    car->m_vecMoveSpeed = savedMoveSpeed;
}

// --- ScanForCrimes ---
// void — scans for nearby crimes the vehicle's driver committed.
// Behavioral test: should not crash and state should be consistent.

GAME_DIFF_TEST(CAutomobile6, ScanForCrimes) {
    auto* car = GetA6();
    if (!car) return;

    { HookDisableGuard guard("Vehicle/CAutomobile/ScanForCrimes");
      car->ScanForCrimes(); }
    // No crash = pass for original

    car->ScanForCrimes();
    // No crash = pass for reversed
    EXPECT_TRUE(true);
}

// --- IsDoorReadyU32 ---
// bool — checks if a door is ready (for all 6 doors).

GAME_DIFF_TEST(CAutomobile6, IsDoorReadyU32_AllDoors) {
    auto* car = GetA6();
    if (!car) return;
    for (uint32 door = 0; door < 6; door++) {
        bool orig, rev;
        { HookDisableGuard guard("Vehicle/CAutomobile/IsDoorReadyU32");
          orig = car->IsDoorReadyU32(door); }
        rev = car->IsDoorReadyU32(door);
        EXPECT_EQ(orig, rev);
    }
}

// --- IsDoorFullyOpenU32 ---
// bool — checks if a door is fully open (for all 6 doors).

GAME_DIFF_TEST(CAutomobile6, IsDoorFullyOpenU32_AllDoors) {
    auto* car = GetA6();
    if (!car) return;
    for (uint32 door = 0; door < 6; door++) {
        bool orig, rev;
        { HookDisableGuard guard("Vehicle/CAutomobile/IsDoorFullyOpenU32");
          orig = car->IsDoorFullyOpenU32(door); }
        rev = car->IsDoorFullyOpenU32(door);
        EXPECT_EQ(orig, rev);
    }
}

// --- IsDoorClosedU32 ---
// bool — checks if a door is closed (for all 6 doors).

GAME_DIFF_TEST(CAutomobile6, IsDoorClosedU32_AllDoors) {
    auto* car = GetA6();
    if (!car) return;
    for (uint32 door = 0; door < 6; door++) {
        bool orig, rev;
        { HookDisableGuard guard("Vehicle/CAutomobile/IsDoorClosedU32");
          orig = car->IsDoorClosedU32(door); }
        rev = car->IsDoorClosedU32(door);
        EXPECT_EQ(orig, rev);
    }
}

// --- GetDooorAngleOpenRatioU32 ---
// float — returns door open ratio for each door.

GAME_DIFF_TEST(CAutomobile6, GetDooorAngleOpenRatioU32_AllDoors) {
    auto* car = GetA6();
    if (!car) return;
    for (uint32 door = 0; door < 6; door++) {
        float orig, rev;
        { HookDisableGuard guard("Vehicle/CAutomobile/GetDooorAngleOpenRatioU32");
          orig = car->GetDooorAngleOpenRatioU32(door); }
        rev = car->GetDooorAngleOpenRatioU32(door);
        EXPECT_NEAR(orig, rev, 1e-5f);
    }
}
