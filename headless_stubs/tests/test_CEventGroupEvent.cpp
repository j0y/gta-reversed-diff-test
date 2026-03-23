// test_CEventGroupEvent.cpp — Tests for CEventGroupEvent.
// Wraps another event for ped group propagation.

#include "StdInc.h"
#include "TestFramework.h"
#include "Events/EventGroupEvent.h"
#include "Events/EventOnFire.h"
#include "Events/EventDeath.h"
#include "Entity/Ped/Ped.h"

GAME_TEST(CEventGroupEvent, GetEventType) {
    auto* ped = FindPlayerPed(0);
    if (!ped) return;
    auto* inner = new CEventOnFire();
    CEventGroupEvent event(ped, inner);
    EXPECT_EQ((int32)event.GetEventType(), (int32)EVENT_GROUP_EVENT);
}

GAME_TEST(CEventGroupEvent, GetEventPriority) {
    auto* ped = FindPlayerPed(0);
    if (!ped) return;
    auto* inner = new CEventOnFire();
    CEventGroupEvent event(ped, inner);
    EXPECT_EQ(event.GetEventPriority(), 41);
}

GAME_TEST(CEventGroupEvent, GetLocalSoundLevel) {
    auto* ped = FindPlayerPed(0);
    if (!ped) return;
    auto* inner = new CEventOnFire();
    CEventGroupEvent event(ped, inner);
    EXPECT_NEAR(event.GetLocalSoundLevel(), 100.0f, 1e-5f);
}

GAME_TEST(CEventGroupEvent, AffectsPed_AlwaysFalse) {
    auto* ped = FindPlayerPed(0);
    if (!ped) return;
    auto* inner = new CEventOnFire();
    CEventGroupEvent event(ped, inner);
    EXPECT_FALSE(event.AffectsPed(ped));
}

GAME_TEST(CEventGroupEvent, GetEvent_ReturnsInner) {
    auto* ped = FindPlayerPed(0);
    if (!ped) return;
    auto* inner = new CEventOnFire();
    CEventGroupEvent event(ped, inner);
    EXPECT_EQ((int32)event.GetEvent().GetEventType(), (int32)EVENT_ON_FIRE);
}

GAME_TEST(CEventGroupEvent, Clone_PreservesInnerType) {
    auto* ped = FindPlayerPed(0);
    if (!ped) return;
    auto* inner = new CEventDeath(true, 5000);
    CEventGroupEvent original(ped, inner);
    CEvent* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;

    EXPECT_EQ((int32)cloned->GetEventType(), (int32)EVENT_GROUP_EVENT);
    auto* c = static_cast<CEventGroupEvent*>(cloned);
    EXPECT_EQ((int32)c->GetEvent().GetEventType(), (int32)EVENT_DEATH);

    delete cloned;
}
