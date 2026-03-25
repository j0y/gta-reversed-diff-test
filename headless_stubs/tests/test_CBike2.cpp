// test_CBike2.cpp — CBike-specific differential tests.
// Hook paths: Vehicle/CBike/<fn>
// Tests CBike methods that are unique to bikes (not CVehicle base methods).

#include "StdInc.h"
#include "TestFramework.h"
#include "ScenarioHelpers.h"
#include "Streaming.h"
#include "StreamingInfo.h"
#include "Models/VehicleModelInfo.h"
#include "Entity/Vehicle/Bike.h"
#include "World.h"

static CBike* s_bike2 = nullptr;
static bool s_triedBike2 = false;

static CBike* GetTestBike2() {
    if (!s_triedBike2) {
        s_triedBike2 = true;
        for (int32 id = 400; id <= 611; id++) {
            auto* mi = CModelInfo::GetModelInfo(id);
            if (!mi || mi->GetModelType() != MODEL_INFO_VEHICLE) continue;
            auto* vmi = mi->AsVehicleModelInfoPtr();
            if (vmi->m_nVehicleType != VEHICLE_TYPE_BIKE) continue;
            if (!CStreaming::IsModelLoaded(id)) {
                auto& info = CStreaming::GetInfo(id);
                if (info.GetCdSize() == 0) continue;
                ResumeOtherThreads resume;
                CStreaming::RequestModel(id, STREAMING_KEEP_IN_MEMORY);
                CStreaming::LoadAllRequestedModels(false);
            }
            if (CStreaming::IsModelLoaded(id)) {
                s_bike2 = new CBike(id, RANDOM_VEHICLE);
                if (s_bike2) {
                    s_bike2->SetPosn(CVector(2490.0f, -1660.0f, 13.5f));
                    s_bike2->SetStatus(STATUS_ABANDONED);
                    CWorld::Add(s_bike2);
                }
                break;
            }
        }
    }
    return s_bike2;
}

#define BIKE2_GUARD() if (!GetTestBike2()) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No bike model loaded"); return; }

// --- GetAllWheelsOffGround ---

GAME_DIFF_TEST(CBike2, GetAllWheelsOffGround) {
    BIKE2_GUARD()
    auto* bike = GetTestBike2();
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CBike/GetAllWheelsOffGround"); orig = bike->GetAllWheelsOffGround(); }
    rev = bike->GetAllWheelsOffGround();
    EXPECT_EQ(orig, rev);
}

// --- CVehicle queries specific to bike context ---

GAME_DIFF_TEST(CBike2, CanBeDriven) {
    BIKE2_GUARD()
    auto* veh = static_cast<CVehicle*>(GetTestBike2());
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/CanBeDriven"); orig = veh->CanBeDriven(); }
    rev = veh->CanBeDriven();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CBike2, CanPedEnterCar) {
    BIKE2_GUARD()
    auto* veh = static_cast<CVehicle*>(GetTestBike2());
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/CanPedEnterCar"); orig = veh->CanPedEnterCar(); }
    rev = veh->CanPedEnterCar();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CBike2, CanPedLeanOut) {
    BIKE2_GUARD()
    auto* veh = static_cast<CVehicle*>(GetTestBike2());
    auto* player = FindPlayerPed(0);
    if (!player) return;
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/CanPedLeanOut"); orig = veh->CanPedLeanOut(player); }
    rev = veh->CanPedLeanOut(player);
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CBike2, UsesSiren) {
    BIKE2_GUARD()
    auto* veh = static_cast<CVehicle*>(GetTestBike2());
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/UsesSiren"); orig = veh->UsesSiren(); }
    rev = veh->UsesSiren();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CBike2, CanDoorsBeDamaged) {
    BIKE2_GUARD()
    auto* veh = static_cast<CVehicle*>(GetTestBike2());
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/CanDoorsBeDamaged"); orig = veh->CanDoorsBeDamaged(); }
    rev = veh->CanDoorsBeDamaged();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CBike2, GetRemapIndex) {
    BIKE2_GUARD()
    auto* veh = static_cast<CVehicle*>(GetTestBike2());
    int32 orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/GetRemapIndex"); orig = veh->GetRemapIndex(); }
    rev = veh->GetRemapIndex();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CBike2, GetPlaneNumGuns) {
    BIKE2_GUARD()
    auto* veh = static_cast<CVehicle*>(GetTestBike2());
    int32 orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/GetPlaneNumGuns"); orig = veh->GetPlaneNumGuns(); }
    rev = veh->GetPlaneNumGuns();
    EXPECT_EQ(orig, rev);
}
