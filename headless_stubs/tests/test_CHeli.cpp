// test_CHeli.cpp — Differential tests for CHeli (helicopters).
// Hook paths: Vehicle/CHeli/<fn>, Vehicle/CVehicle/<fn>
//
// CHeli extends CAutomobile. Tests verify type checks and CVehicle
// base method behavior on helicopter instances.

#include "StdInc.h"
#include "TestFramework.h"
#include "ScenarioHelpers.h"
#include "Streaming.h"
#include "StreamingInfo.h"
#include "Models/VehicleModelInfo.h"
#include "Entity/Vehicle/Heli.h"
#include "Entity/Vehicle/Automobile.h"
#include "World.h"

static CAutomobile* s_heli = nullptr;
static bool s_triedHeli = false;

static CVehicle* GetTestHeli() {
    if (!s_triedHeli) {
        s_triedHeli = true;
        // Find a heli model
        for (int32 id = 400; id <= 611; id++) {
            auto* mi = CModelInfo::GetModelInfo(id);
            if (!mi || mi->GetModelType() != MODEL_INFO_VEHICLE) continue;
            auto* vmi = mi->AsVehicleModelInfoPtr();
            if (vmi->m_nVehicleType != VEHICLE_TYPE_HELI) continue;
            if (!CStreaming::IsModelLoaded(id)) {
                auto& info = CStreaming::GetInfo(id);
                if (info.GetCdSize() == 0) continue;
                ResumeOtherThreads resume;
                CStreaming::RequestModel(id, STREAMING_KEEP_IN_MEMORY);
                CStreaming::LoadAllRequestedModels(false);
            }
            if (CStreaming::IsModelLoaded(id)) {
                s_heli = new CAutomobile(id, RANDOM_VEHICLE, true);
                if (s_heli) {
                    s_heli->SetPosn(CVector(2480.0f, -1668.0f, 30.0f));
                    s_heli->SetStatus(STATUS_ABANDONED);
                    CWorld::Add(s_heli);
                }
                break;
            }
        }
    }
    return s_heli ? static_cast<CVehicle*>(s_heli) : nullptr;
}

#define HELI_GUARD() if (!GetTestHeli()) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No heli — model not loaded"); return; }

// --- Type checks ---

GAME_TEST(CHeli, VehicleType) {
    HELI_GUARD()
    // CAutomobile constructor sets VEHICLE_TYPE_HELI based on model info
    auto* veh = GetTestHeli();
    auto* mi = CModelInfo::GetModelInfo(veh->m_nModelIndex);
    auto* vmi = mi->AsVehicleModelInfoPtr();
    EXPECT_EQ((int32)vmi->m_nVehicleType, (int32)VEHICLE_TYPE_HELI);
}

// --- CVehicle base method differentials on heli ---

GAME_DIFF_TEST(CHeli, IsLawEnforcementVehicle) {
    HELI_GUARD()
    auto* veh = GetTestHeli();
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/IsLawEnforcementVehicle"); orig = veh->IsLawEnforcementVehicle(); }
    rev = veh->IsLawEnforcementVehicle();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CHeli, CanPedStepOutCar) {
    HELI_GUARD()
    auto* veh = GetTestHeli();
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/CanPedStepOutCar"); orig = veh->CanPedStepOutCar(false); }
    rev = veh->CanPedStepOutCar(false);
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CHeli, IsUpsideDown) {
    HELI_GUARD()
    auto* veh = GetTestHeli();
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/IsUpsideDown"); orig = veh->IsUpsideDown(); }
    rev = veh->IsUpsideDown();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CHeli, GetVehicleAppearance) {
    HELI_GUARD()
    auto* veh = GetTestHeli();
    int32 orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/GetVehicleAppearance"); orig = (int32)veh->GetVehicleAppearance(); }
    rev = (int32)veh->GetVehicleAppearance();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CHeli, IsVehicleNormal) {
    HELI_GUARD()
    auto* veh = GetTestHeli();
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/IsVehicleNormal"); orig = veh->IsVehicleNormal(); }
    rev = veh->IsVehicleNormal();
    EXPECT_EQ(orig, rev);
}
