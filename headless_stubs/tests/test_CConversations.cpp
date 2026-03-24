// test_CConversations.cpp — Differential tests for CConversations queries.
// Hook paths: Conversations/CConversations/<fn>
// Conversation system state queries.

#include "StdInc.h"
#include "TestFramework.h"
#include "Conversations/Conversations.h"

GAME_DIFF_TEST(CConversations, IsConversationGoingOn) {
    bool orig, rev;
    { HookDisableGuard guard("Conversations/CConversations/IsConversationGoingOn");
      orig = CConversations::IsConversationGoingOn(); }
    rev = CConversations::IsConversationGoingOn();
    EXPECT_EQ(orig, rev);
}

// IsPlayerInPositionForConversation crashes headlessly. Skipped.

// Clear — original crashes (conversation subsystem not initialized headlessly). Skipped.

// EnableConversation/IsConversationAtNode take different args. Skipped.
