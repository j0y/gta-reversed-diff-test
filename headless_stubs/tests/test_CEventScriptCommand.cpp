// test_CEventScriptCommand.cpp — Tests for CEventScriptCommand.
// CEventScriptCommand extends CEvent. Used by scripts to assign tasks
// to peds via the event system.

#include "StdInc.h"
#include "TestFramework.h"
#include "Events/EventScriptCommand.h"
#include "Tasks/TaskTypes/TaskSimpleStandStill.h"

// --- Construction & type ---

GAME_TEST(CEventScriptCommand, GetEventType) {
    auto* task = new CTaskSimpleStandStill(1000);
    CEventScriptCommand event(0, task, false);
    EXPECT_EQ((int32)event.GetEventType(), (int32)EVENT_SCRIPT_COMMAND);
}

GAME_TEST(CEventScriptCommand, GetLifeTime) {
    auto* task = new CTaskSimpleStandStill(1000);
    CEventScriptCommand event(0, task, false);
    EXPECT_EQ(event.GetLifeTime(), 0);
}

GAME_TEST(CEventScriptCommand, Constructor_Fields) {
    auto* task = new CTaskSimpleStandStill(2000);
    CEventScriptCommand event(3, task, true);
    EXPECT_EQ(event.m_primaryTaskIndex, 3);
    EXPECT_EQ((uintptr_t)event.m_task, (uintptr_t)task);
    EXPECT_TRUE(event.m_affectsDeadPeds);
}

GAME_TEST(CEventScriptCommand, Constructor_DefaultAffectsDeadPeds) {
    auto* task = new CTaskSimpleStandStill(1000);
    CEventScriptCommand event(0, task);
    EXPECT_FALSE(event.m_affectsDeadPeds);
}

// --- Clone ---

GAME_TEST(CEventScriptCommand, Clone_PreservesType) {
    auto* task = new CTaskSimpleStandStill(1000);
    CEventScriptCommand original(2, task, true);
    CEvent* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;

    EXPECT_EQ((int32)cloned->GetEventType(), (int32)EVENT_SCRIPT_COMMAND);

    auto* c = static_cast<CEventScriptCommand*>(cloned);
    EXPECT_EQ(c->m_primaryTaskIndex, 2);
    EXPECT_TRUE(c->m_affectsDeadPeds);
    // The task should be cloned (separate object)
    EXPECT_TRUE(c->m_task != nullptr);
    EXPECT_TRUE((uintptr_t)c->m_task != (uintptr_t)task);

    delete cloned;
}

// --- GetEventPriority differential ---

GAME_DIFF_TEST(CEventScriptCommand, GetEventPriority) {
    auto* task1 = new CTaskSimpleStandStill(1000);
    CEventScriptCommand event(0, task1, false);

    int32 orig, rev;
    { HookDisableGuard guard("Events/CEventScriptCommand/GetEventPriority");
      orig = event.GetEventPriority(); }
    rev = event.GetEventPriority();
    EXPECT_EQ(orig, rev);
}

// --- Different primary task indices ---

GAME_TEST(CEventScriptCommand, PrimaryTaskIndex_Range) {
    int32 indices[] = { 0, 1, 2, 3, 4 };
    for (int32 idx : indices) {
        auto* task = new CTaskSimpleStandStill(1000);
        CEventScriptCommand event(idx, task, false);
        EXPECT_EQ(event.m_primaryTaskIndex, idx);
    }
}
