// test_CTaskComplexEnterCarAsDriver.cpp — Tests for CTaskComplexEnterCarAsDriver.
// Hook paths: Tasks/TaskTypes/CTaskComplexEnterCarAsDriver/<fn>
//
// CTaskComplexEnterCarAsDriver handles the multi-step sequence of a ped
// entering a vehicle as the driver. Inherits from CTaskComplexEnterCar.
// Tests verify construction, cloning, and type.
//
// Inspired by gta-reversed issue #828 — errors entering police cars.

#include "StdInc.h"
#include "TestFramework.h"
#include "Tasks/TaskTypes/TaskComplexEnterCarAsDriver.h"
#include "Entity/Vehicle/Automobile.h"
#include "Entity/Ped/Ped.h"
#include "ScenarioHelpers.h"

static Scenario::TestVehicle* s_enterVeh = nullptr;
static bool s_triedEnterVeh = false;

static CVehicle* GetEnterTestVehicle() {
    if (!s_triedEnterVeh) {
        s_triedEnterVeh = true;
        int32 model = Scenario::FindLoadedVehicleModel();
        if (model >= 0) {
            s_enterVeh = new Scenario::TestVehicle(model, CVector(2486.0f, -1668.0f, 13.5f));
        }
    }
    return s_enterVeh ? s_enterVeh->AsVehicle() : nullptr;
}

#define ENTER_GUARD() if (!GetEnterTestVehicle()) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No vehicle"); return; }

// --- Construction & type ---

GAME_TEST(CTaskComplexEnterCarAsDriver, GetTaskType) {
    ENTER_GUARD()
    CTaskComplexEnterCarAsDriver task(GetEnterTestVehicle());
    EXPECT_EQ((int32)task.GetTaskType(), (int32)TASK_COMPLEX_ENTER_CAR_AS_DRIVER);
}

GAME_TEST(CTaskComplexEnterCarAsDriver, IsComplex) {
    ENTER_GUARD()
    CTaskComplexEnterCarAsDriver task(GetEnterTestVehicle());
    EXPECT_FALSE(task.IsSimple());
    EXPECT_TRUE(task.IsComplex());
}

// --- Clone ---

GAME_TEST(CTaskComplexEnterCarAsDriver, Clone_PreservesType) {
    ENTER_GUARD()
    CTaskComplexEnterCarAsDriver original(GetEnterTestVehicle());
    CTask* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;

    EXPECT_EQ((int32)cloned->GetTaskType(), (int32)TASK_COMPLEX_ENTER_CAR_AS_DRIVER);
    EXPECT_TRUE(cloned->IsComplex());

    delete cloned;
}

GAME_TEST(CTaskComplexEnterCarAsDriver, Clone_IsSeparateObject) {
    ENTER_GUARD()
    CTaskComplexEnterCarAsDriver original(GetEnterTestVehicle());
    CTask* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    EXPECT_TRUE((uintptr_t)cloned != (uintptr_t)&original);
    delete cloned;
}

// --- Constructor with move state ---

GAME_TEST(CTaskComplexEnterCarAsDriver, Constructor_WithMoveState) {
    ENTER_GUARD()
    CTaskComplexEnterCarAsDriver task(GetEnterTestVehicle(), PEDMOVE_RUN);
    EXPECT_EQ((int32)task.GetTaskType(), (int32)TASK_COMPLEX_ENTER_CAR_AS_DRIVER);
}
