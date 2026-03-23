// test_CEventGunShot.cpp — Tests for CEventGunShot.
// Hook paths: Events/CEventGunShot/<fn>
// CEventGunShot extends CEventEditableResponse. Tracks shooter, bullet
// start/end points, and sound properties.

#include "StdInc.h"
#include "TestFramework.h"
#include "Events/EventGunShot.h"
#include "Entity/Ped/Ped.h"
#include "ScenarioHelpers.h"

// --- Construction & type ---

GAME_TEST(CEventGunShot, GetEventType) {
    CVector start(0, 0, 0), end(10, 0, 0);
    CEventGunShot event(nullptr, start, end, false);
    EXPECT_EQ((int32)event.GetEventType(), (int32)EVENT_SHOT_FIRED);
}

GAME_TEST(CEventGunShot, GetEventPriority) {
    CVector start(0, 0, 0), end(10, 0, 0);
    CEventGunShot event(nullptr, start, end, false);
    EXPECT_EQ(event.GetEventPriority(), 35);
}

GAME_TEST(CEventGunShot, GetLifeTime) {
    CVector start(0, 0, 0), end(10, 0, 0);
    CEventGunShot event(nullptr, start, end, false);
    EXPECT_EQ(event.GetLifeTime(), 0);
}

GAME_TEST(CEventGunShot, Constructor_Positions) {
    CVector start(1.0f, 2.0f, 3.0f), end(10.0f, 20.0f, 30.0f);
    CEventGunShot event(nullptr, start, end, false);
    EXPECT_NEAR(event.m_startPoint.x, 1.0f, 1e-5f);
    EXPECT_NEAR(event.m_startPoint.y, 2.0f, 1e-5f);
    EXPECT_NEAR(event.m_startPoint.z, 3.0f, 1e-5f);
    EXPECT_NEAR(event.m_endPoint.x, 10.0f, 1e-5f);
    EXPECT_NEAR(event.m_endPoint.y, 20.0f, 1e-5f);
    EXPECT_NEAR(event.m_endPoint.z, 30.0f, 1e-5f);
}

// --- Sound level ---

GAME_TEST(CEventGunShot, SoundLevel_Audible) {
    CVector start(0, 0, 0), end(10, 0, 0);
    CEventGunShot event(nullptr, start, end, false);
    EXPECT_NEAR(event.GetLocalSoundLevel(), 160.0f, 1e-5f);
}

GAME_TEST(CEventGunShot, SoundLevel_Silenced) {
    CVector start(0, 0, 0), end(10, 0, 0);
    CEventGunShot event(nullptr, start, end, true);
    EXPECT_NEAR(event.GetLocalSoundLevel(), 0.0f, 1e-5f);
}

// --- CanBeInterrupted ---

GAME_TEST(CEventGunShot, CanBeInterrupted) {
    CVector start(0, 0, 0), end(10, 0, 0);
    CEventGunShot event(nullptr, start, end, false);
    EXPECT_TRUE(event.CanBeInterruptedBySameEvent());
}

// --- GetSourceEntity ---

GAME_TEST(CEventGunShot, GetSourceEntity_Null) {
    CVector start(0, 0, 0), end(10, 0, 0);
    CEventGunShot event(nullptr, start, end, false);
    EXPECT_EQ((uintptr_t)event.GetSourceEntity(), (uintptr_t)nullptr);
}

GAME_TEST(CEventGunShot, GetSourceEntity_Player) {
    auto* player = FindPlayerPed(0);
    if (!player) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No player ped"); return; }

    CVector start(0, 0, 0), end(10, 0, 0);
    CEventGunShot event(player, start, end, false);
    EXPECT_EQ((uintptr_t)event.GetSourceEntity(), (uintptr_t)player);
}

// --- IsCriminalEvent differential ---

GAME_DIFF_TEST(CEventGunShot, IsCriminalEvent_NullShooter) {
    CVector start(0, 0, 0), end(10, 0, 0);
    CEventGunShot event(nullptr, start, end, false);

    bool orig, rev;
    { HookDisableGuard guard("Events/CEventGunShot/IsCriminalEvent");
      orig = event.IsCriminalEvent(); }
    rev = event.IsCriminalEvent();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CEventGunShot, IsCriminalEvent_PlayerShooter) {
    auto* player = FindPlayerPed(0);
    if (!player) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No player ped"); return; }

    CVector start(0, 0, 0), end(10, 0, 0);
    CEventGunShot event(player, start, end, false);

    bool orig, rev;
    { HookDisableGuard guard("Events/CEventGunShot/IsCriminalEvent");
      orig = event.IsCriminalEvent(); }
    rev = event.IsCriminalEvent();
    EXPECT_EQ(orig, rev);
}

// --- TakesPriorityOver differential ---

GAME_DIFF_TEST(CEventGunShot, TakesPriorityOver) {
    auto* player = FindPlayerPed(0);
    if (!player) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No player ped"); return; }

    CVector start(0, 0, 0), end(10, 0, 0);
    CEventGunShot event1(player, start, end, false);
    CEventGunShot event2(nullptr, start, end, false);

    bool orig, rev;
    { HookDisableGuard guard("Events/CEventGunShot/TakesPriorityOver");
      orig = event1.TakesPriorityOver(event2); }
    rev = event1.TakesPriorityOver(event2);
    EXPECT_EQ(orig, rev);
}
