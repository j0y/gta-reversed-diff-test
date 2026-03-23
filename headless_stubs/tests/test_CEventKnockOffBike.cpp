// test_CEventKnockOffBike.cpp — Tests for CEventKnockOffBike.
// Triggered when a ped is knocked off a bike/motorcycle.

#include "StdInc.h"
#include "TestFramework.h"
#include "Events/EventKnockOffBike.h"

// --- Default constructor ---

GAME_TEST(CEventKnockOffBike, GetEventType_Default) {
    CEventKnockOffBike event;
    EXPECT_EQ((int32)event.GetEventType(), (int32)EVENT_KNOCK_OFF_BIKE);
}

GAME_TEST(CEventKnockOffBike, GetEventPriority) {
    CEventKnockOffBike event;
    EXPECT_EQ(event.GetEventPriority(), 70);
}

GAME_TEST(CEventKnockOffBike, GetLifeTime) {
    CEventKnockOffBike event;
    EXPECT_EQ(event.GetLifeTime(), 0);
}

GAME_TEST(CEventKnockOffBike, GetLocalSoundLevel) {
    CEventKnockOffBike event;
    EXPECT_NEAR(event.GetLocalSoundLevel(), 60.0f, 1e-5f);
}

GAME_TEST(CEventKnockOffBike, IsCriminalEvent_AlwaysTrue) {
    CEventKnockOffBike event;
    EXPECT_TRUE(event.IsCriminalEvent());
}

// --- Full constructor ---

GAME_TEST(CEventKnockOffBike, Constructor_Fields) {
    CVector moveSpeed(1.0f, 2.0f, 3.0f);
    CVector impactVel(4.0f, 5.0f, 6.0f);
    CEventKnockOffBike event(nullptr, moveSpeed, impactVel, 50.0f, 0.0f,
                              KNOCK_OFF_TYPE_FALL, 2, 1000, nullptr, true, false);
    EXPECT_NEAR(event.m_moveSpeed.x, 1.0f, 1e-5f);
    EXPECT_NEAR(event.m_moveSpeed.y, 2.0f, 1e-5f);
    EXPECT_NEAR(event.m_collisionImpactVelocity.x, 4.0f, 1e-5f);
    EXPECT_NEAR(event.m_damageIntensity, 50.0f, 1e-5f);
    EXPECT_EQ(event.m_knockOffType, (uint8)KNOCK_OFF_TYPE_FALL);
    EXPECT_EQ(event.m_knockOffDirection, (uint8)2);
    EXPECT_EQ(event.m_time, (uint32)1000);
    EXPECT_TRUE(event.m_isVictimDriver);
    EXPECT_FALSE(event.m_forceKnockOff);
}

GAME_TEST(CEventKnockOffBike, Constructor_ForceKnockOff) {
    CVector v(0, 0, 0);
    CEventKnockOffBike event(nullptr, v, v, 10.0f, 0.0f,
                              KNOCK_OFF_TYPE_EXPLOSION, 0, 500, nullptr, false, true);
    EXPECT_EQ(event.m_knockOffType, (uint8)KNOCK_OFF_TYPE_EXPLOSION);
    EXPECT_FALSE(event.m_isVictimDriver);
    EXPECT_TRUE(event.m_forceKnockOff);
}

// --- Clone ---

GAME_TEST(CEventKnockOffBike, Clone_PreservesFields) {
    CVector moveSpeed(1.0f, 2.0f, 3.0f);
    CVector impactVel(4.0f, 5.0f, 6.0f);
    CEventKnockOffBike original(nullptr, moveSpeed, impactVel, 75.0f, 1.0f,
                                 KNOCK_OFF_TYPE_FALL, 3, 2000, nullptr, true, true);
    CEvent* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;

    EXPECT_EQ((int32)cloned->GetEventType(), (int32)EVENT_KNOCK_OFF_BIKE);
    auto* c = static_cast<CEventKnockOffBike*>(cloned);
    EXPECT_NEAR(c->m_moveSpeed.x, 1.0f, 1e-5f);
    EXPECT_NEAR(c->m_damageIntensity, 75.0f, 1e-5f);
    EXPECT_EQ(c->m_knockOffType, (uint8)KNOCK_OFF_TYPE_FALL);
    EXPECT_EQ(c->m_knockOffDirection, (uint8)3);
    EXPECT_TRUE(c->m_isVictimDriver);
    EXPECT_TRUE(c->m_forceKnockOff);

    delete cloned;
}
