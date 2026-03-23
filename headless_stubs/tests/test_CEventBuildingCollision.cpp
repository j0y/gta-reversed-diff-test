// test_CEventBuildingCollision.cpp — Tests for CEventBuildingCollision.
// Collision event between ped and building.

#include "StdInc.h"
#include "TestFramework.h"
#include "Events/EventBuildingCollision.h"
#include "Events/EventObjectCollision.h"

GAME_TEST(CEventBuildingCollision, GetEventType) {
    CVector normal(0, 0, 1), pos(0, 0, 0);
    CEventBuildingCollision event(0, 10.0f, nullptr, normal, pos, 0);
    EXPECT_EQ((int32)event.GetEventType(), (int32)EVENT_BUILDING_COLLISION);
}

GAME_TEST(CEventBuildingCollision, GetEventPriority) {
    CVector normal(0, 0, 1), pos(0, 0, 0);
    CEventBuildingCollision event(0, 10.0f, nullptr, normal, pos, 0);
    EXPECT_EQ(event.GetEventPriority(), 59);
}

GAME_TEST(CEventBuildingCollision, GetLifeTime) {
    CVector normal(0, 0, 1), pos(0, 0, 0);
    CEventBuildingCollision event(0, 10.0f, nullptr, normal, pos, 0);
    EXPECT_EQ(event.GetLifeTime(), 0);
}

GAME_TEST(CEventBuildingCollision, CanBeInterrupted) {
    CVector v(0, 0, 0);
    CEventBuildingCollision event(0, 10.0f, nullptr, v, v, 0);
    EXPECT_TRUE(event.CanBeInterruptedBySameEvent());
}

GAME_TEST(CEventBuildingCollision, TakesPriorityOver_DifferentType) {
    CVector v(0, 0, 0);
    CEventBuildingCollision bldgEvent(0, 10.0f, nullptr, v, v, 0);
    CEventObjectCollision objEvent(0, 5.0f, nullptr, v, v, 0);
    // Takes priority over different event types
    EXPECT_TRUE(bldgEvent.TakesPriorityOver(objEvent));
}

GAME_TEST(CEventBuildingCollision, TakesPriorityOver_SameType) {
    CVector v(0, 0, 0);
    CEventBuildingCollision e1(0, 10.0f, nullptr, v, v, 0);
    CEventBuildingCollision e2(0, 5.0f, nullptr, v, v, 0);
    // Does NOT take priority over same type
    EXPECT_FALSE(e1.TakesPriorityOver(e2));
}

GAME_TEST(CEventBuildingCollision, Constructor_Fields) {
    CVector normal(1.0f, 2.0f, 3.0f), pos(4.0f, 5.0f, 6.0f);
    CEventBuildingCollision event(7, 50.0f, nullptr, normal, pos, 3);
    EXPECT_EQ(event.m_pieceType, (int16)7);
    EXPECT_NEAR(event.m_damageIntensity, 50.0f, 1e-5f);
    EXPECT_NEAR(event.m_impactNormal.y, 2.0f, 1e-5f);
    EXPECT_NEAR(event.m_impactPos.x, 4.0f, 1e-5f);
}

GAME_TEST(CEventBuildingCollision, Clone_PreservesFields) {
    CVector normal(0.0f, 1.0f, 0.0f), pos(10.0f, 20.0f, 30.0f);
    CEventBuildingCollision original(2, 15.0f, nullptr, normal, pos, 1);
    CEvent* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;

    EXPECT_EQ((int32)cloned->GetEventType(), (int32)EVENT_BUILDING_COLLISION);
    auto* c = static_cast<CEventBuildingCollision*>(cloned);
    EXPECT_EQ(c->m_pieceType, (int16)2);
    EXPECT_NEAR(c->m_damageIntensity, 15.0f, 1e-5f);

    delete cloned;
}
