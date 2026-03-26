// test_CTaskSimpleUseGun.cpp — Differential tests for CTaskSimpleUseGun.
// Hook paths: Tasks/TaskTypes/CTaskSimpleUseGun/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "Tasks/TaskTypes/TaskSimpleUseGun.h"

// --- GetTaskType ---

GAME_DIFF_TEST(CTaskSimpleUseGun, GetTaskType) {
    CVector targetPos(0.f, 0.f, 0.f);
    CTaskSimpleUseGun task(nullptr, targetPos, eGunCommand::AIM, 1, false);

    eTaskType orig, rev;
    { HookDisableGuard guard("Tasks/TaskTypes/CTaskSimpleUseGun/GetTaskType");
      orig = task.GetTaskType(); }
    rev = task.GetTaskType();
    EXPECT_EQ((int)orig, (int)rev);
}

// --- Clone ---

GAME_DIFF_TEST(CTaskSimpleUseGun, Clone) {
    CVector targetPos(100.f, 200.f, 10.f);
    CTaskSimpleUseGun task(nullptr, targetPos, eGunCommand::AIM, 3, true);

    CTask* origClone;
    { HookDisableGuard guard("Tasks/TaskTypes/CTaskSimpleUseGun/Clone");
      origClone = task.Clone(); }

    CTask* revClone = task.Clone();

    EXPECT_TRUE(origClone != nullptr);
    EXPECT_TRUE(revClone != nullptr);

    if (origClone && revClone) {
        EXPECT_EQ(origClone->GetTaskType(), revClone->GetTaskType());
    }
    delete origClone;
    delete revClone;
}

// --- MakeAbortable ---

GAME_DIFF_TEST(CTaskSimpleUseGun, MakeAbortable) {
    auto* player = FindPlayerPed(0);
    if (!player) return;

    CVector targetPos(0.f, 0.f, 0.f);
    CTaskSimpleUseGun task(nullptr, targetPos, eGunCommand::AIM, 1, false);

    bool orig, rev;
    { HookDisableGuard guard("Tasks/TaskTypes/CTaskSimpleUseGun/MakeAbortable");
      orig = task.MakeAbortable(player, ABORT_PRIORITY_URGENT, nullptr); }

    CTaskSimpleUseGun task2(nullptr, targetPos, eGunCommand::AIM, 1, false);
    rev = task2.MakeAbortable(player, ABORT_PRIORITY_URGENT, nullptr);

    EXPECT_EQ(orig, rev);
}
