// test_CEventPedCollisionWithPlayer.cpp — Tests for CEventPedCollisionWithPlayer.
// Extends CEventPedCollisionWithPed — collision specifically with the player.

#include "StdInc.h"
#include "TestFramework.h"
#include "Events/EventPedCollisionWithPlayer.h"
#include "Entity/Ped/Ped.h"

GAME_TEST(CEventPedCollisionWithPlayer, GetEventType) {
    CVector vel(0, 0, 0), pos(0, 0, 0);
    CEventPedCollisionWithPlayer event(0, 5.0f, nullptr, vel, pos, PEDMOVE_WALK, PEDMOVE_STILL);
    EXPECT_EQ((int32)event.GetEventType(), (int32)EVENT_PED_COLLISION_WITH_PLAYER);
}

GAME_TEST(CEventPedCollisionWithPlayer, Clone_PreservesType) {
    CVector vel(1.0f, 2.0f, 3.0f), pos(4.0f, 5.0f, 6.0f);
    CEventPedCollisionWithPlayer original(2, 15.0f, nullptr, vel, pos, PEDMOVE_RUN, PEDMOVE_WALK);
    CEvent* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;
    EXPECT_EQ((int32)cloned->GetEventType(), (int32)EVENT_PED_COLLISION_WITH_PLAYER);
    delete cloned;
}

GAME_TEST(CEventPedCollisionWithPlayer, Constructor_Fields) {
    CVector vel(1.0f, 0.0f, 0.0f), pos(10.0f, 20.0f, 5.0f);
    CEventPedCollisionWithPlayer event(3, 25.0f, nullptr, vel, pos, PEDMOVE_SPRINT, PEDMOVE_RUN);
    EXPECT_EQ(event.m_pieceType, (int16)3);
    EXPECT_NEAR(event.m_damageIntensity, 25.0f, 1e-5f);
    EXPECT_EQ((int32)event.GetMoveState(), (int32)PEDMOVE_SPRINT);
    EXPECT_EQ((int32)event.GetVictimMoveState(), (int32)PEDMOVE_RUN);
}
