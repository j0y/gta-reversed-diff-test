// test_CBoat2.cpp — Extended CBoat differential tests on spawned boat.
// Hook paths: Vehicle/CBoat/<fn>, Vehicle/CVehicle/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "ScenarioHelpers.h"
#include "Streaming.h"
#include "StreamingInfo.h"
#include "Models/VehicleModelInfo.h"
#include "Entity/Vehicle/Boat.h"
#include "World.h"

static CBoat* s_boat2 = nullptr;
static bool s_triedBoat2 = false;

static CBoat* GetTestBoat() {
    if (!s_triedBoat2) {
        s_triedBoat2 = true;
        for (int32 id = 400; id <= 611; id++) {
            auto* mi = CModelInfo::GetModelInfo(id);
            if (!mi || mi->GetModelType() != MODEL_INFO_VEHICLE) continue;
            auto* vmi = mi->AsVehicleModelInfoPtr();
            if (vmi->m_nVehicleType != VEHICLE_TYPE_BOAT) continue;
            if (!CStreaming::IsModelLoaded(id)) {
                auto& info = CStreaming::GetInfo(id);
                if (info.GetCdSize() == 0) continue;
                ResumeOtherThreads resume;
                CStreaming::RequestModel(id, STREAMING_KEEP_IN_MEMORY);
                CStreaming::LoadAllRequestedModels(false);
            }
            if (CStreaming::IsModelLoaded(id)) {
                s_boat2 = new CBoat(id, RANDOM_VEHICLE);
                if (s_boat2) {
                    s_boat2->SetPosn(CVector(370.0f, -1840.0f, 3.0f));
                    s_boat2->SetStatus(STATUS_ABANDONED);
                    CWorld::Add(s_boat2);
                }
                break;
            }
        }
    }
    return s_boat2;
}

#define BOAT_GUARD() if (!GetTestBoat()) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No boat model loaded"); return; }

// --- CBoat type check ---

GAME_TEST(CBoat2, VehicleType) {
    BOAT_GUARD()
    EXPECT_EQ((int32)GetTestBoat()->m_nVehicleType, (int32)VEHICLE_TYPE_BOAT);
}

// --- CVehicle base methods on boat ---

GAME_DIFF_TEST(CBoat2, IsLawEnforcementVehicle) {
    BOAT_GUARD()
    auto* veh = static_cast<CVehicle*>(GetTestBoat());
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/IsLawEnforcementVehicle"); orig = veh->IsLawEnforcementVehicle(); }
    rev = veh->IsLawEnforcementVehicle();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CBoat2, IsUpsideDown) {
    BOAT_GUARD()
    auto* veh = static_cast<CVehicle*>(GetTestBoat());
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/IsUpsideDown"); orig = veh->IsUpsideDown(); }
    rev = veh->IsUpsideDown();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CBoat2, IsOnItsSide) {
    BOAT_GUARD()
    auto* veh = static_cast<CVehicle*>(GetTestBoat());
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/IsOnItsSide"); orig = veh->IsOnItsSide(); }
    rev = veh->IsOnItsSide();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CBoat2, GetVehicleAppearance) {
    BOAT_GUARD()
    auto* veh = static_cast<CVehicle*>(GetTestBoat());
    int32 orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/GetVehicleAppearance"); orig = (int32)veh->GetVehicleAppearance(); }
    rev = (int32)veh->GetVehicleAppearance();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CBoat2, CanBeDriven) {
    BOAT_GUARD()
    auto* veh = static_cast<CVehicle*>(GetTestBoat());
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/CanBeDriven"); orig = veh->CanBeDriven(); }
    rev = veh->CanBeDriven();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CBoat2, CanPedEnterCar) {
    BOAT_GUARD()
    auto* veh = static_cast<CVehicle*>(GetTestBoat());
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/CanPedEnterCar"); orig = veh->CanPedEnterCar(); }
    rev = veh->CanPedEnterCar();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CBoat2, CanBeDeleted) {
    BOAT_GUARD()
    auto* veh = static_cast<CVehicle*>(GetTestBoat());
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/CanBeDeleted"); orig = veh->CanBeDeleted(); }
    rev = veh->CanBeDeleted();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CBoat2, IsVehicleNormal) {
    BOAT_GUARD()
    auto* veh = static_cast<CVehicle*>(GetTestBoat());
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/IsVehicleNormal"); orig = veh->IsVehicleNormal(); }
    rev = veh->IsVehicleNormal();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CBoat2, CanPedStepOutCar) {
    BOAT_GUARD()
    auto* veh = static_cast<CVehicle*>(GetTestBoat());
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/CanPedStepOutCar"); orig = veh->CanPedStepOutCar(false); }
    rev = veh->CanPedStepOutCar(false);
    EXPECT_EQ(orig, rev);
}

// --- CBoat-specific wake queries ---

GAME_DIFF_TEST(CBoat2, IsSectorAffectedByWake_NearBoat) {
    BOAT_GUARD()
    CVector2D boatPos(GetTestBoat()->GetPosition().x, GetTestBoat()->GetPosition().y);
    CBoat* boats[4] = {};
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CBoat/IsSectorAffectedByWake"); orig = CBoat::IsSectorAffectedByWake(boatPos, 100.f, boats); }
    rev = CBoat::IsSectorAffectedByWake(boatPos, 100.f, boats);
    EXPECT_EQ(orig, rev);
}
