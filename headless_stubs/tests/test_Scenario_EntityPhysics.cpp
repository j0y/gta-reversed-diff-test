// test_Scenario_EntityPhysics.cpp — Physics force application tests.

#include "StdInc.h"
#include "TestFramework.h"
#include "Entity/Physical.h"
#include "Entity/Vehicle/Vehicle.h"
#include "ScenarioHelpers.h"

static Scenario::TestVehicle* s_physVeh = nullptr;
static bool s_triedPhysVeh = false;

static CPhysical* GetPhysVeh() {
    if (!s_triedPhysVeh) {
        s_triedPhysVeh = true;
        int32 model = Scenario::FindLoadedVehicleModel();
        if (model >= 0)
            s_physVeh = new Scenario::TestVehicle(model, CVector(2392.f, -1660.f, 13.5f));
    }
    return s_physVeh ? static_cast<CPhysical*>(s_physVeh->AsVehicle()) : nullptr;
}

// --- ApplyForce: linear impulse at center ---

GAME_DIFF_TEST(Scenario_EntityPhysics, ApplyForce_LinearImpulse) {
    auto* phys = GetPhysVeh();
    if (!phys) return;

    CVector savedMove = phys->m_vecMoveSpeed;
    CVector savedTurn = phys->m_vecTurnSpeed;
    CVector zero(0.0f, 0.0f, 0.0f);
    CVector force(1.0f, 0.0f, 0.0f);

    phys->m_vecMoveSpeed = zero;
    phys->m_vecTurnSpeed = zero;
    { HookDisableGuard guard("Entity/CPhysical/ApplyForce");
      phys->ApplyForce(force, zero, true); }
    CVector origMove = phys->m_vecMoveSpeed;
    CVector origTurn = phys->m_vecTurnSpeed;

    phys->m_vecMoveSpeed = zero;
    phys->m_vecTurnSpeed = zero;
    phys->ApplyForce(force, zero, true);
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

// --- ApplyForce: off-center creates torque ---

GAME_DIFF_TEST(Scenario_EntityPhysics, ApplyForce_WithTorque) {
    auto* phys = GetPhysVeh();
    if (!phys) return;

    CVector savedMove = phys->m_vecMoveSpeed;
    CVector savedTurn = phys->m_vecTurnSpeed;
    CVector zero(0.0f, 0.0f, 0.0f);
    CVector force(0.0f, 1.0f, 0.0f);
    CVector offset(1.0f, 0.0f, 0.0f);

    phys->m_vecMoveSpeed = zero;
    phys->m_vecTurnSpeed = zero;
    { HookDisableGuard guard("Entity/CPhysical/ApplyForce");
      phys->ApplyForce(force, offset, true); }
    CVector origMove = phys->m_vecMoveSpeed;
    CVector origTurn = phys->m_vecTurnSpeed;

    phys->m_vecMoveSpeed = zero;
    phys->m_vecTurnSpeed = zero;
    phys->ApplyForce(force, offset, true);
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

// --- ApplyTurnForce ---

GAME_DIFF_TEST(Scenario_EntityPhysics, ApplyTurnForce_ZAxis) {
    auto* phys = GetPhysVeh();
    if (!phys) return;

    CVector savedTurn = phys->m_vecTurnSpeed;
    CVector zero(0.0f, 0.0f, 0.0f);
    CVector force(1.0f, 0.0f, 0.0f);
    CVector offset(0.0f, 1.0f, 0.0f);

    phys->m_vecTurnSpeed = zero;
    { HookDisableGuard guard("Entity/CPhysical/ApplyTurnForce");
      phys->ApplyTurnForce(force, offset); }
    CVector origTurn = phys->m_vecTurnSpeed;

    phys->m_vecTurnSpeed = zero;
    phys->ApplyTurnForce(force, offset);
    CVector revTurn = phys->m_vecTurnSpeed;

    EXPECT_NEAR(origTurn.x, revTurn.x, 1e-5f);
    EXPECT_NEAR(origTurn.y, revTurn.y, 1e-5f);
    EXPECT_NEAR(origTurn.z, revTurn.z, 1e-5f);

    phys->m_vecTurnSpeed = savedTurn;
}

// --- Multiple forces in sequence ---

GAME_DIFF_TEST(Scenario_EntityPhysics, ApplyForce_Sequence) {
    auto* phys = GetPhysVeh();
    if (!phys) return;

    CVector savedMove = phys->m_vecMoveSpeed;
    CVector savedTurn = phys->m_vecTurnSpeed;
    CVector zero(0.0f, 0.0f, 0.0f);

    struct ForceAndOffset { CVector force, offset; };
    ForceAndOffset inputs[] = {
        { {1.0f, 0.0f, 0.0f}, {0.0f, 0.5f, 0.0f} },
        { {0.0f, -0.5f, 0.0f}, {0.5f, 0.0f, 0.0f} },
        { {0.0f, 0.0f, 2.0f}, {0.0f, 0.0f, 0.0f} },
    };

    phys->m_vecMoveSpeed = zero;
    phys->m_vecTurnSpeed = zero;
    { HookDisableGuard guard("Entity/CPhysical/ApplyForce");
      for (auto& inp : inputs)
          phys->ApplyForce(inp.force, inp.offset, true);
    }
    CVector origMove = phys->m_vecMoveSpeed;
    CVector origTurn = phys->m_vecTurnSpeed;

    phys->m_vecMoveSpeed = zero;
    phys->m_vecTurnSpeed = zero;
    for (auto& inp : inputs)
        phys->ApplyForce(inp.force, inp.offset, true);
    CVector revMove = phys->m_vecMoveSpeed;
    CVector revTurn = phys->m_vecTurnSpeed;

    EXPECT_NEAR(origMove.x, revMove.x, 1e-4f);
    EXPECT_NEAR(origMove.y, revMove.y, 1e-4f);
    EXPECT_NEAR(origMove.z, revMove.z, 1e-4f);
    EXPECT_NEAR(origTurn.x, revTurn.x, 1e-4f);
    EXPECT_NEAR(origTurn.y, revTurn.y, 1e-4f);
    EXPECT_NEAR(origTurn.z, revTurn.z, 1e-4f);

    phys->m_vecMoveSpeed = savedMove;
    phys->m_vecTurnSpeed = savedTurn;
}
