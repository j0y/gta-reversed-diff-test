// test_CTaskComplexSignalAtPed.cpp

#include "StdInc.h"
#include "TestFramework.h"
#include "Tasks/TaskTypes/TaskComplexSignalAtPed.h"
#include "Entity/Ped/Ped.h"
#include "ScenarioHelpers.h"

static Scenario::TestPed* s_signalPed = nullptr;
static bool s_triedSignalPed = false;

static CPed* GetSignalTestPed() {
    if (!s_triedSignalPed) {
        s_triedSignalPed = true;
        int32 model = Scenario::FindLoadedPedModel();
        if (model >= 0)
            s_signalPed = new Scenario::TestPed(PED_TYPE_CIVMALE, model, CVector(2438.0f, -1672.0f, 13.5f));
    }
    return s_signalPed ? s_signalPed->AsPed() : nullptr;
}

#define SIGNAL_GUARD() if (!GetSignalTestPed()) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No ped"); return; }

GAME_TEST(CTaskComplexSignalAtPed, GetTaskType) {
    SIGNAL_GUARD()
    CTaskComplexSignalAtPed task(GetSignalTestPed());
    EXPECT_EQ((int32)task.GetTaskType(), (int32)TASK_COMPLEX_SIGNAL_AT_PED);
}

GAME_TEST(CTaskComplexSignalAtPed, IsComplex) {
    SIGNAL_GUARD()
    CTaskComplexSignalAtPed task(GetSignalTestPed());
    EXPECT_TRUE(task.IsComplex());
}

GAME_TEST(CTaskComplexSignalAtPed, Constructor_Fields) {
    SIGNAL_GUARD()
    auto* ped = GetSignalTestPed();
    CTaskComplexSignalAtPed task(ped, -1, true);
    EXPECT_EQ((uintptr_t)task.m_pedToSignalAt, (uintptr_t)ped);
    EXPECT_TRUE(task.m_playAnimAtEnd);
}

GAME_TEST(CTaskComplexSignalAtPed, Clone_PreservesType) {
    SIGNAL_GUARD()
    CTaskComplexSignalAtPed original(GetSignalTestPed());
    CTask* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;
    EXPECT_EQ((int32)cloned->GetTaskType(), (int32)TASK_COMPLEX_SIGNAL_AT_PED);
    delete cloned;
}
