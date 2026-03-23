// test_CEventInWater.cpp — Tests for CEventInWater.
// CEventInWater extends CEvent. Triggered when ped enters water.

#include "StdInc.h"
#include "TestFramework.h"
#include "Events/EventInWater.h"

// --- Construction & type ---

GAME_TEST(CEventInWater, GetEventType) {
    CEventInWater event(1.0f);
    EXPECT_EQ((int32)event.GetEventType(), (int32)EVENT_IN_WATER);
}

GAME_TEST(CEventInWater, GetEventPriority) {
    CEventInWater event(1.0f);
    EXPECT_EQ(event.GetEventPriority(), 62);
}

GAME_TEST(CEventInWater, GetLifeTime) {
    CEventInWater event(1.0f);
    EXPECT_EQ(event.GetLifeTime(), 0);
}

GAME_TEST(CEventInWater, Constructor_Acceleration) {
    CEventInWater event(2.5f);
    EXPECT_NEAR(event.m_acceleration, 2.5f, 1e-5f);
}

// --- Clone ---

GAME_TEST(CEventInWater, Clone_PreservesAcceleration) {
    CEventInWater original(3.14f);
    CEvent* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;

    EXPECT_EQ((int32)cloned->GetEventType(), (int32)EVENT_IN_WATER);

    auto* c = static_cast<CEventInWater*>(cloned);
    EXPECT_NEAR(c->m_acceleration, 3.14f, 1e-5f);

    delete cloned;
}

GAME_TEST(CEventInWater, Clone_IsSeparateObject) {
    CEventInWater original(1.0f);
    CEvent* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    EXPECT_TRUE((uintptr_t)cloned != (uintptr_t)&original);
    delete cloned;
}

// --- Different acceleration values ---

GAME_TEST(CEventInWater, AccelerationValues) {
    float values[] = { 0.0f, 0.5f, 1.0f, 5.0f, -1.0f };
    for (float val : values) {
        CEventInWater event(val);
        EXPECT_NEAR(event.m_acceleration, val, 1e-5f);
    }
}
