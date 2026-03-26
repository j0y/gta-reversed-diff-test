// test_CEventEditableResponse.cpp — Differential tests for CEventEditableResponse.
// Hook paths: Events/CEventEditableResponse/<fn>
//
// Test on player ped's event group.

#include "StdInc.h"
#include "TestFramework.h"
#include "Events/EventEditableResponse.h"
#include "PedIntelligence.h"

// --- WillRespond ---

GAME_DIFF_TEST(CEventEditableResponse, WillRespond) {
    auto* player = FindPlayerPed(0);
    if (!player || !player->GetIntelligence()) return;

    auto& eg = player->GetIntelligence()->m_eventGroup;
    auto* event = eg.GetHighestPriorityEvent();
    if (!event) return;

    // Check if it's an editable response event
    auto* editable = dynamic_cast<CEventEditableResponse*>(event);
    if (!editable) return;

    bool orig, rev;
    { HookDisableGuard guard("Events/CEventEditableResponse/WillRespond");
      orig = editable->WillRespond(); }
    rev = editable->WillRespond();
    EXPECT_EQ(orig, rev);
}

// --- HasEditableResponse ---

GAME_DIFF_TEST(CEventEditableResponse, HasEditableResponse) {
    auto* player = FindPlayerPed(0);
    if (!player || !player->GetIntelligence()) return;

    auto& eg = player->GetIntelligence()->m_eventGroup;
    auto* event = eg.GetHighestPriorityEvent();
    if (!event) return;

    auto* editable = dynamic_cast<CEventEditableResponse*>(event);
    if (!editable) return;

    bool orig, rev;
    { HookDisableGuard guard("Events/CEventEditableResponse/HasEditableResponse");
      orig = editable->HasEditableResponse(); }
    rev = editable->HasEditableResponse();
    EXPECT_EQ(orig, rev);
}
