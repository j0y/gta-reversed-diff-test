// test_CEventAreaCodes.cpp — Tests for CEventAreaCodes.

#include "StdInc.h"
#include "TestFramework.h"
#include "Events/EventAreaCodes.h"
#include "Entity/Ped/Ped.h"

GAME_TEST(CEventAreaCodes, GetEventType) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    CEventAreaCodes event(player);
    EXPECT_EQ((int32)event.GetEventType(), (int32)EVENT_AREA_CODES);
}

GAME_TEST(CEventAreaCodes, GetEventPriority) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    CEventAreaCodes event(player);
    EXPECT_EQ(event.GetEventPriority(), 55);
}

GAME_TEST(CEventAreaCodes, GetLifeTime) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    CEventAreaCodes event(player);
    EXPECT_EQ(event.GetLifeTime(), 0);
}

GAME_TEST(CEventAreaCodes, Clone_PreservesPed) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    CEventAreaCodes original(player);
    CEvent* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;
    EXPECT_EQ((int32)cloned->GetEventType(), (int32)EVENT_AREA_CODES);
    auto* c = static_cast<CEventAreaCodes*>(cloned);
    EXPECT_EQ((uintptr_t)c->m_ped, (uintptr_t)player);
    delete cloned;
}
