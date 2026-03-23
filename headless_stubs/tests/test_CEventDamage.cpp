// test_CEventDamage.cpp — Tests for CEventDamage.
// Hook paths: Events/CEventDamage/<fn>
// CEventDamage is the main damage event — tracks source, weapon type,
// body part, direction, animations, and damage response.

#include "StdInc.h"
#include "TestFramework.h"
#include "Events/EventDamage.h"
#include "Entity/Ped/Ped.h"

// --- Construction & type ---

GAME_TEST(CEventDamage, GetEventType) {
    CEventDamage event(nullptr, 0, WEAPON_UNARMED, PED_PIECE_TORSO, 0, false, false);
    EXPECT_EQ((int32)event.GetEventType(), (int32)EVENT_DAMAGE);
}

GAME_TEST(CEventDamage, GetEventPriority) {
    CEventDamage event(nullptr, 0, WEAPON_UNARMED, PED_PIECE_TORSO, 0, false, false);
    EXPECT_EQ(event.GetEventPriority(), 65);
}

GAME_TEST(CEventDamage, GetLifeTime) {
    CEventDamage event(nullptr, 0, WEAPON_UNARMED, PED_PIECE_TORSO, 0, false, false);
    EXPECT_EQ(event.GetLifeTime(), 0);
}

GAME_TEST(CEventDamage, GetLocalSoundLevel) {
    CEventDamage event(nullptr, 0, WEAPON_UNARMED, PED_PIECE_TORSO, 0, false, false);
    EXPECT_NEAR(event.GetLocalSoundLevel(), 55.0f, 1e-5f);
}

GAME_TEST(CEventDamage, CanBeInterrupted) {
    CEventDamage event(nullptr, 0, WEAPON_UNARMED, PED_PIECE_TORSO, 0, false, false);
    EXPECT_TRUE(event.CanBeInterruptedBySameEvent());
}

GAME_TEST(CEventDamage, Constructor_Fields) {
    CEventDamage event(nullptr, 1000, WEAPON_PISTOL, PED_PIECE_HEAD, 2, true, false);
    EXPECT_EQ((uintptr_t)event.m_pSourceEntity, (uintptr_t)nullptr);
    EXPECT_EQ(event.m_nStartTime, (uint32)1000);
    EXPECT_EQ((int32)event.m_weaponType, (int32)WEAPON_PISTOL);
    EXPECT_EQ((int32)event.m_pedPieceType, (int32)PED_PIECE_HEAD);
    EXPECT_EQ(event.m_ucDirection, (uint8)2);
}

GAME_TEST(CEventDamage, Constructor_NullSource) {
    CEventDamage event(nullptr, 0, WEAPON_UNARMED, PED_PIECE_TORSO, 0, false, false);
    EXPECT_EQ((uintptr_t)event.GetSourceEntity(), (uintptr_t)nullptr);
}

// --- GetSourceEntity differential ---

GAME_DIFF_TEST(CEventDamage, GetSourceEntity_NullSource) {
    CEventDamage event(nullptr, 0, WEAPON_UNARMED, PED_PIECE_TORSO, 0, false, false);

    CEntity* orig;
    CEntity* rev;
    { HookDisableGuard guard("Events/CEventDamage/GetSourceEntity");
      orig = event.GetSourceEntity(); }
    rev = event.GetSourceEntity();
    EXPECT_EQ((uintptr_t)orig, (uintptr_t)rev);
}

GAME_DIFF_TEST(CEventDamage, GetSourceEntity_PlayerSource) {
    auto* player = FindPlayerPed(0);
    if (!player) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No player ped"); return; }

    CEventDamage event(player, 0, WEAPON_PISTOL, PED_PIECE_TORSO, 0, false, false);

    CEntity* orig;
    CEntity* rev;
    { HookDisableGuard guard("Events/CEventDamage/GetSourceEntity");
      orig = event.GetSourceEntity(); }
    rev = event.GetSourceEntity();
    EXPECT_EQ((uintptr_t)orig, (uintptr_t)rev);
}

// --- IsCriminalEvent differential ---

GAME_DIFF_TEST(CEventDamage, IsCriminalEvent_NullSource) {
    CEventDamage event(nullptr, 0, WEAPON_UNARMED, PED_PIECE_TORSO, 0, false, false);

    bool orig, rev;
    { HookDisableGuard guard("Events/CEventDamage/IsCriminalEvent");
      orig = event.IsCriminalEvent(); }
    rev = event.IsCriminalEvent();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CEventDamage, IsCriminalEvent_PlayerSource) {
    auto* player = FindPlayerPed(0);
    if (!player) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No player ped"); return; }

    CEventDamage event(player, 0, WEAPON_PISTOL, PED_PIECE_TORSO, 0, false, false);

    bool orig, rev;
    { HookDisableGuard guard("Events/CEventDamage/IsCriminalEvent");
      orig = event.IsCriminalEvent(); }
    rev = event.IsCriminalEvent();
    EXPECT_EQ(orig, rev);
}

// --- IsSameEventForAI ---

GAME_TEST(CEventDamage, IsSameEventForAI_Same) {
    auto* player = FindPlayerPed(0);
    if (!player) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No player ped"); return; }

    CEventDamage event1(player, 0, WEAPON_PISTOL, PED_PIECE_TORSO, 0, false, false);
    CEventDamage event2(player, 100, WEAPON_PISTOL, PED_PIECE_HEAD, 1, false, false);
    EXPECT_TRUE(event1.IsSameEventForAI(event2));
}

GAME_TEST(CEventDamage, IsSameEventForAI_DifferentWeapon) {
    auto* player = FindPlayerPed(0);
    if (!player) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No player ped"); return; }

    CEventDamage event1(player, 0, WEAPON_PISTOL, PED_PIECE_TORSO, 0, false, false);
    CEventDamage event2(player, 0, WEAPON_SHOTGUN, PED_PIECE_TORSO, 0, false, false);
    EXPECT_FALSE(event1.IsSameEventForAI(event2));
}

GAME_TEST(CEventDamage, IsSameEventForAI_DifferentSource) {
    CEventDamage event1(nullptr, 0, WEAPON_PISTOL, PED_PIECE_TORSO, 0, false, false);
    auto* player = FindPlayerPed(0);
    if (!player) return;
    CEventDamage event2(player, 0, WEAPON_PISTOL, PED_PIECE_TORSO, 0, false, false);
    EXPECT_FALSE(event1.IsSameEventForAI(event2));
}

// --- Weapon type coverage ---

GAME_TEST(CEventDamage, WeaponTypes) {
    eWeaponType weapons[] = {
        WEAPON_UNARMED, WEAPON_KNIFE, WEAPON_PISTOL, WEAPON_SHOTGUN,
        WEAPON_AK47, WEAPON_SNIPERRIFLE, WEAPON_ROCKET, WEAPON_FLAMETHROWER
    };
    for (auto wt : weapons) {
        CEventDamage event(nullptr, 0, wt, PED_PIECE_TORSO, 0, false, false);
        EXPECT_EQ((int32)event.m_weaponType, (int32)wt);
        EXPECT_EQ((int32)event.GetEventType(), (int32)EVENT_DAMAGE);
    }
}

// --- TakesPriorityOver differential ---

GAME_DIFF_TEST(CEventDamage, TakesPriorityOver) {
    CEventDamage event1(nullptr, 0, WEAPON_PISTOL, PED_PIECE_TORSO, 0, false, false);
    CEventDamage event2(nullptr, 0, WEAPON_UNARMED, PED_PIECE_TORSO, 0, false, false);

    bool orig, rev;
    { HookDisableGuard guard("Events/CEventDamage/TakesPriorityOver");
      orig = event1.TakesPriorityOver(event2); }
    rev = event1.TakesPriorityOver(event2);
    EXPECT_EQ(orig, rev);
}
