// test_CEventSoundQuiet.cpp — Tests for CEventSoundQuiet.

#include "StdInc.h"
#include "TestFramework.h"
#include "Events/EventSoundQuiet.h"
#include "Entity/Ped/Ped.h"

GAME_TEST(CEventSoundQuiet, GetEventType) {
    CVector pos(0, 0, 0);
    CEventSoundQuiet event(nullptr, 30.0f, 1000, pos);
    EXPECT_EQ((int32)event.GetEventType(), (int32)EVENT_SOUND_QUIET);
}

GAME_TEST(CEventSoundQuiet, GetEventPriority) {
    CVector pos(0, 0, 0);
    CEventSoundQuiet event(nullptr, 30.0f, 1000, pos);
    EXPECT_EQ(event.GetEventPriority(), 8);
}

GAME_TEST(CEventSoundQuiet, GetLocalSoundLevel) {
    CVector pos(0, 0, 0);
    CEventSoundQuiet event(nullptr, 45.0f, 1000, pos);
    EXPECT_NEAR(event.GetLocalSoundLevel(), 45.0f, 1e-5f);
}

GAME_TEST(CEventSoundQuiet, CanBeInterrupted) {
    CVector pos(0, 0, 0);
    CEventSoundQuiet event(nullptr, 30.0f, 1000, pos);
    EXPECT_TRUE(event.CanBeInterruptedBySameEvent());
}

GAME_TEST(CEventSoundQuiet, GetSourceEntity_Null) {
    CVector pos(0, 0, 0);
    CEventSoundQuiet event(nullptr, 30.0f, 1000, pos);
    EXPECT_EQ((uintptr_t)event.GetSourceEntity(), (uintptr_t)nullptr);
}

GAME_TEST(CEventSoundQuiet, GetSourceEntity_Player) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    CVector pos(100.0f, 200.0f, 10.0f);
    CEventSoundQuiet event(player, 50.0f, 2000, pos);
    EXPECT_EQ((uintptr_t)event.GetSourceEntity(), (uintptr_t)player);
}

GAME_TEST(CEventSoundQuiet, Constructor_Fields) {
    CVector pos(10.0f, 20.0f, 30.0f);
    CEventSoundQuiet event(nullptr, 75.0f, 5000, pos);
    EXPECT_NEAR(event.m_fLocalSoundLevel, 75.0f, 1e-5f);
    EXPECT_EQ(event.m_startTimeInMs, (uint32)5000);
    EXPECT_NEAR(event.m_position.x, 10.0f, 1e-5f);
    EXPECT_NEAR(event.m_position.y, 20.0f, 1e-5f);
    EXPECT_NEAR(event.m_position.z, 30.0f, 1e-5f);
}

GAME_TEST(CEventSoundQuiet, DifferentSoundLevels) {
    CVector pos(0, 0, 0);
    float levels[] = { 0.0f, 10.0f, 50.0f, 100.0f, 160.0f };
    for (float lvl : levels) {
        CEventSoundQuiet event(nullptr, lvl, 0, pos);
        EXPECT_NEAR(event.GetLocalSoundLevel(), lvl, 1e-5f);
    }
}
