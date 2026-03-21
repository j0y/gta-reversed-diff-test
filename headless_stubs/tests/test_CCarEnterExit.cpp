// test_CCarEnterExit.cpp — Differential tests for CCarEnterExit static queries.
// Hook paths: Global/CCarEnterExit/<fn>
// Uses spawned vehicle + player ped for door/entry queries.
//
// Inspired by gta-reversed issues:
//   #974 — CTaskComplexShuffleSeats bugs (door/seat computations)
//   #828 — Error entering police car with officer inside
//   #896 — Ride on bike bug (door availability checks)

#include "StdInc.h"
#include "TestFramework.h"
#include "ScenarioHelpers.h"
#include "CarEnterExit.h"

static Scenario::TestVehicle* s_ceVeh = nullptr;
static bool s_triedCeVeh = false;

static CVehicle* GetCEVehicle() {
    if (!s_triedCeVeh) {
        s_triedCeVeh = true;
        int32 model = Scenario::FindLoadedVehicleModel();
        if (model >= 0) {
            s_ceVeh = new Scenario::TestVehicle(model, CVector(2508.0f, -1665.0f, 13.5f));
        }
    }
    return s_ceVeh ? s_ceVeh->AsVehicle() : nullptr;
}

// --- Health checks ---

GAME_DIFF_TEST(CCarEnterExit, IsPedHealthy_Player) {
    auto* ped = FindPlayerPed(0);
    if (!ped) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No player ped"); return; }
    bool orig, rev;
    { HookDisableGuard guard("Global/CCarEnterExit/IsPedHealthy"); orig = CCarEnterExit::IsPedHealthy(ped); }
    rev = CCarEnterExit::IsPedHealthy(ped);
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CCarEnterExit, IsVehicleHealthy) {
    auto* veh = GetCEVehicle();
    if (!veh) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No vehicle"); return; }
    bool orig, rev;
    { HookDisableGuard guard("Global/CCarEnterExit/IsVehicleHealthy"); orig = CCarEnterExit::IsVehicleHealthy(veh); }
    rev = CCarEnterExit::IsVehicleHealthy(veh);
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CCarEnterExit, IsVehicleStealable) {
    auto* veh = GetCEVehicle();
    auto* ped = FindPlayerPed(0);
    if (!veh || !ped) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No vehicle or ped"); return; }
    bool orig, rev;
    { HookDisableGuard guard("Global/CCarEnterExit/IsVehicleStealable"); orig = CCarEnterExit::IsVehicleStealable(veh, ped); }
    rev = CCarEnterExit::IsVehicleStealable(veh, ped);
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CCarEnterExit, IsClearToDriveAway) {
    auto* veh = GetCEVehicle();
    if (!veh) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No vehicle"); return; }
    bool orig, rev;
    { HookDisableGuard guard("Global/CCarEnterExit/IsClearToDriveAway"); orig = CCarEnterExit::IsClearToDriveAway(veh); }
    rev = CCarEnterExit::IsClearToDriveAway(veh);
    EXPECT_EQ(orig, rev);
}

// --- Door queries ---

GAME_DIFF_TEST(CCarEnterExit, CarHasDoorToClose_AllDoors) {
    auto* veh = GetCEVehicle();
    if (!veh) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No vehicle"); return; }
    for (int32 door = 0; door < 6; door++) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CCarEnterExit/CarHasDoorToClose"); orig = CCarEnterExit::CarHasDoorToClose(veh, door); }
        rev = CCarEnterExit::CarHasDoorToClose(veh, door);
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(CCarEnterExit, CarHasDoorToOpen_AllDoors) {
    auto* veh = GetCEVehicle();
    if (!veh) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No vehicle"); return; }
    for (int32 door = 0; door < 6; door++) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CCarEnterExit/CarHasDoorToOpen"); orig = CCarEnterExit::CarHasDoorToOpen(veh, door); }
        rev = CCarEnterExit::CarHasDoorToOpen(veh, door);
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(CCarEnterExit, CarHasPartiallyOpenDoor_AllDoors) {
    auto* veh = GetCEVehicle();
    if (!veh) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No vehicle"); return; }
    for (int32 door = 0; door < 6; door++) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CCarEnterExit/CarHasPartiallyOpenDoor"); orig = CCarEnterExit::CarHasPartiallyOpenDoor(veh, door); }
        rev = CCarEnterExit::CarHasPartiallyOpenDoor(veh, door);
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(CCarEnterExit, IsCarDoorReady_AllDoors) {
    auto* veh = GetCEVehicle();
    if (!veh) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No vehicle"); return; }
    for (int32 door = 0; door < 6; door++) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CCarEnterExit/IsCarDoorReady"); orig = CCarEnterExit::IsCarDoorReady(veh, door); }
        rev = CCarEnterExit::IsCarDoorReady(veh, door);
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(CCarEnterExit, IsCarSlowJackRequired_AllDoors) {
    auto* veh = GetCEVehicle();
    if (!veh) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No vehicle"); return; }
    for (int32 door = 0; door < 6; door++) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CCarEnterExit/IsCarSlowJackRequired"); orig = CCarEnterExit::IsCarSlowJackRequired(veh, door); }
        rev = CCarEnterExit::IsCarSlowJackRequired(veh, door);
        EXPECT_EQ(orig, rev);
    }
}

// --- Seat/door computation ---

GAME_DIFF_TEST(CCarEnterExit, ComputePassengerIndexFromCarDoor) {
    auto* veh = GetCEVehicle();
    if (!veh) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No vehicle"); return; }
    // Door IDs: 8=RF, 9=RR, 10=LF, 11=LR
    int32 doors[] = { 8, 9, 10, 11 };
    for (int32 door : doors) {
        int32 orig, rev;
        { HookDisableGuard guard("Global/CCarEnterExit/ComputePassengerIndexFromCarDoor"); orig = CCarEnterExit::ComputePassengerIndexFromCarDoor(veh, door); }
        rev = CCarEnterExit::ComputePassengerIndexFromCarDoor(veh, door);
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(CCarEnterExit, ComputeTargetDoorToEnterAsPassenger) {
    auto* veh = GetCEVehicle();
    if (!veh) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No vehicle"); return; }
    for (int32 seat = 0; seat < 4; seat++) {
        int32 orig, rev;
        { HookDisableGuard guard("Global/CCarEnterExit/ComputeTargetDoorToEnterAsPassenger"); orig = CCarEnterExit::ComputeTargetDoorToEnterAsPassenger(veh, seat); }
        rev = CCarEnterExit::ComputeTargetDoorToEnterAsPassenger(veh, seat);
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(CCarEnterExit, ComputeTargetDoorToExit) {
    auto* veh = GetCEVehicle();
    auto* ped = FindPlayerPed(0);
    if (!veh || !ped) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No vehicle or ped"); return; }
    int32 orig, rev;
    { HookDisableGuard guard("Global/CCarEnterExit/ComputeTargetDoorToExit"); orig = CCarEnterExit::ComputeTargetDoorToExit(veh, ped); }
    rev = CCarEnterExit::ComputeTargetDoorToExit(veh, ped);
    EXPECT_EQ(orig, rev);
}

// --- Additional door queries ---

GAME_DIFF_TEST(CCarEnterExit, ComputeDoorFlag_AllDoors) {
    auto* veh = GetCEVehicle();
    if (!veh) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No vehicle"); return; }
    int32 doorIds[] = { 8, 9, 10, 11 }; // RF, RR, LF, LR
    for (bool checkType : { false, true }) {
        for (int32 door : doorIds) {
            int32 orig, rev;
            { HookDisableGuard guard("Global/CCarEnterExit/ComputeDoorFlag"); orig = CCarEnterExit::ComputeDoorFlag(veh, door, checkType); }
            rev = CCarEnterExit::ComputeDoorFlag(veh, door, checkType);
            EXPECT_EQ(orig, rev);
        }
    }
}

GAME_DIFF_TEST(CCarEnterExit, ComputeOppositeDoorFlag_AllDoors) {
    auto* veh = GetCEVehicle();
    if (!veh) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No vehicle"); return; }
    int32 doorIds[] = { 8, 9, 10, 11 };
    for (bool checkType : { false, true }) {
        for (int32 door : doorIds) {
            int32 orig, rev;
            { HookDisableGuard guard("Global/CCarEnterExit/ComputeOppositeDoorFlag"); orig = CCarEnterExit::ComputeOppositeDoorFlag(veh, door, checkType); }
            rev = CCarEnterExit::ComputeOppositeDoorFlag(veh, door, checkType);
            EXPECT_EQ(orig, rev);
        }
    }
}

GAME_DIFF_TEST(CCarEnterExit, IsCarDoorInUse_AllPairs) {
    auto* veh = GetCEVehicle();
    if (!veh) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No vehicle"); return; }
    int32 doorIds[] = { 8, 9, 10, 11 };
    for (int32 d1 : doorIds) {
        for (int32 d2 : doorIds) {
            bool orig, rev;
            { HookDisableGuard guard("Global/CCarEnterExit/IsCarDoorInUse"); orig = CCarEnterExit::IsCarDoorInUse(veh, d1, d2); }
            rev = CCarEnterExit::IsCarDoorInUse(veh, d1, d2);
            EXPECT_EQ(orig, rev);
        }
    }
}

GAME_DIFF_TEST(CCarEnterExit, CarHasOpenableDoor_AllDoors) {
    auto* veh = GetCEVehicle();
    auto* ped = FindPlayerPed(0);
    if (!veh || !ped) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No vehicle or ped"); return; }
    for (int32 door = 0; door < 6; door++) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CCarEnterExit/CarHasOpenableDoor"); orig = CCarEnterExit::CarHasOpenableDoor(veh, door, ped); }
        rev = CCarEnterExit::CarHasOpenableDoor(veh, door, ped);
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(CCarEnterExit, IsCarQuickJackPossible_AllDoors) {
    auto* veh = GetCEVehicle();
    auto* ped = FindPlayerPed(0);
    if (!veh || !ped) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No vehicle or ped"); return; }
    int32 doorIds[] = { 8, 9, 10, 11 };
    for (int32 door : doorIds) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CCarEnterExit/IsCarQuickJackPossible"); orig = CCarEnterExit::IsCarQuickJackPossible(veh, door, ped); }
        rev = CCarEnterExit::IsCarQuickJackPossible(veh, door, ped);
        EXPECT_EQ(orig, rev);
    }
}
