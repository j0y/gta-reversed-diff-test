// test_CPed6.cpp — Differential tests for CPed query functions.
// Hook paths: Entity/Ped/CPed/<fn>
//
// Previously tested (CPed1-5): IsAlive, IsPedInControl, CanBeArrested,
//   GetBikeRidingSkill, GetWalkAnimSpeed, GetWeaponSlot, DoWeHaveWeaponAvailable,
//   GetWeaponSkill, CanSeeEntity, UseGroundColModel, PedIsReadyForConversation,
//   IsPointerValid, OurPedCanSeeThisEntity, etc.
//
// New targets:
//   GetLocalDirection         — angle math: heading → directional quadrant (0-3)
//   CanStrafeOrMouseControl   — state machine query
//   CanBeDeletedEvenInVehicle — creation type query
//   DoesLOSBulletHitPed       — height comparison for bullet hit zone
//   GetPedTalking             — speech state query
//   IsPlayingHandSignal       — task presence query
//   IsPedShootable            — state machine query

#include "StdInc.h"
#include "TestFramework.h"
#include "ScenarioHelpers.h"

// ── GetLocalDirection ──
// Converts a 2D direction vector relative to the ped's rotation into
// a quadrant index: 0=forward, 1=left, 2=backward, 3=right.
// Uses angle math with RadiansToDegrees and modulo.

GAME_DIFF_TEST(CPed6, Diff_GetLocalDirection_Player) {
    auto* player = FindPlayerPed(0);
    if (!player) return;

    // Test with direction vectors in all quadrants
    CVector2D directions[] = {
        CVector2D(0.0f, 1.0f),    // north
        CVector2D(1.0f, 0.0f),    // east
        CVector2D(0.0f, -1.0f),   // south
        CVector2D(-1.0f, 0.0f),   // west
        CVector2D(0.7f, 0.7f),    // northeast
        CVector2D(-0.7f, 0.7f),   // northwest
        CVector2D(0.7f, -0.7f),   // southeast
        CVector2D(-0.7f, -0.7f),  // southwest
    };

    for (auto& dir : directions) {
        uint8 orig, rev;
        { HookDisableGuard guard("Entity/Ped/CPed/GetLocalDirection");
          orig = player->GetLocalDirection(dir); }
        rev = player->GetLocalDirection(dir);
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(CPed6, Diff_GetLocalDirection_SpawnedPed) {
    int32 pedModel = Scenario::FindLoadedPedModel();
    if (pedModel < 0) return;
    Scenario::TestPed ped(PED_TYPE_CIVMALE, pedModel, CVector(2490.f, -1665.f, 13.5f));

    CVector2D directions[] = {
        CVector2D(0.0f, 1.0f),
        CVector2D(1.0f, 0.0f),
        CVector2D(0.0f, -1.0f),
        CVector2D(-1.0f, 0.0f),
    };

    for (auto& dir : directions) {
        uint8 orig, rev;
        { HookDisableGuard guard("Entity/Ped/CPed/GetLocalDirection");
          orig = ped.AsPed()->GetLocalDirection(dir); }
        rev = ped.AsPed()->GetLocalDirection(dir);
        EXPECT_EQ(orig, rev);
    }
}

// ── CanStrafeOrMouseControl ──
// Returns true for specific ped states (IDLE, FLEE, AIMGUN, ATTACK, etc.)

GAME_DIFF_TEST(CPed6, Diff_CanStrafeOrMouseControl_Player) {
    auto* player = FindPlayerPed(0);
    if (!player) return;

    bool orig, rev;
    { HookDisableGuard guard("Entity/Ped/CPed/CanStrafeOrMouseControl");
      orig = player->CanStrafeOrMouseControl(); }
    rev = player->CanStrafeOrMouseControl();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CPed6, Diff_CanStrafeOrMouseControl_SpawnedPed) {
    int32 pedModel = Scenario::FindLoadedPedModel();
    if (pedModel < 0) return;
    Scenario::TestPed ped(PED_TYPE_CIVMALE, pedModel, CVector(2490.f, -1665.f, 13.5f));

    bool orig, rev;
    { HookDisableGuard guard("Entity/Ped/CPed/CanStrafeOrMouseControl");
      orig = ped.AsPed()->CanStrafeOrMouseControl(); }
    rev = ped.AsPed()->CanStrafeOrMouseControl();
    EXPECT_EQ(orig, rev);
}

// ── CanBeDeletedEvenInVehicle ──
// Returns false for MISSION and UNKNOWN peds, true otherwise.

GAME_DIFF_TEST(CPed6, Diff_CanBeDeletedEvenInVehicle_Player) {
    auto* player = FindPlayerPed(0);
    if (!player) return;

    bool orig, rev;
    { HookDisableGuard guard("Entity/Ped/CPed/CanBeDeletedEvenInVehicle");
      orig = player->CanBeDeletedEvenInVehicle(); }
    rev = player->CanBeDeletedEvenInVehicle();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CPed6, Diff_CanBeDeletedEvenInVehicle_SpawnedPed) {
    int32 pedModel = Scenario::FindLoadedPedModel();
    if (pedModel < 0) return;
    Scenario::TestPed ped(PED_TYPE_CIVMALE, pedModel, CVector(2490.f, -1665.f, 13.5f));

    bool orig, rev;
    { HookDisableGuard guard("Entity/Ped/CPed/CanBeDeletedEvenInVehicle");
      orig = ped.AsPed()->CanBeDeletedEvenInVehicle(); }
    rev = ped.AsPed()->CanBeDeletedEvenInVehicle();
    EXPECT_EQ(orig, rev);
}

// ── DoesLOSBulletHitPed ──
// Returns 0 (above head), 1 (falling or below head), 2 (near head level).
// Tests head bone position vs collision point height.

GAME_DIFF_TEST(CPed6, Diff_DoesLOSBulletHitPed_AboveHead) {
    auto* player = FindPlayerPed(0);
    if (!player) return;

    // Collision point high above player
    CColPoint colPoint{};
    colPoint.m_vecPoint = player->GetPosition() + CVector(0.0f, 0.0f, 5.0f);

    uint8 orig, rev;
    { HookDisableGuard guard("Entity/Ped/CPed/DoesLOSBulletHitPed");
      orig = player->DoesLOSBulletHitPed(colPoint); }
    rev = player->DoesLOSBulletHitPed(colPoint);
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CPed6, Diff_DoesLOSBulletHitPed_BelowHead) {
    auto* player = FindPlayerPed(0);
    if (!player) return;

    // Collision point at torso level
    CColPoint colPoint{};
    colPoint.m_vecPoint = player->GetPosition() + CVector(0.0f, 0.0f, 0.5f);

    uint8 orig, rev;
    { HookDisableGuard guard("Entity/Ped/CPed/DoesLOSBulletHitPed");
      orig = player->DoesLOSBulletHitPed(colPoint); }
    rev = player->DoesLOSBulletHitPed(colPoint);
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CPed6, Diff_DoesLOSBulletHitPed_HeadLevel) {
    auto* player = FindPlayerPed(0);
    if (!player) return;

    // Collision point near head height (~1.7m above feet)
    CColPoint colPoint{};
    colPoint.m_vecPoint = player->GetPosition() + CVector(0.0f, 0.0f, 1.7f);

    uint8 orig, rev;
    { HookDisableGuard guard("Entity/Ped/CPed/DoesLOSBulletHitPed");
      orig = player->DoesLOSBulletHitPed(colPoint); }
    rev = player->DoesLOSBulletHitPed(colPoint);
    EXPECT_EQ(orig, rev);
}

// ── GetPedTalking ──
// Delegates to m_pedSpeech.GetPedTalking().

GAME_DIFF_TEST(CPed6, Diff_GetPedTalking_Player) {
    auto* player = FindPlayerPed(0);
    if (!player) return;

    bool orig, rev;
    { HookDisableGuard guard("Entity/Ped/CPed/GetPedTalking");
      orig = player->GetPedTalking(); }
    rev = player->GetPedTalking();
    EXPECT_EQ(orig, rev);
}

// ── IsPlayingHandSignal ──
// Checks if ped has TASK_COMPLEX_HANDSIGNAL_ANIM active.

GAME_DIFF_TEST(CPed6, Diff_IsPlayingHandSignal_Player) {
    auto* player = FindPlayerPed(0);
    if (!player) return;

    bool orig, rev;
    { HookDisableGuard guard("Entity/Ped/CPed/IsPlayingHandSignal");
      orig = player->IsPlayingHandSignal(); }
    rev = player->IsPlayingHandSignal();
    EXPECT_EQ(orig, rev);
}

// ── IsPedShootable ──
// Returns true for specific ped states where shooting is valid.

GAME_DIFF_TEST(CPed6, Diff_IsPedShootable_Player) {
    auto* player = FindPlayerPed(0);
    if (!player) return;

    bool orig, rev;
    { HookDisableGuard guard("Entity/Ped/CPed/IsPedShootable");
      orig = player->IsPedShootable(); }
    rev = player->IsPedShootable();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CPed6, Diff_IsPedShootable_SpawnedPed) {
    int32 pedModel = Scenario::FindLoadedPedModel();
    if (pedModel < 0) return;
    Scenario::TestPed ped(PED_TYPE_CIVMALE, pedModel, CVector(2490.f, -1665.f, 13.5f));

    bool orig, rev;
    { HookDisableGuard guard("Entity/Ped/CPed/IsPedShootable");
      orig = ped.AsPed()->IsPedShootable(); }
    rev = ped.AsPed()->IsPedShootable();
    EXPECT_EQ(orig, rev);
}
