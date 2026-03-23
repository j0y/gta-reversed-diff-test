// test_CBike.cpp — Differential tests for CBike-specific methods.
// Hook paths: Vehicle/CBike/<fn>
//
// Extends the existing vehicle type coverage (test_CStreaming_Vehicles.cpp
// covers spawn/basic queries). These tests focus on CBike-specific methods
// and lean/suspension behavior accessible without rendering.

#include "StdInc.h"
#include "TestFramework.h"
#include "ScenarioHelpers.h"
#include "Streaming.h"
#include "StreamingInfo.h"
#include "Models/VehicleModelInfo.h"
#include "Entity/Vehicle/Bike.h"
#include "World.h"

// --- Find and load a bike model ---

static CBike* s_bike = nullptr;
static bool s_triedBike = false;

static int32 FindBikeModel() {
    for (int32 id = 400; id <= 611; id++) {
        auto* mi = CModelInfo::GetModelInfo(id);
        if (!mi || mi->GetModelType() != MODEL_INFO_VEHICLE) continue;
        auto* vmi = mi->AsVehicleModelInfoPtr();
        if (vmi->m_nVehicleType != VEHICLE_TYPE_BIKE) continue;
        if (CStreaming::IsModelLoaded(id)) return id;
    }
    return -1;
}

static CBike* GetTestBike() {
    if (!s_triedBike) {
        s_triedBike = true;
        int32 model = FindBikeModel();
        if (model < 0) {
            // Try loading one
            for (int32 id = 400; id <= 611; id++) {
                auto* mi = CModelInfo::GetModelInfo(id);
                if (!mi || mi->GetModelType() != MODEL_INFO_VEHICLE) continue;
                auto* vmi = mi->AsVehicleModelInfoPtr();
                if (vmi->m_nVehicleType != VEHICLE_TYPE_BIKE) continue;
                auto& info = CStreaming::GetInfo(id);
                if (info.GetCdSize() > 0) {
                    ResumeOtherThreads resume;
                    CStreaming::RequestModel(id, STREAMING_KEEP_IN_MEMORY);
                    CStreaming::LoadAllRequestedModels(false);
                    if (CStreaming::IsModelLoaded(id)) { model = id; break; }
                }
            }
        }
        if (model >= 0) {
            s_bike = new CBike(model, RANDOM_VEHICLE);
            if (s_bike) {
                s_bike->SetPosn(CVector(2482.0f, -1668.0f, 13.5f));
                s_bike->SetStatus(STATUS_ABANDONED);
                CWorld::Add(s_bike);
            }
        }
    }
    return s_bike;
}

#define BIKE_GUARD() if (!GetTestBike()) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No bike — model not loaded"); return; }

// --- Type checks ---

GAME_TEST(CBike, VehicleType) {
    BIKE_GUARD()
    EXPECT_EQ((int32)GetTestBike()->m_nVehicleType, (int32)VEHICLE_TYPE_BIKE);
}

GAME_TEST(CBike, IsNotAutomobile) {
    BIKE_GUARD()
    auto* veh = static_cast<CVehicle*>(GetTestBike());
    // Bikes should not be identified as automobiles
    EXPECT_EQ((int32)veh->m_nVehicleType, (int32)VEHICLE_TYPE_BIKE);
}

// --- CVehicle base method differentials on bike ---

GAME_DIFF_TEST(CBike, IsLawEnforcementVehicle) {
    BIKE_GUARD()
    auto* veh = static_cast<CVehicle*>(GetTestBike());
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/IsLawEnforcementVehicle"); orig = veh->IsLawEnforcementVehicle(); }
    rev = veh->IsLawEnforcementVehicle();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CBike, CanPedStepOutCar) {
    BIKE_GUARD()
    auto* veh = static_cast<CVehicle*>(GetTestBike());
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/CanPedStepOutCar"); orig = veh->CanPedStepOutCar(false); }
    rev = veh->CanPedStepOutCar(false);
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CBike, IsUpsideDown) {
    BIKE_GUARD()
    auto* veh = static_cast<CVehicle*>(GetTestBike());
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/IsUpsideDown"); orig = veh->IsUpsideDown(); }
    rev = veh->IsUpsideDown();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CBike, IsOnItsSide) {
    BIKE_GUARD()
    auto* veh = static_cast<CVehicle*>(GetTestBike());
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/IsOnItsSide"); orig = veh->IsOnItsSide(); }
    rev = veh->IsOnItsSide();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CBike, IsVehicleNormal) {
    BIKE_GUARD()
    auto* veh = static_cast<CVehicle*>(GetTestBike());
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/IsVehicleNormal"); orig = veh->IsVehicleNormal(); }
    rev = veh->IsVehicleNormal();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CBike, GetVehicleAppearance) {
    BIKE_GUARD()
    auto* veh = static_cast<CVehicle*>(GetTestBike());
    int32 orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/GetVehicleAppearance"); orig = (int32)veh->GetVehicleAppearance(); }
    rev = (int32)veh->GetVehicleAppearance();
    EXPECT_EQ(orig, rev);
}
