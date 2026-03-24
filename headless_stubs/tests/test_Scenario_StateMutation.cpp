// test_Scenario_StateMutation.cpp — Advanced state mutation differential tests.

#include "StdInc.h"
#include "TestFramework.h"
#include "Entity/Ped/Ped.h"
#include "Entity/Vehicle/Vehicle.h"
#include "Entity/Physical.h"
#include "ScenarioHelpers.h"

// ============================================================
// CPed state mutation tests
// ============================================================

// --- SetPedState round-trip ---

GAME_DIFF_TEST(Scenario_StateMutation, SetPedState_RoundTrip) {
    auto* player = FindPlayerPed(0);
    if (!player) return;

    ePedState savedState = player->m_nPedState;

    ePedState testStates[] = {
        PEDSTATE_IDLE, PEDSTATE_FLEE_ENTITY, PEDSTATE_ATTACK,
        PEDSTATE_DEAD, PEDSTATE_DRIVING,
    };
    for (auto state : testStates) {
        player->m_nPedState = savedState;
        { HookDisableGuard guard("Entity/Ped/CPed/SetPedState");
          player->SetPedState(state); }
        ePedState origResult = player->m_nPedState;

        player->m_nPedState = savedState;
        player->SetPedState(state);
        ePedState revResult = player->m_nPedState;

        EXPECT_EQ((int32)origResult, (int32)revResult);
    }
    player->m_nPedState = savedState;
}

// ============================================================
// CPhysical state mutation tests
// ============================================================

// --- ApplyMoveSpeed ---

GAME_DIFF_TEST(Scenario_StateMutation, ApplyMoveSpeed_ComparePosition) {
    auto* player = FindPlayerPed(0);
    if (!player) return;

    CMatrix savedMatrix = player->GetMatrix();
    CVector savedMove = player->m_vecMoveSpeed;

    player->m_vecMoveSpeed = CVector(0.1f, 0.0f, 0.0f);

    player->SetMatrix(savedMatrix);
    { HookDisableGuard guard("Entity/CPhysical/ApplyMoveSpeed");
      player->ApplyMoveSpeed(); }
    CVector origPos = player->GetPosition();

    player->SetMatrix(savedMatrix);
    player->ApplyMoveSpeed();
    CVector revPos = player->GetPosition();

    EXPECT_NEAR(origPos.x, revPos.x, 1e-4f);
    EXPECT_NEAR(origPos.y, revPos.y, 1e-4f);
    EXPECT_NEAR(origPos.z, revPos.z, 1e-4f);

    player->SetMatrix(savedMatrix);
    player->m_vecMoveSpeed = savedMove;
}

// --- ApplyTurnSpeed ---

GAME_DIFF_TEST(Scenario_StateMutation, ApplyTurnSpeed_CompareMatrix) {
    auto* player = FindPlayerPed(0);
    if (!player) return;

    CMatrix savedMatrix = player->GetMatrix();
    CVector savedTurn = player->m_vecTurnSpeed;

    player->m_vecTurnSpeed = CVector(0.0f, 0.0f, 0.05f);

    player->SetMatrix(savedMatrix);
    { HookDisableGuard guard("Entity/CPhysical/ApplyTurnSpeed");
      player->ApplyTurnSpeed(); }
    CVector origFwd = player->GetForward();

    player->SetMatrix(savedMatrix);
    player->ApplyTurnSpeed();
    CVector revFwd = player->GetForward();

    EXPECT_NEAR(origFwd.x, revFwd.x, 1e-4f);
    EXPECT_NEAR(origFwd.y, revFwd.y, 1e-4f);
    EXPECT_NEAR(origFwd.z, revFwd.z, 1e-4f);

    player->SetMatrix(savedMatrix);
    player->m_vecTurnSpeed = savedTurn;
}

// ============================================================
// Vehicle mutation tests
// ============================================================

static Scenario::TestVehicle* s_mutVeh = nullptr;
static bool s_triedMutVeh = false;

static CVehicle* GetMutVehicle() {
    if (!s_triedMutVeh) {
        s_triedMutVeh = true;
        int32 model = Scenario::FindLoadedVehicleModel();
        if (model >= 0)
            s_mutVeh = new Scenario::TestVehicle(model, CVector(2394.f, -1662.f, 13.5f));
    }
    return s_mutVeh ? s_mutVeh->AsVehicle() : nullptr;
}

// --- Vehicle ApplyMoveSpeed ---

GAME_DIFF_TEST(Scenario_StateMutation, Vehicle_ApplyMoveSpeed) {
    auto* veh = GetMutVehicle();
    if (!veh) return;

    CMatrix savedMatrix = veh->GetMatrix();
    CVector savedMove = veh->m_vecMoveSpeed;

    veh->m_vecMoveSpeed = CVector(0.0f, 0.2f, 0.0f);

    veh->SetMatrix(savedMatrix);
    { HookDisableGuard guard("Entity/CPhysical/ApplyMoveSpeed");
      veh->ApplyMoveSpeed(); }
    CVector origPos = veh->GetPosition();

    veh->SetMatrix(savedMatrix);
    veh->ApplyMoveSpeed();
    CVector revPos = veh->GetPosition();

    EXPECT_NEAR(origPos.x, revPos.x, 1e-4f);
    EXPECT_NEAR(origPos.y, revPos.y, 1e-4f);
    EXPECT_NEAR(origPos.z, revPos.z, 1e-4f);

    veh->SetMatrix(savedMatrix);
    veh->m_vecMoveSpeed = savedMove;
}

// --- ApplyFrictionMoveForce ---

GAME_DIFF_TEST(Scenario_StateMutation, Vehicle_ApplyFrictionMoveForce) {
    auto* veh = GetMutVehicle();
    if (!veh) return;

    CVector savedMove = veh->m_vecMoveSpeed;
    CVector force(0.05f, 0.03f, 0.0f);

    veh->m_vecMoveSpeed = savedMove;
    { HookDisableGuard guard("Entity/CPhysical/ApplyFrictionMoveForce");
      veh->ApplyFrictionMoveForce(force); }
    CVector origSpeed = veh->m_vecMoveSpeed;

    veh->m_vecMoveSpeed = savedMove;
    veh->ApplyFrictionMoveForce(force);
    CVector revSpeed = veh->m_vecMoveSpeed;

    EXPECT_NEAR(origSpeed.x, revSpeed.x, 1e-5f);
    EXPECT_NEAR(origSpeed.y, revSpeed.y, 1e-5f);
    EXPECT_NEAR(origSpeed.z, revSpeed.z, 1e-5f);

    veh->m_vecMoveSpeed = savedMove;
}
