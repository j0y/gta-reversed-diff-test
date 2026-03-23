// test_CTaskComplexDriveWander.cpp

#include "StdInc.h"
#include "TestFramework.h"
#include "Tasks/TaskTypes/TaskComplexDriveWander.h"
#include "ScenarioHelpers.h"

static Scenario::TestVehicle* s_driveWanderVeh = nullptr;
static bool s_triedDriveWanderVeh = false;

static CVehicle* GetDriveWanderVehicle() {
    if (!s_triedDriveWanderVeh) {
        s_triedDriveWanderVeh = true;
        int32 model = Scenario::FindLoadedVehicleModel();
        if (model >= 0)
            s_driveWanderVeh = new Scenario::TestVehicle(model, CVector(2436.0f, -1668.0f, 13.5f));
    }
    return s_driveWanderVeh ? s_driveWanderVeh->AsVehicle() : nullptr;
}

#define DW_GUARD() if (!GetDriveWanderVehicle()) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No vehicle"); return; }

GAME_TEST(CTaskComplexDriveWander, GetTaskType) {
    DW_GUARD()
    CTaskComplexCarDriveWander task(GetDriveWanderVehicle());
    EXPECT_EQ((int32)task.GetTaskType(), (int32)TASK_COMPLEX_CAR_DRIVE_WANDER);
}

GAME_TEST(CTaskComplexDriveWander, IsComplex) {
    DW_GUARD()
    CTaskComplexCarDriveWander task(GetDriveWanderVehicle());
    EXPECT_TRUE(task.IsComplex());
}

GAME_TEST(CTaskComplexDriveWander, Constructor_DefaultSpeed) {
    DW_GUARD()
    CTaskComplexCarDriveWander task(GetDriveWanderVehicle());
    EXPECT_NEAR(task.m_CruiseSpeed, 10.0f, 1e-5f);
}

GAME_TEST(CTaskComplexDriveWander, Constructor_CustomSpeed) {
    DW_GUARD()
    CTaskComplexCarDriveWander task(GetDriveWanderVehicle(), DRIVING_STYLE_AVOID_CARS, 25.0f);
    EXPECT_NEAR(task.m_CruiseSpeed, 25.0f, 1e-5f);
}
