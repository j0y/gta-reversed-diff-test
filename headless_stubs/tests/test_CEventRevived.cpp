// test_CEventRevived.cpp — Tests for CEventRevived.
// Simplest possible event — no members, just type/priority/clone.

#include "StdInc.h"
#include "TestFramework.h"
#include "Events/EventRevived.h"

GAME_TEST(CEventRevived, GetEventType) {
    CEventRevived event;
    EXPECT_EQ((int32)event.GetEventType(), (int32)EVENT_REVIVE);
}

GAME_TEST(CEventRevived, GetEventPriority) {
    CEventRevived event;
    EXPECT_EQ(event.GetEventPriority(), 74);
}

GAME_TEST(CEventRevived, GetLifeTime) {
    CEventRevived event;
    EXPECT_EQ(event.GetLifeTime(), 0);
}

GAME_TEST(CEventRevived, Clone_PreservesType) {
    CEventRevived original;
    CEvent* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;
    EXPECT_EQ((int32)cloned->GetEventType(), (int32)EVENT_REVIVE);
    delete cloned;
}

GAME_TEST(CEventRevived, Clone_IsSeparateObject) {
    CEventRevived original;
    CEvent* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    EXPECT_TRUE((uintptr_t)cloned != (uintptr_t)&original);
    delete cloned;
}
