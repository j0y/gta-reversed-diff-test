// test_Scenario_PedInVehicleTypes.cpp — Ped-in-vehicle scenarios across vehicle types.
//
// Uses CStreaming to load bikes, boats, helis, planes, and tests ped/vehicle
// queries that behave differently depending on vehicle type.
// Exercises code paths that were previously untestable (only police car was loaded).

#include "StdInc.h"
#include "TestFramework.h"
#include "ScenarioHelpers.h"
#include "Streaming.h"
#include "StreamingInfo.h"
#include "TxdStore.h"
#include "Models/VehicleModelInfo.h"
#include "Entity/Vehicle/Bike.h"
#include "Entity/Vehicle/Bmx.h"
#include "Entity/Vehicle/Boat.h"
#include "CarEnterExit.h"

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

static void PedInVehLog(const char* fmt, ...) {
    FILE* f = fopen("C:\\ped_in_veh_diag.log", "a");
    if (!f) return;
    va_list args;
    va_start(args, fmt);
    vfprintf(f, fmt, args);
    va_end(args);
    fprintf(f, "\n");
    fflush(f);
    fclose(f);
}

static int32 FindUnloadedVehicleOfType(eVehicleType type) {
    for (int32 id = 400; id <= 611; id++) {
        auto* mi = CModelInfo::GetModelInfo(id);
        if (!mi || mi->GetModelType() != MODEL_INFO_VEHICLE) continue;
        if (mi->AsVehicleModelInfoPtr()->m_nVehicleType != type) continue;
        if (CStreaming::GetInfo(id).GetCdSize() > 0) return id;
    }
    return -1;
}

static bool EnsureModelLoaded(int32 modelId) {
    if (CStreaming::IsModelLoaded(modelId)) return true;
    ResumeOtherThreads resume;
    CStreaming::FlushChannels();
    CStreaming::RequestModel(modelId, STREAMING_KEEP_IN_MEMORY);
    CStreaming::LoadAllRequestedModels(false);
    return CStreaming::IsModelLoaded(modelId);
}

// Spawns a vehicle of specific type via streaming
struct StreamedVeh {
    int32 modelId = -1;
    CVehicle* ptr = nullptr;
    bool weLoaded = false;

    StreamedVeh(eVehicleType type, CVector pos) {
        modelId = FindUnloadedVehicleOfType(type);
        if (modelId < 0) return;
        weLoaded = !CStreaming::IsModelLoaded(modelId);
        if (weLoaded && !EnsureModelLoaded(modelId)) { modelId = -1; return; }

        switch (type) {
        case VEHICLE_TYPE_BMX:  ptr = new CBmx(modelId, RANDOM_VEHICLE); break;
        case VEHICLE_TYPE_BIKE: ptr = new CBike(modelId, RANDOM_VEHICLE); break;
        case VEHICLE_TYPE_BOAT: ptr = new CBoat(modelId, RANDOM_VEHICLE); break;
        default:                ptr = new CAutomobile(modelId, RANDOM_VEHICLE, true); break;
        }
        if (ptr) { ptr->SetPosn(pos); ptr->SetStatus(STATUS_ABANDONED); CWorld::Add(ptr); }
    }

    ~StreamedVeh() {
        if (ptr) { CWorld::Remove(ptr); delete ptr; }
        if (weLoaded && modelId >= 0) CStreaming::RemoveModel(modelId);
    }

    operator bool() const { return ptr != nullptr; }
    CVehicle* Get() const { return ptr; }
    StreamedVeh(const StreamedVeh&) = delete;
    StreamedVeh& operator=(const StreamedVeh&) = delete;
};

// Place/remove ped as driver (no side effects)
static void PlaceAsDriver(CVehicle* veh, CPed* ped) {
    veh->m_pDriver = ped;
    ped->m_pVehicle = veh;
    ped->bInVehicle = true;
}

static void RemoveDriver(CVehicle* veh, CPed* ped) {
    veh->m_pDriver = nullptr;
    ped->m_pVehicle = nullptr;
    ped->bInVehicle = false;
}

// Find the spawned ped from existing tests or make one
static CPed* GetSpawnedPed() {
    static Scenario::TestPed* s_ped = nullptr;
    static bool s_tried = false;
    if (!s_tried) {
        s_tried = true;
        int32 model = Scenario::FindLoadedPedModel();
        if (model >= 0)
            s_ped = new Scenario::TestPed(PED_TYPE_CIVMALE, model, CVector(2465.0f, -1665.0f, 13.5f));
    }
    return (s_ped && *s_ped) ? s_ped->AsPed() : nullptr;
}

#define PV_GUARD(veh, ped) \
    if (!(veh)) { EXPECT_TRUE(true); return; } \
    if (!(ped)) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No ped"); return; }

// ---------------------------------------------------------------------------
// Test: ped state queries differ when in bike vs car vs boat
// ---------------------------------------------------------------------------

// CanPedStepOutCar — virtual, different per vehicle type
GAME_TEST(Scenario_PedInVehTypes, Diff_CanPedStepOutCar_Car) {
    Scenario::TestVehicle car(Scenario::FindLoadedVehicleModel(), CVector(2450.0f, -1665.0f, 13.5f));
    auto* ped = GetSpawnedPed();
    PV_GUARD(car, ped);
    PlaceAsDriver(car.AsVehicle(), ped);
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/CanPedStepOutCar"); orig = car.AsVehicle()->CanPedStepOutCar(false); }
    rev = car.AsVehicle()->CanPedStepOutCar(false);
    EXPECT_EQ(orig, rev);
    RemoveDriver(car.AsVehicle(), ped);
}

GAME_TEST(Scenario_PedInVehTypes, Diff_CanPedStepOutCar_Bike) {
    StreamedVeh bike(VEHICLE_TYPE_BIKE, CVector(2450.0f, -1670.0f, 13.5f));
    auto* ped = GetSpawnedPed();
    PV_GUARD(bike, ped);
    PlaceAsDriver(bike.Get(), ped);
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/CanPedStepOutCar"); orig = bike.Get()->CanPedStepOutCar(false); }
    rev = bike.Get()->CanPedStepOutCar(false);
    EXPECT_EQ(orig, rev);
    RemoveDriver(bike.Get(), ped);
}

GAME_TEST(Scenario_PedInVehTypes, Diff_CanPedStepOutCar_Boat) {
    StreamedVeh boat(VEHICLE_TYPE_BOAT, CVector(2450.0f, -1675.0f, 5.0f));
    auto* ped = GetSpawnedPed();
    PV_GUARD(boat, ped);
    PlaceAsDriver(boat.Get(), ped);
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/CanPedStepOutCar"); orig = boat.Get()->CanPedStepOutCar(false); }
    rev = boat.Get()->CanPedStepOutCar(false);
    EXPECT_EQ(orig, rev);
    RemoveDriver(boat.Get(), ped);
}

GAME_TEST(Scenario_PedInVehTypes, Diff_CanPedStepOutCar_Heli) {
    StreamedVeh heli(VEHICLE_TYPE_HELI, CVector(2450.0f, -1680.0f, 30.0f));
    auto* ped = GetSpawnedPed();
    PV_GUARD(heli, ped);
    PlaceAsDriver(heli.Get(), ped);
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/CanPedStepOutCar"); orig = heli.Get()->CanPedStepOutCar(false); }
    rev = heli.Get()->CanPedStepOutCar(false);
    EXPECT_EQ(orig, rev);
    RemoveDriver(heli.Get(), ped);
}

// ---------------------------------------------------------------------------
// CanPedJumpOutCar — virtual, ped parameter matters
// ---------------------------------------------------------------------------

GAME_TEST(Scenario_PedInVehTypes, Diff_CanPedJumpOutCar_Car) {
    Scenario::TestVehicle car(Scenario::FindLoadedVehicleModel(), CVector(2445.0f, -1665.0f, 13.5f));
    auto* ped = GetSpawnedPed();
    PV_GUARD(car, ped);
    PlaceAsDriver(car.AsVehicle(), ped);
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/CanPedJumpOutCar"); orig = car.AsVehicle()->CanPedJumpOutCar(ped); }
    rev = car.AsVehicle()->CanPedJumpOutCar(ped);
    EXPECT_EQ(orig, rev);
    RemoveDriver(car.AsVehicle(), ped);
}

GAME_TEST(Scenario_PedInVehTypes, Diff_CanPedJumpOutCar_Bike) {
    StreamedVeh bike(VEHICLE_TYPE_BIKE, CVector(2445.0f, -1670.0f, 13.5f));
    auto* ped = GetSpawnedPed();
    PV_GUARD(bike, ped);
    PlaceAsDriver(bike.Get(), ped);
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/CanPedJumpOutCar"); orig = bike.Get()->CanPedJumpOutCar(ped); }
    rev = bike.Get()->CanPedJumpOutCar(ped);
    EXPECT_EQ(orig, rev);
    RemoveDriver(bike.Get(), ped);
}

// ---------------------------------------------------------------------------
// CCarEnterExit cross-type: door queries on different vehicle types
// ---------------------------------------------------------------------------

// IsVehicleHealthy — simple but exercises vehicle type checks
GAME_TEST(Scenario_PedInVehTypes, Diff_IsVehicleHealthy_CrossType) {
    eVehicleType types[] = { VEHICLE_TYPE_BIKE, VEHICLE_TYPE_BOAT, VEHICLE_TYPE_HELI, VEHICLE_TYPE_PLANE };
    const char* names[] = { "bike", "boat", "heli", "plane" };
    for (int i = 0; i < 4; i++) {
        StreamedVeh veh(types[i], CVector(2440.0f + i * 5.0f, -1665.0f, 13.5f));
        if (!veh) { PedInVehLog("SKIP %s", names[i]); continue; }
        bool orig, rev;
        { HookDisableGuard guard("Global/CCarEnterExit/IsVehicleHealthy"); orig = CCarEnterExit::IsVehicleHealthy(veh.Get()); }
        rev = CCarEnterExit::IsVehicleHealthy(veh.Get());
        EXPECT_EQ(orig, rev);
    }
}

// IsClearToDriveAway — checks driver, health, engine status
GAME_TEST(Scenario_PedInVehTypes, Diff_IsClearToDriveAway_CrossType) {
    auto* ped = GetSpawnedPed();
    if (!ped) return;
    eVehicleType types[] = { VEHICLE_TYPE_BIKE, VEHICLE_TYPE_BOAT, VEHICLE_TYPE_HELI };
    const char* names[] = { "bike", "boat", "heli" };
    for (int i = 0; i < 3; i++) {
        StreamedVeh veh(types[i], CVector(2435.0f + i * 5.0f, -1665.0f, 13.5f));
        if (!veh) { PedInVehLog("SKIP %s", names[i]); continue; }

        // Test without driver
        bool orig1, rev1;
        { HookDisableGuard guard("Global/CCarEnterExit/IsClearToDriveAway"); orig1 = CCarEnterExit::IsClearToDriveAway(veh.Get()); }
        rev1 = CCarEnterExit::IsClearToDriveAway(veh.Get());
        EXPECT_EQ(orig1, rev1);

        // Test with driver
        PlaceAsDriver(veh.Get(), ped);
        bool orig2, rev2;
        { HookDisableGuard guard("Global/CCarEnterExit/IsClearToDriveAway"); orig2 = CCarEnterExit::IsClearToDriveAway(veh.Get()); }
        rev2 = CCarEnterExit::IsClearToDriveAway(veh.Get());
        EXPECT_EQ(orig2, rev2);
        RemoveDriver(veh.Get(), ped);
    }
}

// IsVehicleStealable — different behavior for locked vehicles, bikes, etc.
GAME_TEST(Scenario_PedInVehTypes, Diff_IsVehicleStealable_CrossType) {
    auto* ped = GetSpawnedPed();
    if (!ped) return;
    eVehicleType types[] = { VEHICLE_TYPE_BIKE, VEHICLE_TYPE_BOAT, VEHICLE_TYPE_HELI, VEHICLE_TYPE_BMX };
    const char* names[] = { "bike", "boat", "heli", "bmx" };
    for (int i = 0; i < 4; i++) {
        StreamedVeh veh(types[i], CVector(2430.0f + i * 5.0f, -1665.0f, 13.5f));
        if (!veh) { PedInVehLog("SKIP %s", names[i]); continue; }
        bool orig, rev;
        { HookDisableGuard guard("Global/CCarEnterExit/IsVehicleStealable"); orig = CCarEnterExit::IsVehicleStealable(veh.Get(), ped); }
        rev = CCarEnterExit::IsVehicleStealable(veh.Get(), ped);
        EXPECT_EQ(orig, rev);
    }
}

// CarHasDoorToOpen / CarHasDoorToClose — bikes have no doors!
GAME_TEST(Scenario_PedInVehTypes, Diff_CarHasDoorToOpen_CrossType) {
    eVehicleType types[] = { VEHICLE_TYPE_BIKE, VEHICLE_TYPE_BOAT, VEHICLE_TYPE_HELI };
    int doorIds[] = { 0, 1, 2, 3 }; // DOOR_FRONT_LEFT through DOOR_REAR_RIGHT
    for (auto type : types) {
        StreamedVeh veh(type, CVector(2425.0f, -1665.0f, 13.5f));
        if (!veh) continue;
        for (int door : doorIds) {
            bool orig, rev;
            { HookDisableGuard guard("Global/CCarEnterExit/CarHasDoorToOpen"); orig = CCarEnterExit::CarHasDoorToOpen(veh.Get(), door); }
            rev = CCarEnterExit::CarHasDoorToOpen(veh.Get(), door);
            EXPECT_EQ(orig, rev);
        }
    }
}

GAME_TEST(Scenario_PedInVehTypes, Diff_CarHasDoorToClose_CrossType) {
    eVehicleType types[] = { VEHICLE_TYPE_BIKE, VEHICLE_TYPE_BOAT, VEHICLE_TYPE_HELI };
    int doorIds[] = { 0, 1, 2, 3 };
    for (auto type : types) {
        StreamedVeh veh(type, CVector(2420.0f, -1665.0f, 13.5f));
        if (!veh) continue;
        for (int door : doorIds) {
            bool orig, rev;
            { HookDisableGuard guard("Global/CCarEnterExit/CarHasDoorToClose"); orig = CCarEnterExit::CarHasDoorToClose(veh.Get(), door); }
            rev = CCarEnterExit::CarHasDoorToClose(veh.Get(), door);
            EXPECT_EQ(orig, rev);
        }
    }
}

// CarHasOpenableDoor — ped parameter + vehicle type
GAME_TEST(Scenario_PedInVehTypes, Diff_CarHasOpenableDoor_CrossType) {
    auto* ped = GetSpawnedPed();
    if (!ped) return;
    eVehicleType types[] = { VEHICLE_TYPE_BIKE, VEHICLE_TYPE_BOAT, VEHICLE_TYPE_HELI, VEHICLE_TYPE_BMX };
    int doorIds[] = { 0, 1, 2, 3 };
    for (auto type : types) {
        StreamedVeh veh(type, CVector(2415.0f, -1665.0f, 13.5f));
        if (!veh) continue;
        for (int door : doorIds) {
            bool orig, rev;
            { HookDisableGuard guard("Global/CCarEnterExit/CarHasOpenableDoor"); orig = CCarEnterExit::CarHasOpenableDoor(veh.Get(), door, ped); }
            rev = CCarEnterExit::CarHasOpenableDoor(veh.Get(), door, ped);
            EXPECT_EQ(orig, rev);
        }
    }
}

// ComputeDoorFlag — original code crashes when called on non-CAutomobile vehicles
// (accesses door state data that only exists on automobiles). Skipped for streamed types.
// Safe to test on pre-loaded police car (CAutomobile) — covered in test_CCarEnterExit.cpp.

// ComputeTargetDoorToExit — which door does the ped exit from?
GAME_TEST(Scenario_PedInVehTypes, Diff_ComputeTargetDoorToExit_CrossType) {
    auto* ped = GetSpawnedPed();
    if (!ped) return;
    eVehicleType types[] = { VEHICLE_TYPE_BIKE, VEHICLE_TYPE_BOAT, VEHICLE_TYPE_HELI };
    for (auto type : types) {
        StreamedVeh veh(type, CVector(2405.0f, -1665.0f, 13.5f));
        if (!veh) continue;
        PlaceAsDriver(veh.Get(), ped);
        int orig, rev;
        { HookDisableGuard guard("Global/CCarEnterExit/ComputeTargetDoorToExit"); orig = CCarEnterExit::ComputeTargetDoorToExit(veh.Get(), ped); }
        rev = CCarEnterExit::ComputeTargetDoorToExit(veh.Get(), ped);
        EXPECT_EQ(orig, rev);
        RemoveDriver(veh.Get(), ped);
    }
}

// IsCarDoorReady — bikes always ready (no doors to be stuck)
GAME_TEST(Scenario_PedInVehTypes, Diff_IsCarDoorReady_CrossType) {
    eVehicleType types[] = { VEHICLE_TYPE_BIKE, VEHICLE_TYPE_BOAT, VEHICLE_TYPE_HELI, VEHICLE_TYPE_BMX };
    int doorIds[] = { 0, 1 };
    for (auto type : types) {
        StreamedVeh veh(type, CVector(2400.0f, -1665.0f, 13.5f));
        if (!veh) continue;
        for (int door : doorIds) {
            bool orig, rev;
            { HookDisableGuard guard("Global/CCarEnterExit/IsCarDoorReady"); orig = CCarEnterExit::IsCarDoorReady(veh.Get(), door); }
            rev = CCarEnterExit::IsCarDoorReady(veh.Get(), door);
            EXPECT_EQ(orig, rev);
        }
    }
}

// ---------------------------------------------------------------------------
// GetHeightAboveRoad — virtual, different per vehicle subclass
// ---------------------------------------------------------------------------

GAME_TEST(Scenario_PedInVehTypes, Diff_GetHeightAboveRoad_CrossType) {
    eVehicleType types[] = { VEHICLE_TYPE_BIKE, VEHICLE_TYPE_BOAT, VEHICLE_TYPE_HELI, VEHICLE_TYPE_PLANE, VEHICLE_TYPE_BMX };
    const char* names[] = { "bike", "boat", "heli", "plane", "bmx" };
    for (int i = 0; i < 5; i++) {
        StreamedVeh veh(types[i], CVector(2395.0f + i * 5.0f, -1665.0f, 13.5f));
        if (!veh) { PedInVehLog("SKIP %s", names[i]); continue; }
        float orig, rev;
        { HookDisableGuard guard("Vehicle/CVehicle/GetHeightAboveRoad"); orig = veh.Get()->GetHeightAboveRoad(); }
        rev = veh.Get()->GetHeightAboveRoad();
        EXPECT_NEAR(orig, rev, 1e-3f);
    }
}

// ---------------------------------------------------------------------------
// Ped queries while driving different vehicle types
// ---------------------------------------------------------------------------

GAME_TEST(Scenario_PedInVehTypes, Diff_PedIsInControl_InBike) {
    StreamedVeh bike(VEHICLE_TYPE_BIKE, CVector(2390.0f, -1665.0f, 13.5f));
    auto* ped = GetSpawnedPed();
    PV_GUARD(bike, ped);
    PlaceAsDriver(bike.Get(), ped);
    bool orig, rev;
    { HookDisableGuard guard("Entity/Ped/CPed/IsPedInControl"); orig = ped->IsPedInControl(); }
    rev = ped->IsPedInControl();
    EXPECT_EQ(orig, rev);
    RemoveDriver(bike.Get(), ped);
}

GAME_TEST(Scenario_PedInVehTypes, Diff_PedIsInControl_InBoat) {
    StreamedVeh boat(VEHICLE_TYPE_BOAT, CVector(2390.0f, -1670.0f, 5.0f));
    auto* ped = GetSpawnedPed();
    PV_GUARD(boat, ped);
    PlaceAsDriver(boat.Get(), ped);
    bool orig, rev;
    { HookDisableGuard guard("Entity/Ped/CPed/IsPedInControl"); orig = ped->IsPedInControl(); }
    rev = ped->IsPedInControl();
    EXPECT_EQ(orig, rev);
    RemoveDriver(boat.Get(), ped);
}

GAME_TEST(Scenario_PedInVehTypes, Diff_PedIsInControl_InHeli) {
    StreamedVeh heli(VEHICLE_TYPE_HELI, CVector(2390.0f, -1675.0f, 30.0f));
    auto* ped = GetSpawnedPed();
    PV_GUARD(heli, ped);
    PlaceAsDriver(heli.Get(), ped);
    bool orig, rev;
    { HookDisableGuard guard("Entity/Ped/CPed/IsPedInControl"); orig = ped->IsPedInControl(); }
    rev = ped->IsPedInControl();
    EXPECT_EQ(orig, rev);
    RemoveDriver(heli.Get(), ped);
}

// ---------------------------------------------------------------------------
// CanBeDeleted while occupied — different per type
// ---------------------------------------------------------------------------

GAME_TEST(Scenario_PedInVehTypes, Diff_CanBeDeleted_Occupied_CrossType) {
    auto* ped = GetSpawnedPed();
    if (!ped) return;
    eVehicleType types[] = { VEHICLE_TYPE_BIKE, VEHICLE_TYPE_BOAT, VEHICLE_TYPE_HELI, VEHICLE_TYPE_BMX };
    for (auto type : types) {
        StreamedVeh veh(type, CVector(2385.0f, -1665.0f, 13.5f));
        if (!veh) continue;
        PlaceAsDriver(veh.Get(), ped);
        bool orig, rev;
        { HookDisableGuard guard("Vehicle/CVehicle/CanBeDeleted"); orig = veh.Get()->CanBeDeleted(); }
        rev = veh.Get()->CanBeDeleted();
        EXPECT_EQ(orig, rev);
        RemoveDriver(veh.Get(), ped);
    }
}
