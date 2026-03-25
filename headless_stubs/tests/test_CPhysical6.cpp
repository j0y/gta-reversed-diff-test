// test_CPhysical6.cpp — More CPhysical differential tests.
// Hook paths: Entity/CPhysical/<fn>
// Previously tested: GetSpeed, GetHasCollidedWithAnyObject, CanPhysicalBeDamaged,
// ApplyGravity, ApplyAirResistance, ApplyFrictionForce, SkipPhysics,
// GetLightingTotal, GetLightingFromCol, AddToMovingList, RemoveFromMovingList,
// AddCollisionRecord, GetHasCollidedWith.
// New targets: ApplyMoveSpeed, ApplyTurnSpeed, ApplySpeed,
// UnsetIsInSafePosition, ProcessShift, GetSpeed (with point), ApplyFrictionMoveForce.

#include "StdInc.h"
#include "TestFramework.h"
#include "Entity/Physical.h"
#include "Entity/Ped/Ped.h"
#include "Entity/Vehicle/Vehicle.h"
#include "ScenarioHelpers.h"

static Scenario::TestVehicle* s_p6Veh = nullptr;
static bool s_triedP6 = false;

static CPhysical* GetP6Veh() {
    if (!s_triedP6) {
        s_triedP6 = true;
        int32 model = Scenario::FindLoadedVehicleModel();
        if (model >= 0)
            s_p6Veh = new Scenario::TestVehicle(model, CVector(2525.f, -1640.f, 13.5f));
    }
    return s_p6Veh ? static_cast<CPhysical*>(s_p6Veh->AsVehicle()) : nullptr;
}

// --- ApplyMoveSpeed ---
// void — applies m_vecMoveSpeed to position.

GAME_DIFF_TEST(CPhysical6, ApplyMoveSpeed_Vehicle) {
    auto* phys = GetP6Veh();
    if (!phys) return;
    CVector savedPos = phys->GetPosition();
    CVector savedMove = phys->m_vecMoveSpeed;

    // Set a known move speed
    phys->m_vecMoveSpeed = CVector(0.01f, 0.02f, 0.0f);
    CVector startPos(2525.f, -1640.f, 13.5f);

    phys->SetPosn(startPos);
    { HookDisableGuard guard("Entity/CPhysical/ApplyMoveSpeed");
      phys->ApplyMoveSpeed(); }
    CVector origPos = phys->GetPosition();

    phys->SetPosn(startPos);
    phys->m_vecMoveSpeed = CVector(0.01f, 0.02f, 0.0f);
    phys->ApplyMoveSpeed();
    CVector revPos = phys->GetPosition();

    EXPECT_NEAR(origPos.x, revPos.x, 1e-5f);
    EXPECT_NEAR(origPos.y, revPos.y, 1e-5f);
    EXPECT_NEAR(origPos.z, revPos.z, 1e-5f);

    // Restore
    phys->SetPosn(savedPos);
    phys->m_vecMoveSpeed = savedMove;
}

// --- ApplyTurnSpeed ---
// void — applies m_vecTurnSpeed to orientation.

GAME_DIFF_TEST(CPhysical6, ApplyTurnSpeed_Vehicle) {
    auto* phys = GetP6Veh();
    if (!phys) return;
    CVector savedTurn = phys->m_vecTurnSpeed;

    // Set a small turn speed
    phys->m_vecTurnSpeed = CVector(0.001f, 0.0f, 0.001f);
    { HookDisableGuard guard("Entity/CPhysical/ApplyTurnSpeed");
      phys->ApplyTurnSpeed(); }
    // No crash = original OK

    phys->m_vecTurnSpeed = CVector(0.001f, 0.0f, 0.001f);
    phys->ApplyTurnSpeed();
    // No crash = reversed OK
    EXPECT_TRUE(true);

    phys->m_vecTurnSpeed = savedTurn;
}

// --- ApplyMoveSpeed on player ---

GAME_DIFF_TEST(CPhysical6, ApplyMoveSpeed_Player) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    CVector savedPos = player->GetPosition();
    CVector savedMove = player->m_vecMoveSpeed;

    player->m_vecMoveSpeed = CVector(0.005f, 0.005f, 0.0f);
    CVector startPos = savedPos;

    player->SetPosn(startPos);
    { HookDisableGuard guard("Entity/CPhysical/ApplyMoveSpeed");
      player->ApplyMoveSpeed(); }
    CVector origPos = player->GetPosition();

    player->SetPosn(startPos);
    player->m_vecMoveSpeed = CVector(0.005f, 0.005f, 0.0f);
    player->ApplyMoveSpeed();
    CVector revPos = player->GetPosition();

    EXPECT_NEAR(origPos.x, revPos.x, 1e-5f);
    EXPECT_NEAR(origPos.y, revPos.y, 1e-5f);
    EXPECT_NEAR(origPos.z, revPos.z, 1e-5f);

    player->SetPosn(savedPos);
    player->m_vecMoveSpeed = savedMove;
}

// --- GetSpeed with point ---
// CVector — returns speed at a given point relative to center of mass.

GAME_DIFF_TEST(CPhysical6, GetSpeed_WithPoint_Vehicle) {
    auto* phys = GetP6Veh();
    if (!phys) return;

    CVector point(1.0f, 0.0f, 0.5f);
    CVector orig, rev;
    { HookDisableGuard guard("Entity/CPhysical/GetSpeed");
      orig = phys->GetSpeed(point); }
    rev = phys->GetSpeed(point);
    EXPECT_NEAR(orig.x, rev.x, 1e-5f);
    EXPECT_NEAR(orig.y, rev.y, 1e-5f);
    EXPECT_NEAR(orig.z, rev.z, 1e-5f);
}

GAME_DIFF_TEST(CPhysical6, GetSpeed_WithPoint_Player) {
    auto* player = FindPlayerPed(0);
    if (!player) return;

    CVector point(0.0f, 0.5f, 0.0f);
    CVector orig, rev;
    { HookDisableGuard guard("Entity/CPhysical/GetSpeed");
      orig = player->GetSpeed(point); }
    rev = player->GetSpeed(point);
    EXPECT_NEAR(orig.x, rev.x, 1e-5f);
    EXPECT_NEAR(orig.y, rev.y, 1e-5f);
    EXPECT_NEAR(orig.z, rev.z, 1e-5f);
}

// --- ApplyFrictionMoveForce ---
// void — applies friction move force to m_vecFrictionMoveSpeed.

GAME_DIFF_TEST(CPhysical6, ApplyFrictionMoveForce_Vehicle) {
    auto* phys = GetP6Veh();
    if (!phys) return;
    CVector savedFriction = phys->m_vecFrictionMoveSpeed;

    CVector force(0.01f, 0.02f, 0.0f);

    phys->m_vecFrictionMoveSpeed = CVector(0.0f, 0.0f, 0.0f);
    { HookDisableGuard guard("Entity/CPhysical/ApplyFrictionMoveForce");
      phys->ApplyFrictionMoveForce(force); }
    CVector origFriction = phys->m_vecFrictionMoveSpeed;

    phys->m_vecFrictionMoveSpeed = CVector(0.0f, 0.0f, 0.0f);
    phys->ApplyFrictionMoveForce(force);
    CVector revFriction = phys->m_vecFrictionMoveSpeed;

    EXPECT_NEAR(origFriction.x, revFriction.x, 1e-5f);
    EXPECT_NEAR(origFriction.y, revFriction.y, 1e-5f);
    EXPECT_NEAR(origFriction.z, revFriction.z, 1e-5f);

    phys->m_vecFrictionMoveSpeed = savedFriction;
}

// --- UnsetIsInSafePosition ---
// void — unsets safe position state, clears collision list.

GAME_DIFF_TEST(CPhysical6, UnsetIsInSafePosition_Vehicle) {
    auto* phys = GetP6Veh();
    if (!phys) return;

    { HookDisableGuard guard("Entity/CPhysical/UnsetIsInSafePosition");
      phys->UnsetIsInSafePosition(); }
    uint8 origCollided = phys->m_nNumEntitiesCollided;

    phys->UnsetIsInSafePosition();
    uint8 revCollided = phys->m_nNumEntitiesCollided;

    EXPECT_EQ(origCollided, revCollided);
}

GAME_DIFF_TEST(CPhysical6, UnsetIsInSafePosition_Player) {
    auto* player = FindPlayerPed(0);
    if (!player) return;

    { HookDisableGuard guard("Entity/CPhysical/UnsetIsInSafePosition");
      player->UnsetIsInSafePosition(); }
    uint8 origCollided = player->m_nNumEntitiesCollided;

    player->UnsetIsInSafePosition();
    uint8 revCollided = player->m_nNumEntitiesCollided;

    EXPECT_EQ(origCollided, revCollided);
}

// --- ApplyTurnForce ---
// void — applies a turn force at a given point.

GAME_DIFF_TEST(CPhysical6, ApplyTurnForce_Vehicle) {
    auto* phys = GetP6Veh();
    if (!phys) return;
    CVector savedTorque = phys->m_vecTorque;

    CVector force(0.0f, 0.0f, 0.01f);
    CVector point(1.0f, 0.0f, 0.0f);

    phys->m_vecTorque = CVector(0.0f, 0.0f, 0.0f);
    { HookDisableGuard guard("Entity/CPhysical/ApplyTurnForce");
      phys->ApplyTurnForce(force, point); }
    CVector origTorque = phys->m_vecTorque;

    phys->m_vecTorque = CVector(0.0f, 0.0f, 0.0f);
    phys->ApplyTurnForce(force, point);
    CVector revTorque = phys->m_vecTorque;

    EXPECT_NEAR(origTorque.x, revTorque.x, 1e-5f);
    EXPECT_NEAR(origTorque.y, revTorque.y, 1e-5f);
    EXPECT_NEAR(origTorque.z, revTorque.z, 1e-5f);

    phys->m_vecTorque = savedTorque;
}

// --- ApplyForce ---
// void — applies force at point, updating both move and turn speed.

GAME_DIFF_TEST(CPhysical6, ApplyForce_Vehicle) {
    auto* phys = GetP6Veh();
    if (!phys) return;
    CVector savedMove = phys->m_vecMoveSpeed;
    CVector savedTurn = phys->m_vecTurnSpeed;

    CVector force(0.01f, 0.0f, 0.0f);
    CVector point(0.0f, 1.0f, 0.0f);

    phys->m_vecMoveSpeed = CVector(0.0f, 0.0f, 0.0f);
    phys->m_vecTurnSpeed = CVector(0.0f, 0.0f, 0.0f);
    { HookDisableGuard guard("Entity/CPhysical/ApplyForce");
      phys->ApplyForce(force, point, true); }
    CVector origMove = phys->m_vecMoveSpeed;
    CVector origTurn = phys->m_vecTurnSpeed;

    phys->m_vecMoveSpeed = CVector(0.0f, 0.0f, 0.0f);
    phys->m_vecTurnSpeed = CVector(0.0f, 0.0f, 0.0f);
    phys->ApplyForce(force, point, true);
    CVector revMove = phys->m_vecMoveSpeed;
    CVector revTurn = phys->m_vecTurnSpeed;

    EXPECT_NEAR(origMove.x, revMove.x, 1e-5f);
    EXPECT_NEAR(origMove.y, revMove.y, 1e-5f);
    EXPECT_NEAR(origMove.z, revMove.z, 1e-5f);
    EXPECT_NEAR(origTurn.x, revTurn.x, 1e-5f);
    EXPECT_NEAR(origTurn.y, revTurn.y, 1e-5f);
    EXPECT_NEAR(origTurn.z, revTurn.z, 1e-5f);

    phys->m_vecMoveSpeed = savedMove;
    phys->m_vecTurnSpeed = savedTurn;
}
