// test_CEvent2.cpp — Differential tests for CEvent subclasses and CEventGroup.
// Hook paths: Events/CEventScriptCommand/<fn>, Events/CEventGroup/<fn>,
//             Events/CEventHandlerHistory/<fn>
//
// CEventDeath/CEventRevived have inline-only virtuals (no hooks).
// CEventScriptCommand has hooked GetEventType, GetEventPriority, AffectsPed, etc.
// CEventGroup and CEventHandlerHistory have various hooked query methods.

#include "StdInc.h"
#include "TestFramework.h"
#include "Events/EventScriptCommand.h"
#include "Events/EventDeath.h"
#include "Events/EventRevived.h"
#include "Events/EventGroup.h"
#include "Events/EventHandlerHistory.h"

// ---- CEventScriptCommand: GetEventType ----

GAME_DIFF_TEST(CEventScriptCommand, GetEventType) {
    // Construct with null task — GetEventType doesn't use it
    CEventScriptCommand evt(0, nullptr, false);
    eEventType orig, rev;
    { HookDisableGuard guard("Events/CEventScriptCommand/GetEventType");
      orig = evt.GetEventType(); }
    rev = evt.GetEventType();
    EXPECT_EQ(orig, rev);
}

// GetEventPriority already tested in test_CEventScriptCommand.cpp

// ---- CEventScriptCommand: AffectsPed with player ----

GAME_DIFF_TEST(CEventScriptCommand, AffectsPed_Player) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    CEventScriptCommand evt(0, nullptr, false);
    bool orig, rev;
    { HookDisableGuard guard("Events/CEventScriptCommand/AffectsPed");
      orig = evt.AffectsPed(player); }
    rev = evt.AffectsPed(player);
    EXPECT_EQ(orig, rev);
}

// ---- CEventScriptCommand: AffectsPed with affectsDeadPeds=true ----

GAME_DIFF_TEST(CEventScriptCommand, AffectsPed_DeadPedsFlag) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    CEventScriptCommand evt(0, nullptr, true);
    bool orig, rev;
    { HookDisableGuard guard("Events/CEventScriptCommand/AffectsPed");
      orig = evt.AffectsPed(player); }
    rev = evt.AffectsPed(player);
    EXPECT_EQ(orig, rev);
}

// ---- CEventScriptCommand: IsValid ----

GAME_DIFF_TEST(CEventScriptCommand, IsValid_Player) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    CEventScriptCommand evt(0, nullptr, false);
    bool orig, rev;
    { HookDisableGuard guard("Events/CEventScriptCommand/IsValid");
      orig = evt.IsValid(player); }
    rev = evt.IsValid(player);
    EXPECT_EQ(orig, rev);
}

// ---- CEventScriptCommand: TakesPriorityOver with CEventDeath ----

GAME_DIFF_TEST(CEventScriptCommand, TakesPriorityOver_Death) {
    CEventScriptCommand scriptEvt(0, nullptr, false);
    CEventDeath deathEvt(false, 0);
    bool orig, rev;
    { HookDisableGuard guard("Events/CEventScriptCommand/TakesPriorityOver");
      orig = scriptEvt.TakesPriorityOver(deathEvt); }
    rev = scriptEvt.TakesPriorityOver(deathEvt);
    EXPECT_EQ(orig, rev);
}

// ---- CEventGroup: GetHighestPriorityEvent on player ----

GAME_DIFF_TEST(CEventGroup, GetHighestPriorityEvent_Player) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    // Player's event group — query the highest-priority event
    auto& group = player->GetEventGroup();
    CEvent* orig;
    CEvent* rev;
    { HookDisableGuard guard("Events/CEventGroup/GetHighestPriorityEvent");
      orig = group.GetHighestPriorityEvent(); }
    rev = group.GetHighestPriorityEvent();
    EXPECT_EQ(orig, rev);
}

// ---- CEventHandlerHistory: GetCurrentEventPriority on player ----

GAME_DIFF_TEST(CEventHandlerHistory, GetCurrentEventPriority_Player) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    auto& history = player->GetEventHandlerHistory();
    int32 orig, rev;
    { HookDisableGuard guard("Events/CEventHandlerHistory/GetCurrentEventPriority");
      orig = history.GetCurrentEventPriority(); }
    rev = history.GetCurrentEventPriority();
    EXPECT_EQ(orig, rev);
}

// ---- CEventHandlerHistory: IsRespondingToEvent ----

GAME_DIFF_TEST(CEventHandlerHistory, IsRespondingToEvent_Death) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    auto& history = player->GetEventHandlerHistory();
    bool orig, rev;
    { HookDisableGuard guard("Events/CEventHandlerHistory/IsRespondingToEvent");
      orig = history.IsRespondingToEvent(EVENT_DEATH); }
    rev = history.IsRespondingToEvent(EVENT_DEATH);
    EXPECT_EQ(orig, rev);
}

// ---- CEventHandlerHistory: TakesPriorityOverCurrentEvent ----

GAME_DIFF_TEST(CEventHandlerHistory, TakesPriorityOverCurrentEvent_Revived) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    auto& history = player->GetEventHandlerHistory();
    CEventRevived reviveEvt;
    bool orig, rev;
    { HookDisableGuard guard("Events/CEventHandlerHistory/TakesPriorityOverCurrentEvent");
      orig = history.TakesPriorityOverCurrentEvent(reviveEvt); }
    rev = history.TakesPriorityOverCurrentEvent(reviveEvt);
    EXPECT_EQ(orig, rev);
}
