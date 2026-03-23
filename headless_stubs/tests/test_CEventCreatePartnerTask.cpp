// test_CEventCreatePartnerTask.cpp — Tests for CEventCreatePartnerTask.

#include "StdInc.h"
#include "TestFramework.h"
#include "Events/EventCreatePartnerTask.h"
#include "Entity/Ped/Ped.h"

GAME_TEST(CEventCreatePartnerTask, GetEventType) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    CEventCreatePartnerTask event(1, player, true, 5.0f);
    EXPECT_EQ((int32)event.GetEventType(), (int32)EVENT_CREATE_PARTNER_TASK);
}

GAME_TEST(CEventCreatePartnerTask, GetEventPriority) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    CEventCreatePartnerTask event(0, player, false, 3.0f);
    EXPECT_EQ(event.GetEventPriority(), 6);
}

GAME_TEST(CEventCreatePartnerTask, AffectsPed_AlwaysTrue) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    CEventCreatePartnerTask event(0, player, false, 3.0f);
    EXPECT_TRUE(event.AffectsPed(nullptr));
}

GAME_TEST(CEventCreatePartnerTask, Constructor_Fields) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    CEventCreatePartnerTask event(2, player, true, 10.0f);
    EXPECT_EQ(event.m_partnerType, 2);
    EXPECT_EQ((uintptr_t)event.m_partner, (uintptr_t)player);
    EXPECT_TRUE(event.m_isLeadSpeaker);
    EXPECT_NEAR(event.m_meetDist, 10.0f, 1e-5f);
}

GAME_TEST(CEventCreatePartnerTask, Clone_PreservesFields) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    CEventCreatePartnerTask original(3, player, false, 7.5f);
    CEvent* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;
    EXPECT_EQ((int32)cloned->GetEventType(), (int32)EVENT_CREATE_PARTNER_TASK);
    auto* c = static_cast<CEventCreatePartnerTask*>(cloned);
    EXPECT_EQ(c->m_partnerType, 3);
    EXPECT_EQ((uintptr_t)c->m_partner, (uintptr_t)player);
    EXPECT_FALSE(c->m_isLeadSpeaker);
    EXPECT_NEAR(c->m_meetDist, 7.5f, 1e-5f);
    delete cloned;
}
