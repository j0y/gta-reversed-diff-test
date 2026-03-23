// test_CTaskComplexKillPedFromBoat.cpp

#include "StdInc.h"
#include "TestFramework.h"
#include "Tasks/TaskTypes/TaskComplexKillPedFromBoat.h"
#include "Entity/Ped/Ped.h"
#include "ScenarioHelpers.h"

static Scenario::TestPed* s_boatKillPed = nullptr;
static bool s_triedBoatKillPed = false;

static CPed* GetBoatKillTestPed() {
    if (!s_triedBoatKillPed) {
        s_triedBoatKillPed = true;
        int32 model = Scenario::FindLoadedPedModel();
        if (model >= 0)
            s_boatKillPed = new Scenario::TestPed(PED_TYPE_CIVMALE, model, CVector(2424.0f, -1672.0f, 13.5f));
    }
    return s_boatKillPed ? s_boatKillPed->AsPed() : nullptr;
}

#define BOAT_KILL_GUARD() if (!GetBoatKillTestPed()) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No ped"); return; }

GAME_TEST(CTaskComplexKillPedFromBoat, GetTaskType) {
    BOAT_KILL_GUARD()
    CTaskComplexKillPedFromBoat task(GetBoatKillTestPed());
    EXPECT_EQ((int32)task.GetTaskType(), (int32)TASK_KILL_PED_FROM_BOAT);
}

GAME_TEST(CTaskComplexKillPedFromBoat, IsComplex) {
    BOAT_KILL_GUARD()
    CTaskComplexKillPedFromBoat task(GetBoatKillTestPed());
    EXPECT_TRUE(task.IsComplex());
}

GAME_TEST(CTaskComplexKillPedFromBoat, Constructor_Target) {
    BOAT_KILL_GUARD()
    auto* ped = GetBoatKillTestPed();
    CTaskComplexKillPedFromBoat task(ped);
    EXPECT_EQ((uintptr_t)task.m_Ped, (uintptr_t)ped);
}

GAME_TEST(CTaskComplexKillPedFromBoat, Clone_PreservesType) {
    BOAT_KILL_GUARD()
    CTaskComplexKillPedFromBoat original(GetBoatKillTestPed());
    CTask* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;
    EXPECT_EQ((int32)cloned->GetTaskType(), (int32)TASK_KILL_PED_FROM_BOAT);
    delete cloned;
}
