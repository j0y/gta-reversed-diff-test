// test_CEventGroup2.cpp — Differential tests for CEventGroup on player ped.
// Hook paths: Events/CEventGroup/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "EventGroup.h"
#include "Ped.h"
#include "PedIntelligence.h"

static CEventGroup* GetPlayerEventGroup() {
    auto* player = FindPlayerPed(0);
    if (!player || !player->GetIntelligence()) return nullptr;
    return &player->GetIntelligence()->m_eventGroup;
}

// --- GetHighestPriorityEvent ---

GAME_DIFF_TEST(CEventGroup2, GetHighestPriorityEvent) {
    auto* eg = GetPlayerEventGroup();
    if (!eg) return;

    CEvent* orig;
    CEvent* rev;
    { HookDisableGuard guard("Events/CEventGroup/GetHighestPriorityEvent");
      orig = eg->GetHighestPriorityEvent(); }
    rev = eg->GetHighestPriorityEvent();
    EXPECT_EQ(orig, rev);
}

// --- HasEventOfType ---
// Test with common event types

GAME_DIFF_TEST(CEventGroup2, HasEventOfType_Various) {
    auto* eg = GetPlayerEventGroup();
    if (!eg) return;

    // Test several event types
    eEventType types[] = {
        EVENT_NONE,
        EVENT_SCRIPT_COMMAND,
        EVENT_DAMAGE,
        EVENT_DEATH,
        EVENT_IN_WATER,
        EVENT_ON_FIRE,
    };
    for (auto type : types) {
        CEvent* orig;
        CEvent* rev;
        { HookDisableGuard guard("Events/CEventGroup/GetEventOfType");
          orig = eg->GetEventOfType(type); }
        rev = eg->GetEventOfType(type);
        EXPECT_EQ(orig, rev);
    }
}

// --- HasScriptCommandOfTaskType ---

GAME_DIFF_TEST(CEventGroup2, HasScriptCommandOfTaskType) {
    auto* eg = GetPlayerEventGroup();
    if (!eg) return;

    eTaskType tasks[] = {
        TASK_NONE,
        TASK_SIMPLE_STAND_STILL,
        TASK_SIMPLE_CAR_DRIVE,
        TASK_COMPLEX_KILL_PED_ON_FOOT,
    };
    for (auto task : tasks) {
        bool orig, rev;
        { HookDisableGuard guard("Events/CEventGroup/HasScriptCommandOfTaskType");
          orig = eg->HasScriptCommandOfTaskType(task); }
        rev = eg->HasScriptCommandOfTaskType(task);
        EXPECT_EQ(orig, rev);
    }
}
