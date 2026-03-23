// test_CEventPassObject.cpp — Tests for CEventPassObject.

#include "StdInc.h"
#include "TestFramework.h"
#include "Events/EventPassObject.h"
#include "Entity/Ped/Ped.h"

GAME_TEST(CEventPassObject, GetEventType) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    CEventPassObject event(player, false);
    EXPECT_EQ((int32)event.GetEventType(), (int32)EVENT_PASS_OBJECT);
}

GAME_TEST(CEventPassObject, GetEventPriority) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    CEventPassObject event(player, false);
    // Just verify it returns a reasonable value (not crashing)
    int32 prio = event.GetEventPriority();
    EXPECT_TRUE(prio >= 0);
    EXPECT_TRUE(prio <= 100);
}

GAME_TEST(CEventPassObject, Constructor_Fields) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    CEventPassObject event(player, true);
    EXPECT_EQ((uintptr_t)event.m_giver, (uintptr_t)player);
    EXPECT_TRUE(event.m_dontPassObject);
}

GAME_TEST(CEventPassObject, Constructor_DontPass_False) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    CEventPassObject event(player, false);
    EXPECT_FALSE(event.m_dontPassObject);
}

GAME_TEST(CEventPassObject, Clone_PreservesFields) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    CEventPassObject original(player, true);
    CEvent* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;

    EXPECT_EQ((int32)cloned->GetEventType(), (int32)EVENT_PASS_OBJECT);
    auto* c = static_cast<CEventPassObject*>(cloned);
    EXPECT_EQ((uintptr_t)c->m_giver, (uintptr_t)player);
    EXPECT_TRUE(c->m_dontPassObject);

    delete cloned;
}
