// test_CTaskComplexSequence.cpp — Differential tests for CTaskComplexSequence.
// Hook paths: Tasks/CTaskComplexSequence/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "Tasks/TaskComplexSequence.h"
#include "Tasks/TaskTypes/TaskSimpleStandStill.h"
#include "Tasks/TaskTypes/TaskSimplePause.h"

// --- Clone ---
// Create a sequence, clone it, verify clone matches.

GAME_DIFF_TEST(CTaskComplexSequence, Clone) {
    CTaskComplexSequence seq;

    // Clone via original
    CTask* origClone;
    { HookDisableGuard guard("Tasks/CTaskComplexSequence/Clone");
      origClone = seq.Clone(); }

    // Clone via reversed
    CTask* revClone = seq.Clone();

    // Both should produce non-null clones
    EXPECT_TRUE(origClone != nullptr);
    EXPECT_TRUE(revClone != nullptr);

    if (origClone) {
        EXPECT_EQ(origClone->GetTaskType(), revClone->GetTaskType());
        delete origClone;
    }
    if (revClone) delete revClone;
}

// Contains and Flush crash in original code when called on a
// standalone (non-ped-attached) CTaskComplexSequence. The original
// code dereferences internal pointers that are only valid when the
// task is managed by the ped task system. Skipped.
