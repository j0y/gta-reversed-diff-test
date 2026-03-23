// test_CTaskComplexKillCriminal.cpp

#include "StdInc.h"
#include "TestFramework.h"
#include "Tasks/TaskTypes/TaskComplexKillCriminal.h"
#include "Entity/Ped/Ped.h"
#include "ScenarioHelpers.h"

static Scenario::TestPed* s_criminalPed = nullptr;
static bool s_triedCriminalPed = false;

static CPed* GetCriminalTestPed() {
    if (!s_triedCriminalPed) {
        s_triedCriminalPed = true;
        int32 model = Scenario::FindLoadedPedModel();
        if (model >= 0)
            s_criminalPed = new Scenario::TestPed(PED_TYPE_CIVMALE, model, CVector(2420.0f, -1672.0f, 13.5f));
    }
    return s_criminalPed ? s_criminalPed->AsPed() : nullptr;
}

#define CRIMINAL_GUARD() if (!GetCriminalTestPed()) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No ped"); return; }

GAME_TEST(CTaskComplexKillCriminal, GetTaskType) {
    CRIMINAL_GUARD()
    CTaskComplexKillCriminal task(GetCriminalTestPed());
    EXPECT_EQ((int32)task.GetTaskType(), (int32)TASK_COMPLEX_KILL_CRIMINAL);
}

GAME_TEST(CTaskComplexKillCriminal, IsComplex) {
    CRIMINAL_GUARD()
    CTaskComplexKillCriminal task(GetCriminalTestPed());
    EXPECT_TRUE(task.IsComplex());
}

GAME_TEST(CTaskComplexKillCriminal, Clone_PreservesType) {
    CRIMINAL_GUARD()
    CTaskComplexKillCriminal original(GetCriminalTestPed());
    CTask* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;
    EXPECT_EQ((int32)cloned->GetTaskType(), (int32)TASK_COMPLEX_KILL_CRIMINAL);
    delete cloned;
}
