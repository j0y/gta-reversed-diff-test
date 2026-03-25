// test_CHeli2.cpp — Extended CHeli differential tests.
// Hook paths: Vehicle/CHeli/<fn>, Vehicle/CVehicle/<fn>
// Tests heli-specific static functions and more CVehicle queries on heli.

#include "StdInc.h"
#include "TestFramework.h"
#include "ScenarioHelpers.h"
#include "Streaming.h"
#include "StreamingInfo.h"
#include "Models/VehicleModelInfo.h"
#include "Entity/Vehicle/Heli.h"
#include "Entity/Vehicle/Automobile.h"
#include "World.h"

static CAutomobile* s_heli2 = nullptr;
static bool s_triedHeli2 = false;

static CVehicle* GetTestHeli2() {
    if (!s_triedHeli2) {
        s_triedHeli2 = true;
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
                s_heli2 = new CAutomobile(id, RANDOM_VEHICLE, true);
                if (s_heli2) {
                    s_heli2->SetPosn(CVector(2476.0f, -1660.0f, 30.0f));
                    s_heli2->SetStatus(STATUS_ABANDONED);
                    CWorld::Add(s_heli2);
                }
                break;
            }
        }
    }
    return s_heli2 ? static_cast<CVehicle*>(s_heli2) : nullptr;
}

#define HELI2_GUARD() if (!GetTestHeli2()) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No heli model loaded"); return; }

// --- CHeli static functions ---

GAME_TEST(CHeli2, InitHelis_NoCrash) {
    CHeli::InitHelis();
    EXPECT_TRUE(true);
}

GAME_TEST(CHeli2, SwitchPoliceHelis_Off) {
    CHeli::SwitchPoliceHelis(false);
    EXPECT_TRUE(true);
}

GAME_TEST(CHeli2, SwitchPoliceHelis_On) {
    CHeli::SwitchPoliceHelis(true);
    EXPECT_TRUE(true);
}

// --- More CVehicle queries on heli ---

GAME_DIFF_TEST(CHeli2, CanBeDriven) {
    HELI2_GUARD()
    auto* veh = GetTestHeli2();
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/CanBeDriven"); orig = veh->CanBeDriven(); }
    rev = veh->CanBeDriven();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CHeli2, CanPedEnterCar) {
    HELI2_GUARD()
    auto* veh = GetTestHeli2();
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/CanPedEnterCar"); orig = veh->CanPedEnterCar(); }
    rev = veh->CanPedEnterCar();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CHeli2, CanDoorsBeDamaged) {
    HELI2_GUARD()
    auto* veh = GetTestHeli2();
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/CanDoorsBeDamaged"); orig = veh->CanDoorsBeDamaged(); }
    rev = veh->CanDoorsBeDamaged();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CHeli2, CanBeDeleted) {
    HELI2_GUARD()
    auto* veh = GetTestHeli2();
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/CanBeDeleted"); orig = veh->CanBeDeleted(); }
    rev = veh->CanBeDeleted();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CHeli2, IsOnItsSide) {
    HELI2_GUARD()
    auto* veh = GetTestHeli2();
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/IsOnItsSide"); orig = veh->IsOnItsSide(); }
    rev = veh->IsOnItsSide();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CHeli2, GetRemapIndex) {
    HELI2_GUARD()
    auto* veh = GetTestHeli2();
    int32 orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/GetRemapIndex"); orig = veh->GetRemapIndex(); }
    rev = veh->GetRemapIndex();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CHeli2, GetPlaneNumGuns) {
    HELI2_GUARD()
    auto* veh = GetTestHeli2();
    int32 orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/GetPlaneNumGuns"); orig = veh->GetPlaneNumGuns(); }
    rev = veh->GetPlaneNumGuns();
    EXPECT_EQ(orig, rev);
}

// --- CAutomobile queries on heli (CHeli inherits CAutomobile) ---

GAME_DIFF_TEST(CHeli2, GetAllWheelsOffGround) {
    HELI2_GUARD()
    auto* car = static_cast<CAutomobile*>(s_heli2);
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CAutomobile/GetAllWheelsOffGround"); orig = car->GetAllWheelsOffGround(); }
    rev = car->GetAllWheelsOffGround();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CHeli2, GetCarRoll) {
    HELI2_GUARD()
    auto* car = static_cast<CAutomobile*>(s_heli2);
    float orig, rev;
    { HookDisableGuard guard("Vehicle/CAutomobile/GetCarRoll"); orig = car->GetCarRoll(); }
    rev = car->GetCarRoll();
    EXPECT_NEAR(orig, rev, 1e-5f);
}
