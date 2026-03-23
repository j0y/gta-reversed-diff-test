// test_CEventObjectCollision.cpp — Tests for CEventObjectCollision.
// Collision event between ped and world object.

#include "StdInc.h"
#include "TestFramework.h"
#include "Events/EventObjectCollision.h"

GAME_TEST(CEventObjectCollision, GetEventType) {
    CVector normal(0, 0, 1), pos(0, 0, 0);
    CEventObjectCollision event(0, 10.0f, nullptr, normal, pos, 0);
    EXPECT_EQ((int32)event.GetEventType(), (int32)EVENT_OBJECT_COLLISION);
}

GAME_TEST(CEventObjectCollision, GetEventPriority) {
    CVector normal(0, 0, 1), pos(0, 0, 0);
    CEventObjectCollision event(0, 10.0f, nullptr, normal, pos, 0);
    EXPECT_EQ(event.GetEventPriority(), 57);
}

GAME_TEST(CEventObjectCollision, GetLifeTime) {
    CVector normal(0, 0, 1), pos(0, 0, 0);
    CEventObjectCollision event(0, 10.0f, nullptr, normal, pos, 0);
    EXPECT_EQ(event.GetLifeTime(), 0);
}

GAME_TEST(CEventObjectCollision, Constructor_Fields) {
    CVector normal(1.0f, 2.0f, 3.0f), pos(4.0f, 5.0f, 6.0f);
    CEventObjectCollision event(5, 30.0f, nullptr, normal, pos, 2);
    EXPECT_EQ(event.m_pieceType, (int16)5);
    EXPECT_NEAR(event.m_damageIntensity, 30.0f, 1e-5f);
    EXPECT_NEAR(event.m_impactNormal.x, 1.0f, 1e-5f);
    EXPECT_NEAR(event.m_impactPos.z, 6.0f, 1e-5f);
    EXPECT_EQ(event.m_moveState, (int16)2);
}

GAME_TEST(CEventObjectCollision, TakesPriorityOver_AlwaysTrue) {
    CVector v(0, 0, 0);
    CEventObjectCollision e1(0, 5.0f, nullptr, v, v, 0);
    CEventObjectCollision e2(0, 10.0f, nullptr, v, v, 0);
    EXPECT_TRUE(e1.TakesPriorityOver(e2));
}

GAME_TEST(CEventObjectCollision, Clone_PreservesFields) {
    CVector normal(1.0f, 0.0f, 0.0f), pos(10.0f, 20.0f, 5.0f);
    CEventObjectCollision original(3, 25.0f, nullptr, normal, pos, 1);
    CEvent* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;

    EXPECT_EQ((int32)cloned->GetEventType(), (int32)EVENT_OBJECT_COLLISION);
    auto* c = static_cast<CEventObjectCollision*>(cloned);
    EXPECT_EQ(c->m_pieceType, (int16)3);
    EXPECT_NEAR(c->m_damageIntensity, 25.0f, 1e-5f);

    delete cloned;
}
