// test_CEventFireNearby.cpp — Tests for CEventFireNearby.
// CEventFireNearby extends CEventEditableResponse. Position-based event
// triggered when fire is detected near a ped.

#include "StdInc.h"
#include "TestFramework.h"
#include "Events/EventFireNearby.h"

// --- Construction & type ---

GAME_TEST(CEventFireNearby, GetEventType) {
    CVector pos(100.0f, 200.0f, 10.0f);
    CEventFireNearby event(pos);
    EXPECT_EQ((int32)event.GetEventType(), (int32)EVENT_FIRE_NEARBY);
}

GAME_TEST(CEventFireNearby, GetEventPriority) {
    CVector pos(0.0f, 0.0f, 0.0f);
    CEventFireNearby event(pos);
    EXPECT_EQ(event.GetEventPriority(), 17);
}

GAME_TEST(CEventFireNearby, GetLifeTime) {
    CVector pos(0.0f, 0.0f, 0.0f);
    CEventFireNearby event(pos);
    EXPECT_EQ(event.GetLifeTime(), 0);
}

GAME_TEST(CEventFireNearby, Constructor_Position) {
    CVector pos(50.0f, 75.0f, 12.5f);
    CEventFireNearby event(pos);
    EXPECT_NEAR(event.m_position.x, 50.0f, 1e-5f);
    EXPECT_NEAR(event.m_position.y, 75.0f, 1e-5f);
    EXPECT_NEAR(event.m_position.z, 12.5f, 1e-5f);
}

GAME_TEST(CEventFireNearby, TakesPriorityOver_AlwaysTrue) {
    CVector pos(0.0f, 0.0f, 0.0f);
    CEventFireNearby event1(pos);
    CEventFireNearby event2(pos);
    EXPECT_TRUE(event1.TakesPriorityOver(event2));
}

// --- Clone (CloneEditable) ---

GAME_TEST(CEventFireNearby, CloneEditable_PreservesPosition) {
    CVector pos(123.0f, 456.0f, 78.9f);
    CEventFireNearby original(pos);
    auto* cloned = original.CloneEditable();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;

    EXPECT_EQ((int32)cloned->GetEventType(), (int32)EVENT_FIRE_NEARBY);

    auto* c = static_cast<CEventFireNearby*>(cloned);
    EXPECT_NEAR(c->m_position.x, 123.0f, 1e-5f);
    EXPECT_NEAR(c->m_position.y, 456.0f, 1e-5f);
    EXPECT_NEAR(c->m_position.z, 78.9f, 1e-5f);

    delete cloned;
}
