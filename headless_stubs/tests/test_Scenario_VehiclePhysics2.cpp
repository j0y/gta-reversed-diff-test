// test_Scenario_VehiclePhysics2.cpp — Advanced vehicle physics differential tests.

#include "StdInc.h"
#include "TestFramework.h"
#include "Entity/Vehicle/Vehicle.h"
#include "Entity/Physical.h"
#include "ScenarioHelpers.h"

static Scenario::TestVehicle* s_vp2 = nullptr;
static bool s_triedVp2 = false;

static CVehicle* GetVP2() {
    if (!s_triedVp2) {
        s_triedVp2 = true;
        int32 model = Scenario::FindLoadedVehicleModel();
        if (model >= 0)
            s_vp2 = new Scenario::TestVehicle(model, CVector(2350.f, -1632.f, 13.5f));
    }
    return s_vp2 ? s_vp2->AsVehicle() : nullptr;
}

// --- ApplyFrictionForce (self) ---

GAME_DIFF_TEST(Scenario_VehiclePhysics2, ApplyFrictionForce_Self) {
    auto* veh = GetVP2();
    if (!veh) return;
    CVector savedMove = veh->m_vecMoveSpeed;
    CVector savedTurn = veh->m_vecTurnSpeed;
    CVector force(0.05f, 0.0f, 0.0f);
    CVector point(0.0f, 1.0f, 0.0f);

    veh->m_vecMoveSpeed = CVector(0.5f, 0.0f, 0.0f);
    veh->m_vecTurnSpeed = CVector(0.0f, 0.0f, 0.0f);
    { HookDisableGuard guard("Entity/CPhysical/ApplyFrictionForce");
      veh->ApplyFrictionForce(force, point); }
    CVector origMove = veh->m_vecMoveSpeed;
    CVector origTurn = veh->m_vecTurnSpeed;

    veh->m_vecMoveSpeed = CVector(0.5f, 0.0f, 0.0f);
    veh->m_vecTurnSpeed = CVector(0.0f, 0.0f, 0.0f);
    veh->ApplyFrictionForce(force, point);
    CVector revMove = veh->m_vecMoveSpeed;
    CVector revTurn = veh->m_vecTurnSpeed;

    EXPECT_NEAR(origMove.x, revMove.x, 1e-5f);
    EXPECT_NEAR(origMove.y, revMove.y, 1e-5f);
    EXPECT_NEAR(origTurn.z, revTurn.z, 1e-5f);

    veh->m_vecMoveSpeed = savedMove;
    veh->m_vecTurnSpeed = savedTurn;
}

// --- Multiple sequential forces ---

GAME_DIFF_TEST(Scenario_VehiclePhysics2, SequentialForces) {
    auto* veh = GetVP2();
    if (!veh) return;
    CVector savedMove = veh->m_vecMoveSpeed;
    CVector savedTurn = veh->m_vecTurnSpeed;
    CVector zero(0.0f, 0.0f, 0.0f);

    // Apply gravity + air resistance + turn force in sequence
    veh->m_vecMoveSpeed = CVector(1.0f, 0.0f, 0.0f);
    veh->m_vecTurnSpeed = CVector(0.0f, 0.0f, 0.1f);
    { HookDisableGuard guard("Entity/CPhysical/ApplyGravity");
      veh->ApplyGravity(); }
    { HookDisableGuard guard("Entity/CPhysical/ApplyAirResistance");
      veh->ApplyAirResistance(); }
    CVector origMove = veh->m_vecMoveSpeed;
    CVector origTurn = veh->m_vecTurnSpeed;

    veh->m_vecMoveSpeed = CVector(1.0f, 0.0f, 0.0f);
    veh->m_vecTurnSpeed = CVector(0.0f, 0.0f, 0.1f);
    veh->ApplyGravity();
    veh->ApplyAirResistance();
    CVector revMove = veh->m_vecMoveSpeed;
    CVector revTurn = veh->m_vecTurnSpeed;

    EXPECT_NEAR(origMove.x, revMove.x, 1e-5f);
    EXPECT_NEAR(origMove.y, revMove.y, 1e-5f);
    EXPECT_NEAR(origMove.z, revMove.z, 1e-5f);
    EXPECT_NEAR(origTurn.x, revTurn.x, 1e-5f);
    EXPECT_NEAR(origTurn.y, revTurn.y, 1e-5f);
    EXPECT_NEAR(origTurn.z, revTurn.z, 1e-5f);

    veh->m_vecMoveSpeed = savedMove;
    veh->m_vecTurnSpeed = savedTurn;
}

// --- CalculateDriveAcceleration across 10 vehicles × 5 throttle values ---

GAME_DIFF_TEST(Scenario_VehiclePhysics2, CalculateDriveAcceleration) {
    for (int32 id = 0; id < 10; id++) {
        tHandlingData* h = gHandlingDataMgr.GetVehiclePointer(id);
        if (h->m_fMass == 0.0f) continue;
        auto& trans = h->GetTransmission();
        float throttles[] = { 0.0f, 0.5f, 1.0f, -0.5f, -1.0f };
        for (float throttle : throttles) {
            float origAccel = 0.f, revAccel = 0.f;
            uint8 origGear = 3, revGear = 3;
            float speed = 0.3f;
            { HookDisableGuard guard("Global/cTransmission/CalculateDriveAcceleration");
              trans.CalculateDriveAcceleration(throttle, origGear, origAccel, speed, nullptr, nullptr, false, false); }
            trans.CalculateDriveAcceleration(throttle, revGear, revAccel, speed, nullptr, nullptr, false, false);
            EXPECT_NEAR(origAccel, revAccel, 1e-4f);
            EXPECT_EQ(origGear, revGear);
        }
    }
}

// --- CWorld::FindObjectsInRange at multiple positions ---

GAME_DIFF_TEST(Scenario_VehiclePhysics2, FindObjectsInRange) {
    CVector positions[] = {
        {2488.f, -1666.f, 13.f},
        {0.f, 0.f, 0.f},
        {-2170.f, 250.f, 30.f},
    };
    for (auto& pos : positions) {
        int16 origCount = 0, revCount = 0;
        CEntity* origEnts[32] = {};
        CEntity* revEnts[32] = {};
        { HookDisableGuard guard("Global/CWorld/FindObjectsInRange");
          CWorld::FindObjectsInRange(pos, 50.f, true, &origCount, 32, origEnts, false, true, true, false, false); }
        CWorld::FindObjectsInRange(pos, 50.f, true, &revCount, 32, revEnts, false, true, true, false, false);
        EXPECT_EQ(origCount, revCount);
        for (int i = 0; i < origCount && i < revCount; i++) {
            EXPECT_EQ(origEnts[i], revEnts[i]);
        }
    }
}
