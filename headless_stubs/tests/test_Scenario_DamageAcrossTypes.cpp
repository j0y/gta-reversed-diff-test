// test_Scenario_DamageAcrossTypes.cpp — Vehicle damage/health queries across vehicle types.
//
// Tests CVehicle health-dependent queries on bikes, boats, helis, planes.
// CDamageManager is CAutomobile-only, but m_fHealth and CVehicle-level queries
// apply to all types and may have type-dependent code paths.

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
// Helpers (same streaming pattern as test_CStreaming_Vehicles.cpp)
// ---------------------------------------------------------------------------

static int32 FindVehicleOfType(eVehicleType type) {
    for (int32 id = 400; id <= 611; id++) {
        auto* mi = CModelInfo::GetModelInfo(id);
        if (!mi || mi->GetModelType() != MODEL_INFO_VEHICLE) continue;
        if (mi->AsVehicleModelInfoPtr()->m_nVehicleType != type) continue;
        if (CStreaming::GetInfo(id).GetCdSize() > 0) return id;
    }
    return -1;
}

static bool EnsureLoaded(int32 modelId) {
    if (CStreaming::IsModelLoaded(modelId)) return true;
    ResumeOtherThreads resume;
    CStreaming::FlushChannels();
    CStreaming::RequestModel(modelId, STREAMING_KEEP_IN_MEMORY);
    CStreaming::LoadAllRequestedModels(false);
    return CStreaming::IsModelLoaded(modelId);
}

struct DmgVehicle {
    int32 modelId = -1;
    CVehicle* ptr = nullptr;
    bool weLoaded = false;
    float savedHealth = 0.0f;

    DmgVehicle(eVehicleType type, CVector pos) {
        modelId = FindVehicleOfType(type);
        if (modelId < 0) return;
        weLoaded = !CStreaming::IsModelLoaded(modelId);
        if (weLoaded && !EnsureLoaded(modelId)) { modelId = -1; return; }

        switch (type) {
        case VEHICLE_TYPE_BMX:  ptr = new CBmx(modelId, RANDOM_VEHICLE); break;
        case VEHICLE_TYPE_BIKE: ptr = new CBike(modelId, RANDOM_VEHICLE); break;
        case VEHICLE_TYPE_BOAT: ptr = new CBoat(modelId, RANDOM_VEHICLE); break;
        default:                ptr = new CAutomobile(modelId, RANDOM_VEHICLE, true); break;
        }
        if (ptr) {
            ptr->SetPosn(pos);
            ptr->SetStatus(STATUS_ABANDONED);
            CWorld::Add(ptr);
            savedHealth = ptr->m_fHealth;
        }
    }

    ~DmgVehicle() {
        if (ptr) { ptr->m_fHealth = savedHealth; CWorld::Remove(ptr); delete ptr; }
        if (weLoaded && modelId >= 0) CStreaming::RemoveModel(modelId);
    }

    operator bool() const { return ptr != nullptr; }
    CVehicle* Get() const { return ptr; }
    DmgVehicle(const DmgVehicle&) = delete;
    DmgVehicle& operator=(const DmgVehicle&) = delete;
};

// ---------------------------------------------------------------------------
// IsVehicleNormal at different health levels across types
// ---------------------------------------------------------------------------

GAME_TEST(Scenario_DmgTypes, Diff_IsVehicleNormal_HealthLevels) {
    eVehicleType types[] = { VEHICLE_TYPE_BIKE, VEHICLE_TYPE_BOAT, VEHICLE_TYPE_HELI, VEHICLE_TYPE_PLANE, VEHICLE_TYPE_BMX };
    float healthValues[] = { 1000.0f, 500.0f, 250.0f, 100.0f, 0.0f };
    for (auto type : types) {
        DmgVehicle veh(type, CVector(2360.0f, -1665.0f, 13.5f));
        if (!veh) continue;
        for (float h : healthValues) {
            veh.Get()->m_fHealth = h;
            bool orig, rev;
            { HookDisableGuard guard("Vehicle/CVehicle/IsVehicleNormal"); orig = veh.Get()->IsVehicleNormal(); }
            rev = veh.Get()->IsVehicleNormal();
            EXPECT_EQ(orig, rev);
        }
    }
}

// ---------------------------------------------------------------------------
// CanBeDriven at different health levels
// ---------------------------------------------------------------------------

GAME_TEST(Scenario_DmgTypes, Diff_CanBeDriven_HealthLevels) {
    eVehicleType types[] = { VEHICLE_TYPE_BIKE, VEHICLE_TYPE_BOAT, VEHICLE_TYPE_HELI, VEHICLE_TYPE_PLANE, VEHICLE_TYPE_BMX };
    float healthValues[] = { 1000.0f, 500.0f, 250.0f, 0.0f, -1.0f };
    for (auto type : types) {
        DmgVehicle veh(type, CVector(2355.0f, -1665.0f, 13.5f));
        if (!veh) continue;
        for (float h : healthValues) {
            veh.Get()->m_fHealth = h;
            bool orig, rev;
            { HookDisableGuard guard("Vehicle/CVehicle/CanBeDriven"); orig = veh.Get()->CanBeDriven(); }
            rev = veh.Get()->CanBeDriven();
            EXPECT_EQ(orig, rev);
        }
    }
}

// ---------------------------------------------------------------------------
// CanVehicleBeDamaged — weapon type + vehicle type matrix
// ---------------------------------------------------------------------------

GAME_TEST(Scenario_DmgTypes, Diff_CanVehicleBeDamaged_WeaponMatrix) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    eVehicleType types[] = { VEHICLE_TYPE_BIKE, VEHICLE_TYPE_BOAT, VEHICLE_TYPE_HELI };
    eWeaponType weapons[] = { WEAPON_UNARMED, WEAPON_PISTOL, WEAPON_AK47, WEAPON_ROCKET, WEAPON_MOLOTOV };
    for (auto type : types) {
        DmgVehicle veh(type, CVector(2350.0f, -1665.0f, 13.5f));
        if (!veh) continue;
        for (auto wep : weapons) {
            bool origFlag = false, revFlag = false;
            bool orig, rev;
            { HookDisableGuard guard("Vehicle/CVehicle/CanVehicleBeDamaged");
              orig = veh.Get()->CanVehicleBeDamaged(player, wep, origFlag); }
            rev = veh.Get()->CanVehicleBeDamaged(player, wep, revFlag);
            EXPECT_EQ(orig, rev);
            EXPECT_EQ(origFlag, revFlag);
        }
    }
}

// ---------------------------------------------------------------------------
// GetVehicleLightsStatus across types at different health
// ---------------------------------------------------------------------------

GAME_TEST(Scenario_DmgTypes, Diff_GetVehicleLightsStatus_CrossType) {
    eVehicleType types[] = { VEHICLE_TYPE_BIKE, VEHICLE_TYPE_BOAT, VEHICLE_TYPE_HELI, VEHICLE_TYPE_PLANE };
    for (auto type : types) {
        DmgVehicle veh(type, CVector(2345.0f, -1665.0f, 13.5f));
        if (!veh) continue;

        // Full health
        veh.Get()->m_fHealth = 1000.0f;
        int orig1, rev1;
        { HookDisableGuard guard("Vehicle/CVehicle/GetVehicleLightsStatus"); orig1 = (int)veh.Get()->GetVehicleLightsStatus(); }
        rev1 = (int)veh.Get()->GetVehicleLightsStatus();
        EXPECT_EQ(orig1, rev1);

        // Low health
        veh.Get()->m_fHealth = 200.0f;
        int orig2, rev2;
        { HookDisableGuard guard("Vehicle/CVehicle/GetVehicleLightsStatus"); orig2 = (int)veh.Get()->GetVehicleLightsStatus(); }
        rev2 = (int)veh.Get()->GetVehicleLightsStatus();
        EXPECT_EQ(orig2, rev2);
    }
}

// ---------------------------------------------------------------------------
// CCarEnterExit::IsVehicleHealthy at various health thresholds
// ---------------------------------------------------------------------------

GAME_TEST(Scenario_DmgTypes, Diff_IsVehicleHealthy_HealthLevels) {
    eVehicleType types[] = { VEHICLE_TYPE_BIKE, VEHICLE_TYPE_BOAT, VEHICLE_TYPE_HELI, VEHICLE_TYPE_PLANE, VEHICLE_TYPE_BMX };
    float healthValues[] = { 1000.0f, 500.0f, 250.0f, 100.0f, 0.0f };
    for (auto type : types) {
        DmgVehicle veh(type, CVector(2340.0f, -1665.0f, 13.5f));
        if (!veh) continue;
        for (float h : healthValues) {
            veh.Get()->m_fHealth = h;
            bool orig, rev;
            { HookDisableGuard guard("Global/CCarEnterExit/IsVehicleHealthy"); orig = CCarEnterExit::IsVehicleHealthy(veh.Get()); }
            rev = CCarEnterExit::IsVehicleHealthy(veh.Get());
            EXPECT_EQ(orig, rev);
        }
    }
}

// ---------------------------------------------------------------------------
// CanPedEnterCar on damaged vehicles — health affects enterability
// ---------------------------------------------------------------------------

GAME_TEST(Scenario_DmgTypes, Diff_CanPedEnterCar_Damaged) {
    eVehicleType types[] = { VEHICLE_TYPE_BIKE, VEHICLE_TYPE_BOAT, VEHICLE_TYPE_HELI, VEHICLE_TYPE_BMX };
    float healthValues[] = { 1000.0f, 250.0f, 0.0f };
    for (auto type : types) {
        DmgVehicle veh(type, CVector(2335.0f, -1665.0f, 13.5f));
        if (!veh) continue;
        for (float h : healthValues) {
            veh.Get()->m_fHealth = h;
            bool orig, rev;
            { HookDisableGuard guard("Vehicle/CVehicle/CanPedEnterCar"); orig = veh.Get()->CanPedEnterCar(); }
            rev = veh.Get()->CanPedEnterCar();
            EXPECT_EQ(orig, rev);
        }
    }
}

// ---------------------------------------------------------------------------
// CanDoorsBeDamaged — should differ for bikes (no doors) vs helis
// ---------------------------------------------------------------------------

GAME_TEST(Scenario_DmgTypes, Diff_CanDoorsBeDamaged_CrossType) {
    eVehicleType types[] = { VEHICLE_TYPE_BIKE, VEHICLE_TYPE_BOAT, VEHICLE_TYPE_HELI, VEHICLE_TYPE_PLANE, VEHICLE_TYPE_BMX };
    for (auto type : types) {
        DmgVehicle veh(type, CVector(2330.0f, -1665.0f, 13.5f));
        if (!veh) continue;
        bool orig, rev;
        { HookDisableGuard guard("Vehicle/CVehicle/CanDoorsBeDamaged"); orig = veh.Get()->CanDoorsBeDamaged(); }
        rev = veh.Get()->CanDoorsBeDamaged();
        EXPECT_EQ(orig, rev);
    }
}

// ---------------------------------------------------------------------------
// Behavior: health clamping and initial state
// ---------------------------------------------------------------------------

GAME_TEST(Scenario_DmgTypes, InitialHealth_CrossType) {
    eVehicleType types[] = { VEHICLE_TYPE_BIKE, VEHICLE_TYPE_BOAT, VEHICLE_TYPE_HELI, VEHICLE_TYPE_PLANE, VEHICLE_TYPE_BMX };
    for (auto type : types) {
        DmgVehicle veh(type, CVector(2325.0f, -1665.0f, 13.5f));
        if (!veh) continue;
        // All freshly spawned vehicles should have positive health
        EXPECT_GT(veh.Get()->m_fHealth, 0.0f);
        EXPECT_TRUE(veh.Get()->m_fHealth <= 1000.0f);
    }
}

// ---------------------------------------------------------------------------
// UsesSiren at zero health — edge case
// ---------------------------------------------------------------------------

GAME_TEST(Scenario_DmgTypes, Diff_UsesSiren_ZeroHealth) {
    eVehicleType types[] = { VEHICLE_TYPE_BIKE, VEHICLE_TYPE_BOAT, VEHICLE_TYPE_HELI };
    for (auto type : types) {
        DmgVehicle veh(type, CVector(2320.0f, -1665.0f, 13.5f));
        if (!veh) continue;
        veh.Get()->m_fHealth = 0.0f;
        bool orig, rev;
        { HookDisableGuard guard("Vehicle/CVehicle/UsesSiren"); orig = veh.Get()->UsesSiren(); }
        rev = veh.Get()->UsesSiren();
        EXPECT_EQ(orig, rev);
    }
}
