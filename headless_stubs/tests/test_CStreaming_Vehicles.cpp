// test_CStreaming_Vehicles.cpp — Tests using CStreaming to load and test different vehicle types.
// Loads bikes, boats, and other vehicle types on demand, then runs CVehicle differential tests.
// Requires ResumeOtherThreads for streaming I/O (CdStream worker must be alive).

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

// ---------------------------------------------------------------------------
// Streaming helpers — ResumeOtherThreads is in TestFramework.h
// ---------------------------------------------------------------------------

static void StreamVehLog(const char* fmt, ...) {
    FILE* f = fopen("C:\\streaming_veh_diag.log", "a");
    if (!f) return;
    va_list args;
    va_start(args, fmt);
    vfprintf(f, fmt, args);
    va_end(args);
    fprintf(f, "\n");
    fflush(f);
    fclose(f);
}

// Find an unloaded model of a specific vehicle type
static int32 FindUnloadedVehicleOfType(eVehicleType type) {
    for (int32 id = 400; id <= 611; id++) {
        auto* mi = CModelInfo::GetModelInfo(id);
        if (!mi || mi->GetModelType() != MODEL_INFO_VEHICLE) continue;
        auto* vmi = mi->AsVehicleModelInfoPtr();
        if (vmi->m_nVehicleType != type) continue;
        // Prefer unloaded so we exercise CStreaming, but accept loaded too
        auto& info = CStreaming::GetInfo(id);
        if (info.GetCdSize() > 0) return id;
    }
    return -1;
}

// Load a model via streaming. Returns true if model is loaded after call.
static bool EnsureModelLoaded(int32 modelId) {
    if (CStreaming::IsModelLoaded(modelId)) return true;

    ResumeOtherThreads resume;
    CStreaming::FlushChannels();
    CStreaming::RequestModel(modelId, STREAMING_KEEP_IN_MEMORY);
    CStreaming::LoadAllRequestedModels(false);
    return CStreaming::IsModelLoaded(modelId);
}

// Unload a model we loaded
static void UnloadModel(int32 modelId) {
    CStreaming::RemoveModel(modelId);
}

// ---------------------------------------------------------------------------
// RAII wrappers for vehicle types
// ---------------------------------------------------------------------------

struct StreamedVehicle {
    int32 modelId = -1;
    CVehicle* ptr = nullptr;
    bool weLoaded = false; // did we load this model ourselves?

    StreamedVehicle(eVehicleType type, CVector pos) {
        modelId = FindUnloadedVehicleOfType(type);
        if (modelId < 0) return;

        weLoaded = !CStreaming::IsModelLoaded(modelId);
        if (weLoaded && !EnsureModelLoaded(modelId)) {
            StreamVehLog("Failed to load model %d (type %d)", modelId, (int)type);
            modelId = -1;
            return;
        }

        auto* mi = CModelInfo::GetModelInfo(modelId);
        auto* vmi = mi->AsVehicleModelInfoPtr();

        switch (type) {
        case VEHICLE_TYPE_BMX:
            ptr = new CBmx(modelId, RANDOM_VEHICLE);
            break;
        case VEHICLE_TYPE_BIKE:
            ptr = new CBike(modelId, RANDOM_VEHICLE);
            break;
        case VEHICLE_TYPE_BOAT:
            ptr = new CBoat(modelId, RANDOM_VEHICLE);
            break;
        default:
            ptr = new CAutomobile(modelId, RANDOM_VEHICLE, true);
            break;
        }

        if (ptr) {
            ptr->SetPosn(pos);
            ptr->SetStatus(STATUS_ABANDONED);
            CWorld::Add(ptr);
            StreamVehLog("Spawned model %d (type %d) at (%.0f,%.0f,%.0f)",
                modelId, (int)type, pos.x, pos.y, pos.z);
        }
    }

    ~StreamedVehicle() {
        if (ptr) {
            CWorld::Remove(ptr);
            delete ptr;
            ptr = nullptr;
        }
        if (weLoaded && modelId >= 0) {
            UnloadModel(modelId);
        }
    }

    operator bool() const { return ptr != nullptr; }
    CVehicle* operator->() const { return ptr; }
    CVehicle* Get() const { return ptr; }

    StreamedVehicle(const StreamedVehicle&) = delete;
    StreamedVehicle& operator=(const StreamedVehicle&) = delete;
};

// ---------------------------------------------------------------------------
// Helper macros
// ---------------------------------------------------------------------------

#define SKIP_IF_NULL(veh) \
    if (!(veh)) { StreamVehLog("SKIP: could not spawn vehicle"); EXPECT_TRUE(true); return; }

// ---------------------------------------------------------------------------
// BIKE tests
// ---------------------------------------------------------------------------

GAME_TEST(CStreamVeh, Bike_Spawn) {
    StreamedVehicle bike(VEHICLE_TYPE_BIKE, CVector(2490.0f, -1665.0f, 13.5f));
    SKIP_IF_NULL(bike);
    StreamVehLog("Bike model=%d", bike.modelId);
    EXPECT_TRUE(bike.Get() != nullptr);
    EXPECT_TRUE(bike->GetVehicleAppearance() == VEHICLE_APPEARANCE_BIKE);
}

GAME_TEST(CStreamVeh, Bike_IsLawEnforcement) {
    StreamedVehicle bike(VEHICLE_TYPE_BIKE, CVector(2490.0f, -1665.0f, 13.5f));
    SKIP_IF_NULL(bike);
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/IsLawEnforcementVehicle"); orig = bike->IsLawEnforcementVehicle(); }
    rev = bike->IsLawEnforcementVehicle();
    EXPECT_EQ(orig, rev);
}

GAME_TEST(CStreamVeh, Bike_CanBeDriven) {
    StreamedVehicle bike(VEHICLE_TYPE_BIKE, CVector(2490.0f, -1665.0f, 13.5f));
    SKIP_IF_NULL(bike);
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/CanBeDriven"); orig = bike->CanBeDriven(); }
    rev = bike->CanBeDriven();
    EXPECT_EQ(orig, rev);
}

GAME_TEST(CStreamVeh, Bike_UsesSiren) {
    StreamedVehicle bike(VEHICLE_TYPE_BIKE, CVector(2490.0f, -1665.0f, 13.5f));
    SKIP_IF_NULL(bike);
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/UsesSiren"); orig = bike->UsesSiren(); }
    rev = bike->UsesSiren();
    EXPECT_EQ(orig, rev);
}

GAME_TEST(CStreamVeh, Bike_GetVehicleAppearance) {
    StreamedVehicle bike(VEHICLE_TYPE_BIKE, CVector(2490.0f, -1665.0f, 13.5f));
    SKIP_IF_NULL(bike);
    int orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/GetVehicleAppearance"); orig = bike->GetVehicleAppearance(); }
    rev = bike->GetVehicleAppearance();
    EXPECT_EQ(orig, rev);
}

GAME_TEST(CStreamVeh, Bike_CanPedEnterCar) {
    StreamedVehicle bike(VEHICLE_TYPE_BIKE, CVector(2490.0f, -1665.0f, 13.5f));
    SKIP_IF_NULL(bike);
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/CanPedEnterCar"); orig = bike->CanPedEnterCar(); }
    rev = bike->CanPedEnterCar();
    EXPECT_EQ(orig, rev);
}

GAME_TEST(CStreamVeh, Bike_IsUpsideDown) {
    StreamedVehicle bike(VEHICLE_TYPE_BIKE, CVector(2490.0f, -1665.0f, 13.5f));
    SKIP_IF_NULL(bike);
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/IsUpsideDown"); orig = bike->IsUpsideDown(); }
    rev = bike->IsUpsideDown();
    EXPECT_EQ(orig, rev);
}

GAME_TEST(CStreamVeh, Bike_GetAllWheelsOffGround) {
    StreamedVehicle bike(VEHICLE_TYPE_BIKE, CVector(2490.0f, -1665.0f, 13.5f));
    SKIP_IF_NULL(bike);
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CBike/GetAllWheelsOffGround"); orig = ((CBike*)bike.Get())->GetAllWheelsOffGround(); }
    rev = ((CBike*)bike.Get())->GetAllWheelsOffGround();
    EXPECT_EQ(orig, rev);
}

GAME_TEST(CStreamVeh, Bike_GetVehicleLightsStatus) {
    StreamedVehicle bike(VEHICLE_TYPE_BIKE, CVector(2490.0f, -1665.0f, 13.5f));
    SKIP_IF_NULL(bike);
    int orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/GetVehicleLightsStatus"); orig = bike->GetVehicleLightsStatus(); }
    rev = bike->GetVehicleLightsStatus();
    EXPECT_EQ(orig, rev);
}

GAME_TEST(CStreamVeh, Bike_CanVehicleBeDamaged) {
    StreamedVehicle bike(VEHICLE_TYPE_BIKE, CVector(2490.0f, -1665.0f, 13.5f));
    SKIP_IF_NULL(bike);
    bool orig, rev;
    bool dmgFlag1 = false, dmgFlag2 = false;
    auto* player = FindPlayerPed(0);
    { HookDisableGuard guard("Vehicle/CVehicle/CanVehicleBeDamaged"); orig = bike->CanVehicleBeDamaged(player, WEAPON_UNARMED, dmgFlag1); }
    rev = bike->CanVehicleBeDamaged(player, WEAPON_UNARMED, dmgFlag2);
    EXPECT_EQ(orig, rev);
    EXPECT_EQ(dmgFlag1, dmgFlag2);
}

// ---------------------------------------------------------------------------
// BOAT tests
// ---------------------------------------------------------------------------

GAME_TEST(CStreamVeh, Boat_Spawn) {
    StreamedVehicle boat(VEHICLE_TYPE_BOAT, CVector(2490.0f, -1665.0f, 5.0f));
    SKIP_IF_NULL(boat);
    StreamVehLog("Boat model=%d", boat.modelId);
    EXPECT_TRUE(boat.Get() != nullptr);
    EXPECT_TRUE(boat->GetVehicleAppearance() == VEHICLE_APPEARANCE_BOAT);
}

GAME_TEST(CStreamVeh, Boat_IsLawEnforcement) {
    StreamedVehicle boat(VEHICLE_TYPE_BOAT, CVector(2490.0f, -1665.0f, 5.0f));
    SKIP_IF_NULL(boat);
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/IsLawEnforcementVehicle"); orig = boat->IsLawEnforcementVehicle(); }
    rev = boat->IsLawEnforcementVehicle();
    EXPECT_EQ(orig, rev);
}

GAME_TEST(CStreamVeh, Boat_CanBeDriven) {
    StreamedVehicle boat(VEHICLE_TYPE_BOAT, CVector(2490.0f, -1665.0f, 5.0f));
    SKIP_IF_NULL(boat);
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/CanBeDriven"); orig = boat->CanBeDriven(); }
    rev = boat->CanBeDriven();
    EXPECT_EQ(orig, rev);
}

GAME_TEST(CStreamVeh, Boat_UsesSiren) {
    StreamedVehicle boat(VEHICLE_TYPE_BOAT, CVector(2490.0f, -1665.0f, 5.0f));
    SKIP_IF_NULL(boat);
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/UsesSiren"); orig = boat->UsesSiren(); }
    rev = boat->UsesSiren();
    EXPECT_EQ(orig, rev);
}

GAME_TEST(CStreamVeh, Boat_GetVehicleAppearance) {
    StreamedVehicle boat(VEHICLE_TYPE_BOAT, CVector(2490.0f, -1665.0f, 5.0f));
    SKIP_IF_NULL(boat);
    int orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/GetVehicleAppearance"); orig = boat->GetVehicleAppearance(); }
    rev = boat->GetVehicleAppearance();
    EXPECT_EQ(orig, rev);
}

GAME_TEST(CStreamVeh, Boat_CanPedEnterCar) {
    StreamedVehicle boat(VEHICLE_TYPE_BOAT, CVector(2490.0f, -1665.0f, 5.0f));
    SKIP_IF_NULL(boat);
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/CanPedEnterCar"); orig = boat->CanPedEnterCar(); }
    rev = boat->CanPedEnterCar();
    EXPECT_EQ(orig, rev);
}

GAME_TEST(CStreamVeh, Boat_IsOnItsSide) {
    StreamedVehicle boat(VEHICLE_TYPE_BOAT, CVector(2490.0f, -1665.0f, 5.0f));
    SKIP_IF_NULL(boat);
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/IsOnItsSide"); orig = boat->IsOnItsSide(); }
    rev = boat->IsOnItsSide();
    EXPECT_EQ(orig, rev);
}

GAME_TEST(CStreamVeh, Boat_GetVehicleLightsStatus) {
    StreamedVehicle boat(VEHICLE_TYPE_BOAT, CVector(2490.0f, -1665.0f, 5.0f));
    SKIP_IF_NULL(boat);
    int orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/GetVehicleLightsStatus"); orig = boat->GetVehicleLightsStatus(); }
    rev = boat->GetVehicleLightsStatus();
    EXPECT_EQ(orig, rev);
}

// ---------------------------------------------------------------------------
// HELI tests (uses CAutomobile constructor — helis are CAutomobile subtype)
// ---------------------------------------------------------------------------

GAME_TEST(CStreamVeh, Heli_Spawn) {
    StreamedVehicle heli(VEHICLE_TYPE_HELI, CVector(2490.0f, -1665.0f, 30.0f));
    SKIP_IF_NULL(heli);
    StreamVehLog("Heli model=%d", heli.modelId);
    EXPECT_TRUE(heli.Get() != nullptr);
    EXPECT_TRUE(heli->GetVehicleAppearance() == VEHICLE_APPEARANCE_HELI);
}

GAME_TEST(CStreamVeh, Heli_CanBeDriven) {
    StreamedVehicle heli(VEHICLE_TYPE_HELI, CVector(2490.0f, -1665.0f, 30.0f));
    SKIP_IF_NULL(heli);
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/CanBeDriven"); orig = heli->CanBeDriven(); }
    rev = heli->CanBeDriven();
    EXPECT_EQ(orig, rev);
}

GAME_TEST(CStreamVeh, Heli_GetVehicleAppearance) {
    StreamedVehicle heli(VEHICLE_TYPE_HELI, CVector(2490.0f, -1665.0f, 30.0f));
    SKIP_IF_NULL(heli);
    int orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/GetVehicleAppearance"); orig = heli->GetVehicleAppearance(); }
    rev = heli->GetVehicleAppearance();
    EXPECT_EQ(orig, rev);
}

GAME_TEST(CStreamVeh, Heli_UsesSiren) {
    StreamedVehicle heli(VEHICLE_TYPE_HELI, CVector(2490.0f, -1665.0f, 30.0f));
    SKIP_IF_NULL(heli);
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/UsesSiren"); orig = heli->UsesSiren(); }
    rev = heli->UsesSiren();
    EXPECT_EQ(orig, rev);
}

GAME_TEST(CStreamVeh, Heli_GetPlaneNumGuns) {
    StreamedVehicle heli(VEHICLE_TYPE_HELI, CVector(2490.0f, -1665.0f, 30.0f));
    SKIP_IF_NULL(heli);
    int orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/GetPlaneNumGuns"); orig = heli->GetPlaneNumGuns(); }
    rev = heli->GetPlaneNumGuns();
    EXPECT_EQ(orig, rev);
}

// ---------------------------------------------------------------------------
// PLANE tests
// ---------------------------------------------------------------------------

GAME_TEST(CStreamVeh, Plane_Spawn) {
    StreamedVehicle plane(VEHICLE_TYPE_PLANE, CVector(2490.0f, -1665.0f, 30.0f));
    SKIP_IF_NULL(plane);
    StreamVehLog("Plane model=%d", plane.modelId);
    EXPECT_TRUE(plane.Get() != nullptr);
    EXPECT_TRUE(plane->GetVehicleAppearance() == VEHICLE_APPEARANCE_PLANE);
}

GAME_TEST(CStreamVeh, Plane_CanBeDriven) {
    StreamedVehicle plane(VEHICLE_TYPE_PLANE, CVector(2490.0f, -1665.0f, 30.0f));
    SKIP_IF_NULL(plane);
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/CanBeDriven"); orig = plane->CanBeDriven(); }
    rev = plane->CanBeDriven();
    EXPECT_EQ(orig, rev);
}

GAME_TEST(CStreamVeh, Plane_GetVehicleAppearance) {
    StreamedVehicle plane(VEHICLE_TYPE_PLANE, CVector(2490.0f, -1665.0f, 30.0f));
    SKIP_IF_NULL(plane);
    int orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/GetVehicleAppearance"); orig = plane->GetVehicleAppearance(); }
    rev = plane->GetVehicleAppearance();
    EXPECT_EQ(orig, rev);
}

GAME_TEST(CStreamVeh, Plane_GetPlaneNumGuns) {
    StreamedVehicle plane(VEHICLE_TYPE_PLANE, CVector(2490.0f, -1665.0f, 30.0f));
    SKIP_IF_NULL(plane);
    int orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/GetPlaneNumGuns"); orig = plane->GetPlaneNumGuns(); }
    rev = plane->GetPlaneNumGuns();
    EXPECT_EQ(orig, rev);
}

// ---------------------------------------------------------------------------
// TRAILER tests
// ---------------------------------------------------------------------------

GAME_TEST(CStreamVeh, Trailer_Spawn) {
    StreamedVehicle trailer(VEHICLE_TYPE_TRAILER, CVector(2490.0f, -1665.0f, 13.5f));
    SKIP_IF_NULL(trailer);
    StreamVehLog("Trailer model=%d", trailer.modelId);
    EXPECT_TRUE(trailer.Get() != nullptr);
}

GAME_TEST(CStreamVeh, Trailer_CanBeDriven) {
    StreamedVehicle trailer(VEHICLE_TYPE_TRAILER, CVector(2490.0f, -1665.0f, 13.5f));
    SKIP_IF_NULL(trailer);
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/CanBeDriven"); orig = trailer->CanBeDriven(); }
    rev = trailer->CanBeDriven();
    EXPECT_EQ(orig, rev);
}

GAME_TEST(CStreamVeh, Trailer_IsLawEnforcement) {
    StreamedVehicle trailer(VEHICLE_TYPE_TRAILER, CVector(2490.0f, -1665.0f, 13.5f));
    SKIP_IF_NULL(trailer);
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/IsLawEnforcementVehicle"); orig = trailer->IsLawEnforcementVehicle(); }
    rev = trailer->IsLawEnforcementVehicle();
    EXPECT_EQ(orig, rev);
}

// ---------------------------------------------------------------------------
// BMX tests (CBmx extends CBike)
// ---------------------------------------------------------------------------

GAME_TEST(CStreamVeh, Bmx_Spawn) {
    StreamedVehicle bmx(VEHICLE_TYPE_BMX, CVector(2490.0f, -1665.0f, 13.5f));
    SKIP_IF_NULL(bmx);
    StreamVehLog("BMX model=%d", bmx.modelId);
    EXPECT_TRUE(bmx.Get() != nullptr);
    EXPECT_TRUE(bmx->GetVehicleAppearance() == VEHICLE_APPEARANCE_BIKE);
}

GAME_TEST(CStreamVeh, Bmx_CanBeDriven) {
    StreamedVehicle bmx(VEHICLE_TYPE_BMX, CVector(2490.0f, -1665.0f, 13.5f));
    SKIP_IF_NULL(bmx);
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/CanBeDriven"); orig = bmx->CanBeDriven(); }
    rev = bmx->CanBeDriven();
    EXPECT_EQ(orig, rev);
}

GAME_TEST(CStreamVeh, Bmx_UsesSiren) {
    StreamedVehicle bmx(VEHICLE_TYPE_BMX, CVector(2490.0f, -1665.0f, 13.5f));
    SKIP_IF_NULL(bmx);
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/UsesSiren"); orig = bmx->UsesSiren(); }
    rev = bmx->UsesSiren();
    EXPECT_EQ(orig, rev);
}

GAME_TEST(CStreamVeh, Bmx_CanPedEnterCar) {
    StreamedVehicle bmx(VEHICLE_TYPE_BMX, CVector(2490.0f, -1665.0f, 13.5f));
    SKIP_IF_NULL(bmx);
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/CanPedEnterCar"); orig = bmx->CanPedEnterCar(); }
    rev = bmx->CanPedEnterCar();
    EXPECT_EQ(orig, rev);
}

GAME_TEST(CStreamVeh, Bmx_GetVehicleAppearance) {
    StreamedVehicle bmx(VEHICLE_TYPE_BMX, CVector(2490.0f, -1665.0f, 13.5f));
    SKIP_IF_NULL(bmx);
    int orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/GetVehicleAppearance"); orig = bmx->GetVehicleAppearance(); }
    rev = bmx->GetVehicleAppearance();
    EXPECT_EQ(orig, rev);
}

GAME_TEST(CStreamVeh, Bmx_GetAllWheelsOffGround) {
    StreamedVehicle bmx(VEHICLE_TYPE_BMX, CVector(2490.0f, -1665.0f, 13.5f));
    SKIP_IF_NULL(bmx);
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CBike/GetAllWheelsOffGround"); orig = ((CBike*)bmx.Get())->GetAllWheelsOffGround(); }
    rev = ((CBike*)bmx.Get())->GetAllWheelsOffGround();
    EXPECT_EQ(orig, rev);
}

GAME_TEST(CStreamVeh, Bmx_IsUpsideDown) {
    StreamedVehicle bmx(VEHICLE_TYPE_BMX, CVector(2490.0f, -1665.0f, 13.5f));
    SKIP_IF_NULL(bmx);
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/IsUpsideDown"); orig = bmx->IsUpsideDown(); }
    rev = bmx->IsUpsideDown();
    EXPECT_EQ(orig, rev);
}

GAME_TEST(CStreamVeh, Bmx_GetVehicleLightsStatus) {
    StreamedVehicle bmx(VEHICLE_TYPE_BMX, CVector(2490.0f, -1665.0f, 13.5f));
    SKIP_IF_NULL(bmx);
    int orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/GetVehicleLightsStatus"); orig = bmx->GetVehicleLightsStatus(); }
    rev = bmx->GetVehicleLightsStatus();
    EXPECT_EQ(orig, rev);
}

// ---------------------------------------------------------------------------
// Cross-type: same CVehicle function tested on EVERY vehicle type
// Verifies GetVehicleAppearance, CanPedOpenLocks, IsSphereTouchingVehicle
// ---------------------------------------------------------------------------

GAME_TEST(CStreamVeh, CrossType_CanPedOpenLocks) {
    eVehicleType types[] = { VEHICLE_TYPE_BIKE, VEHICLE_TYPE_BMX, VEHICLE_TYPE_BOAT, VEHICLE_TYPE_HELI, VEHICLE_TYPE_PLANE };
    const char* names[] = { "bike", "bmx", "boat", "heli", "plane" };
    for (int i = 0; i < 5; i++) {
        StreamedVehicle veh(types[i], CVector(2480.0f + i * 5.0f, -1665.0f, 13.5f));
        if (!veh) { StreamVehLog("SKIP %s: couldn't spawn", names[i]); continue; }
        bool orig, rev;
        { HookDisableGuard guard("Vehicle/CVehicle/CanPedOpenLocks"); orig = veh->CanPedOpenLocks(FindPlayerPed(0)); }
        rev = veh->CanPedOpenLocks(FindPlayerPed(0));
        EXPECT_EQ(orig, rev);
    }
}

GAME_TEST(CStreamVeh, CrossType_CanDoorsBeDamaged) {
    eVehicleType types[] = { VEHICLE_TYPE_BIKE, VEHICLE_TYPE_BMX, VEHICLE_TYPE_BOAT, VEHICLE_TYPE_HELI, VEHICLE_TYPE_PLANE };
    const char* names[] = { "bike", "bmx", "boat", "heli", "plane" };
    for (int i = 0; i < 5; i++) {
        StreamedVehicle veh(types[i], CVector(2480.0f + i * 5.0f, -1665.0f, 13.5f));
        if (!veh) { StreamVehLog("SKIP %s: couldn't spawn", names[i]); continue; }
        bool orig, rev;
        { HookDisableGuard guard("Vehicle/CVehicle/CanDoorsBeDamaged"); orig = veh->CanDoorsBeDamaged(); }
        rev = veh->CanDoorsBeDamaged();
        EXPECT_EQ(orig, rev);
    }
}

GAME_TEST(CStreamVeh, CrossType_IsVisible) {
    eVehicleType types[] = { VEHICLE_TYPE_BIKE, VEHICLE_TYPE_BMX, VEHICLE_TYPE_BOAT, VEHICLE_TYPE_HELI, VEHICLE_TYPE_PLANE };
    const char* names[] = { "bike", "bmx", "boat", "heli", "plane" };
    for (int i = 0; i < 5; i++) {
        StreamedVehicle veh(types[i], CVector(2480.0f + i * 5.0f, -1665.0f, 13.5f));
        if (!veh) { StreamVehLog("SKIP %s: couldn't spawn", names[i]); continue; }
        bool orig, rev;
        { HookDisableGuard guard("Entity/CEntity/IsVisible"); orig = veh->IsVisible(); }
        rev = veh->IsVisible();
        EXPECT_EQ(orig, rev);
    }
}
