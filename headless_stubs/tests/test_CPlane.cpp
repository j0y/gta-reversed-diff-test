// test_CPlane.cpp — Differential tests for CPlane (airplanes).
// Hook paths: Vehicle/CPlane/<fn>, Vehicle/CVehicle/<fn>
//
// CPlane extends CAutomobile. Tests verify type checks and CVehicle
// base method behavior on plane instances.

#include "StdInc.h"
#include "TestFramework.h"
#include "ScenarioHelpers.h"
#include "Streaming.h"
#include "StreamingInfo.h"
#include "Models/VehicleModelInfo.h"
#include "Entity/Vehicle/Plane.h"
#include "Entity/Vehicle/Automobile.h"
#include "World.h"

static CAutomobile* s_plane = nullptr;
static bool s_triedPlane = false;

static CVehicle* GetTestPlane() {
    if (!s_triedPlane) {
        s_triedPlane = true;
        for (int32 id = 400; id <= 611; id++) {
            auto* mi = CModelInfo::GetModelInfo(id);
            if (!mi || mi->GetModelType() != MODEL_INFO_VEHICLE) continue;
            auto* vmi = mi->AsVehicleModelInfoPtr();
            if (vmi->m_nVehicleType != VEHICLE_TYPE_PLANE) continue;
            if (!CStreaming::IsModelLoaded(id)) {
                auto& info = CStreaming::GetInfo(id);
                if (info.GetCdSize() == 0) continue;
                ResumeOtherThreads resume;
                CStreaming::RequestModel(id, STREAMING_KEEP_IN_MEMORY);
                CStreaming::LoadAllRequestedModels(false);
            }
            if (CStreaming::IsModelLoaded(id)) {
                s_plane = new CAutomobile(id, RANDOM_VEHICLE, true);
                if (s_plane) {
                    s_plane->SetPosn(CVector(2478.0f, -1668.0f, 30.0f));
                    s_plane->SetStatus(STATUS_ABANDONED);
                    CWorld::Add(s_plane);
                }
                break;
            }
        }
    }
    return s_plane ? static_cast<CVehicle*>(s_plane) : nullptr;
}

#define PLANE_GUARD() if (!GetTestPlane()) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No plane — model not loaded"); return; }

// --- Type checks ---

GAME_TEST(CPlane, VehicleType) {
    PLANE_GUARD()
    // CAutomobile constructor sets VEHICLE_TYPE_PLANE based on model info
    auto* veh = GetTestPlane();
    auto* mi = CModelInfo::GetModelInfo(veh->m_nModelIndex);
    auto* vmi = mi->AsVehicleModelInfoPtr();
    EXPECT_EQ((int32)vmi->m_nVehicleType, (int32)VEHICLE_TYPE_PLANE);
}

// --- CVehicle base method differentials on plane ---

GAME_DIFF_TEST(CPlane, IsLawEnforcementVehicle) {
    PLANE_GUARD()
    auto* veh = GetTestPlane();
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/IsLawEnforcementVehicle"); orig = veh->IsLawEnforcementVehicle(); }
    rev = veh->IsLawEnforcementVehicle();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CPlane, CanPedStepOutCar) {
    PLANE_GUARD()
    auto* veh = GetTestPlane();
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/CanPedStepOutCar"); orig = veh->CanPedStepOutCar(false); }
    rev = veh->CanPedStepOutCar(false);
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CPlane, IsUpsideDown) {
    PLANE_GUARD()
    auto* veh = GetTestPlane();
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/IsUpsideDown"); orig = veh->IsUpsideDown(); }
    rev = veh->IsUpsideDown();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CPlane, GetVehicleAppearance) {
    PLANE_GUARD()
    auto* veh = GetTestPlane();
    int32 orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/GetVehicleAppearance"); orig = (int32)veh->GetVehicleAppearance(); }
    rev = (int32)veh->GetVehicleAppearance();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CPlane, IsVehicleNormal) {
    PLANE_GUARD()
    auto* veh = GetTestPlane();
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/IsVehicleNormal"); orig = veh->IsVehicleNormal(); }
    rev = veh->IsVehicleNormal();
    EXPECT_EQ(orig, rev);
}
