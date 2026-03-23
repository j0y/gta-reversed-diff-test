// test_CEventDanger.cpp — Tests for CEventDanger.
// Extends CEventEditableResponse. Triggered when ped senses danger nearby.

#include "StdInc.h"
#include "TestFramework.h"
#include "Events/EventDanger.h"
#include "Entity/Ped/Ped.h"

GAME_TEST(CEventDanger, GetEventType) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    CEventDanger event(player, 10.0f);
    EXPECT_EQ((int32)event.GetEventType(), (int32)EVENT_DANGER);
}

GAME_TEST(CEventDanger, GetEventPriority) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    CEventDanger event(player, 10.0f);
    EXPECT_EQ(event.GetEventPriority(), 20);
}

GAME_TEST(CEventDanger, GetLifeTime) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    CEventDanger event(player, 10.0f);
    EXPECT_EQ(event.GetLifeTime(), 0);
}

GAME_TEST(CEventDanger, Constructor_Fields) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    CEventDanger event(player, 25.0f);
    EXPECT_EQ((uintptr_t)event.m_dangerFrom, (uintptr_t)player);
    EXPECT_NEAR(event.m_dangerRadius, 25.0f, 1e-5f);
}

GAME_TEST(CEventDanger, CloneEditable_PreservesFields) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    CEventDanger original(player, 50.0f);
    auto* cloned = original.CloneEditable();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;

    EXPECT_EQ((int32)cloned->GetEventType(), (int32)EVENT_DANGER);
    auto* c = static_cast<CEventDanger*>(cloned);
    EXPECT_EQ((uintptr_t)c->m_dangerFrom, (uintptr_t)player);
    EXPECT_NEAR(c->m_dangerRadius, 50.0f, 1e-5f);

    delete cloned;
}

GAME_TEST(CEventDanger, DifferentRadii) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    float radii[] = { 0.0f, 5.0f, 25.0f, 100.0f };
    for (float r : radii) {
        CEventDanger event(player, r);
        EXPECT_NEAR(event.m_dangerRadius, r, 1e-5f);
    }
}
