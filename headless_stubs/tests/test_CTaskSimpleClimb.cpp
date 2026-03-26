// test_CTaskSimpleClimb.cpp — Differential tests for CTaskSimpleClimb.
// Hook paths: Tasks/TaskTypes/CTaskSimpleClimb/<fn>
//
// Climbing mechanics: TestForClimb scans the world around a ped for
// climbable surfaces. TestForVault/TestForStandUp test specific climb
// outcomes. These are called every frame while a ped is near geometry.

#include "StdInc.h"
#include "TestFramework.h"
#include "Tasks/TaskTypes/TaskSimpleClimb.h"
#include "ScenarioHelpers.h"

// ======================================================================
// TestForClimb — scan for climbable surfaces near the player
// This is the main climbing detection function, called by AI and player code.
// Returns the entity to climb (or null if nothing climbable).
// ======================================================================

GAME_DIFF_TEST(CTaskSimpleClimb, TestForClimb_Player) {
    auto* player = FindPlayerPed(0);
    if (!player) return;

    // Test at current player position
    CVector origPos, revPos;
    float origHeading, revHeading;
    eSurfaceType origSurface, revSurface;

    CEntity* origEntity;
    { HookDisableGuard guard("Tasks/TaskTypes/CTaskSimpleClimb/TestForClimb");
      origEntity = CTaskSimpleClimb::TestForClimb(player, origPos, origHeading, origSurface, false); }

    CEntity* revEntity;
    revEntity = CTaskSimpleClimb::TestForClimb(player, revPos, revHeading, revSurface, false);

    EXPECT_EQ(origEntity, revEntity);
    if (origEntity && revEntity) {
        EXPECT_NEAR(origPos.x, revPos.x, 1e-3f);
        EXPECT_NEAR(origPos.y, revPos.y, 1e-3f);
        EXPECT_NEAR(origPos.z, revPos.z, 1e-3f);
        EXPECT_NEAR(origHeading, revHeading, 1e-3f);
        EXPECT_EQ((int)origSurface, (int)revSurface);
    }
}

GAME_DIFF_TEST(CTaskSimpleClimb, TestForClimb_Launch) {
    auto* player = FindPlayerPed(0);
    if (!player) return;

    CVector origPos, revPos;
    float origHeading, revHeading;
    eSurfaceType origSurface, revSurface;

    CEntity* origEntity;
    { HookDisableGuard guard("Tasks/TaskTypes/CTaskSimpleClimb/TestForClimb");
      origEntity = CTaskSimpleClimb::TestForClimb(player, origPos, origHeading, origSurface, true); }

    CEntity* revEntity;
    revEntity = CTaskSimpleClimb::TestForClimb(player, revPos, revHeading, revSurface, true);

    EXPECT_EQ(origEntity, revEntity);
    if (origEntity && revEntity) {
        EXPECT_NEAR(origPos.x, revPos.x, 1e-3f);
        EXPECT_NEAR(origPos.y, revPos.y, 1e-3f);
        EXPECT_NEAR(origPos.z, revPos.z, 1e-3f);
        EXPECT_NEAR(origHeading, revHeading, 1e-3f);
    }
}

// ======================================================================
// TestForClimb on spawned ped at different positions
// ======================================================================

GAME_DIFF_TEST(CTaskSimpleClimb, TestForClimb_SpawnedPed) {
    int32 pedModel = Scenario::FindLoadedPedModel();
    if (pedModel < 0) return;

    // Test at a few positions (near buildings in LS)
    CVector positions[] = {
        {2488.f, -1666.f, 13.5f},   // near LS police station
        {2400.f, -1640.f, 13.5f},   // LS street
    };

    for (auto& pos : positions) {
        Scenario::TestPed ped(PED_TYPE_CIVMALE, pedModel, pos);
        if (!ped) continue;

        CVector origPos, revPos;
        float origHeading, revHeading;
        eSurfaceType origSurface, revSurface;

        CEntity* origEntity;
        { HookDisableGuard guard("Tasks/TaskTypes/CTaskSimpleClimb/TestForClimb");
          origEntity = CTaskSimpleClimb::TestForClimb(ped.AsPed(), origPos, origHeading, origSurface, false); }

        CEntity* revEntity;
        revEntity = CTaskSimpleClimb::TestForClimb(ped.AsPed(), revPos, revHeading, revSurface, false);

        EXPECT_EQ(origEntity, revEntity);
        if (origEntity && revEntity) {
            EXPECT_NEAR(origPos.x, revPos.x, 1e-3f);
            EXPECT_NEAR(origPos.y, revPos.y, 1e-3f);
            EXPECT_NEAR(origPos.z, revPos.z, 1e-3f);
            EXPECT_NEAR(origHeading, revHeading, 1e-3f);
            EXPECT_EQ((int)origSurface, (int)revSurface);
        }
    }
}

// ======================================================================
// ScanToGrab — lower-level scan for grab points
// ======================================================================

GAME_DIFF_TEST(CTaskSimpleClimb, ScanToGrab) {
    auto* player = FindPlayerPed(0);
    if (!player) return;

    CVector playerPos = player->GetPosition();

    // Test with various flags
    struct Case { bool flag1; bool standUp; bool vault; };
    Case cases[] = {
        {false, false, false},
        {true, false, false},
        {false, true, false},
        {false, false, true},
    };

    for (auto& c : cases) {
        CVector origPos, revPos;
        float origAngle, revAngle;
        eSurfaceType origSurface, revSurface;

        CEntity* origEntity;
        { HookDisableGuard guard("Tasks/TaskTypes/CTaskSimpleClimb/ScanToGrab");
          origEntity = CTaskSimpleClimb::ScanToGrab(player, origPos, origAngle, origSurface, c.flag1, c.standUp, c.vault, &playerPos); }

        CEntity* revEntity;
        revEntity = CTaskSimpleClimb::ScanToGrab(player, revPos, revAngle, revSurface, c.flag1, c.standUp, c.vault, &playerPos);

        EXPECT_EQ(origEntity, revEntity);
        if (origEntity && revEntity) {
            EXPECT_NEAR(origPos.x, revPos.x, 1e-3f);
            EXPECT_NEAR(origPos.y, revPos.y, 1e-3f);
            EXPECT_NEAR(origPos.z, revPos.z, 1e-3f);
            EXPECT_NEAR(origAngle, revAngle, 1e-3f);
        }
    }
}

// ======================================================================
// TestForVault / TestForStandUp — instance methods on a climb task
// Need a task with a valid climb entity to test meaningfully.
// Create a task with the player's current position.
// ======================================================================

// TestForVault crashes in original code when called on a task with no
// climb entity — the vault test traces physics geometry that requires
// a real entity reference. Would need an active climbing state to test.

GAME_DIFF_TEST(CTaskSimpleClimb, TestForStandUp) {
    auto* player = FindPlayerPed(0);
    if (!player) return;

    CVector climbPos = player->GetPosition() + CVector(0, 0, 1.5f);
    float heading = 0.f;

    CTaskSimpleClimb task(nullptr, climbPos, heading, SURFACE_DEFAULT, CLIMB_GRAB, false);

    bool orig, rev;
    { HookDisableGuard guard("Tasks/TaskTypes/CTaskSimpleClimb/TestForStandUp");
      orig = task.TestForStandUp(player, climbPos, heading); }
    rev = task.TestForStandUp(player, climbPos, heading);
    EXPECT_EQ(orig, rev);
}

// ======================================================================
// GetCameraTargetPos — where camera should look during climb
// ======================================================================

GAME_DIFF_TEST(CTaskSimpleClimb, GetCameraTargetPos) {
    auto* player = FindPlayerPed(0);
    if (!player) return;

    CVector climbPos = player->GetPosition() + CVector(0, 0, 2.0f);
    CTaskSimpleClimb task(nullptr, climbPos, 0.f, SURFACE_DEFAULT, CLIMB_PULLUP, false);

    CVector origTarget, revTarget;
    { HookDisableGuard guard("Tasks/TaskTypes/CTaskSimpleClimb/GetCameraTargetPos");
      task.GetCameraTargetPos(player, origTarget); }
    task.GetCameraTargetPos(player, revTarget);

    EXPECT_NEAR(origTarget.x, revTarget.x, 1e-3f);
    EXPECT_NEAR(origTarget.y, revTarget.y, 1e-3f);
    EXPECT_NEAR(origTarget.z, revTarget.z, 1e-3f);
}

// ======================================================================
// GetCameraStickModifier — camera behavior during climb
// ======================================================================

GAME_DIFF_TEST(CTaskSimpleClimb, GetCameraStickModifier) {
    auto* player = FindPlayerPed(0);
    if (!player) return;

    CVector climbPos = player->GetPosition() + CVector(0, 0, 2.0f);
    CTaskSimpleClimb task(nullptr, climbPos, 0.f, SURFACE_DEFAULT, CLIMB_PULLUP, false);

    float origV = 0, origH = 0, origA5 = 0, origA6 = 0;
    float revV = 0, revH = 0, revA5 = 0, revA6 = 0;
    { HookDisableGuard guard("Tasks/TaskTypes/CTaskSimpleClimb/GetCameraStickModifier");
      task.GetCameraStickModifier(player, origV, origH, origA5, origA6); }
    task.GetCameraStickModifier(player, revV, revH, revA5, revA6);

    EXPECT_NEAR(origV, revV, 1e-3f);
    EXPECT_NEAR(origH, revH, 1e-3f);
    EXPECT_NEAR(origA5, revA5, 1e-3f);
    EXPECT_NEAR(origA6, revA6, 1e-3f);
}

// ======================================================================
// MakeAbortable
// ======================================================================

GAME_DIFF_TEST(CTaskSimpleClimb, MakeAbortable) {
    auto* player = FindPlayerPed(0);
    if (!player) return;

    CVector climbPos = player->GetPosition() + CVector(0, 0, 1.5f);

    // Test with different priorities
    eAbortPriority priorities[] = { ABORT_PRIORITY_LEISURE, ABORT_PRIORITY_URGENT, ABORT_PRIORITY_IMMEDIATE };
    for (auto prio : priorities) {
        CTaskSimpleClimb task1(nullptr, climbPos, 0.f, SURFACE_DEFAULT, CLIMB_GRAB, false);
        bool orig;
        { HookDisableGuard guard("Tasks/TaskTypes/CTaskSimpleClimb/MakeAbortable");
          orig = task1.MakeAbortable(player, prio, nullptr); }

        CTaskSimpleClimb task2(nullptr, climbPos, 0.f, SURFACE_DEFAULT, CLIMB_GRAB, false);
        bool rev = task2.MakeAbortable(player, prio, nullptr);

        EXPECT_EQ(orig, rev);
    }
}

// ======================================================================
// CreateColModel — static, creates the collision model used for climb detection
// ======================================================================

GAME_DIFF_TEST(CTaskSimpleClimb, CreateColModel) {
    bool orig, rev;
    { HookDisableGuard guard("Tasks/TaskTypes/CTaskSimpleClimb/CreateColModel");
      orig = CTaskSimpleClimb::CreateColModel(); }
    rev = CTaskSimpleClimb::CreateColModel();
    EXPECT_EQ(orig, rev);
}
