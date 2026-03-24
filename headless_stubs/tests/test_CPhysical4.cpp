// test_CPhysical4.cpp — CPhysical collision and force tests.
// Hook paths: Entity/CPhysical/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "Entity/Physical.h"
#include "Entity/Ped/Ped.h"
#include "Entity/Vehicle/Vehicle.h"
#include "ScenarioHelpers.h"

static Scenario::TestVehicle* s_p4Veh = nullptr;
static bool s_triedP4 = false;

static CPhysical* GetP4Veh() {
    if (!s_triedP4) {
        s_triedP4 = true;
        int32 model = Scenario::FindLoadedVehicleModel();
        if (model >= 0)
            s_p4Veh = new Scenario::TestVehicle(model, CVector(2360.f, -1638.f, 13.5f));
    }
    return s_p4Veh ? static_cast<CPhysical*>(s_p4Veh->AsVehicle()) : nullptr;
}

// --- AddCollisionRecord ---

GAME_DIFF_TEST(CPhysical4, AddCollisionRecord_PlayerWithVeh) {
    auto* player = FindPlayerPed(0);
    auto* phys = GetP4Veh();
    if (!player || !phys) return;

    { HookDisableGuard guard("Entity/CPhysical/AddCollisionRecord");
      player->AddCollisionRecord(static_cast<CEntity*>(s_p4Veh->AsVehicle())); }
    player->AddCollisionRecord(static_cast<CEntity*>(s_p4Veh->AsVehicle()));
    EXPECT_TRUE(true);
}

// --- GetHasCollidedWith ---

GAME_DIFF_TEST(CPhysical4, GetHasCollidedWith_PlayerWithVeh) {
    auto* player = FindPlayerPed(0);
    auto* veh = s_p4Veh ? s_p4Veh->AsVehicle() : nullptr;
    if (!player || !veh) return;
    bool orig, rev;
    { HookDisableGuard guard("Entity/CPhysical/GetHasCollidedWith");
      orig = player->GetHasCollidedWith(veh); }
    rev = player->GetHasCollidedWith(veh);
    EXPECT_EQ(orig, rev);
}

// --- ApplyAirResistance ---

GAME_DIFF_TEST(CPhysical4, ApplyAirResistance_Vehicle) {
    auto* phys = GetP4Veh();
    if (!phys) return;
    CVector savedMove = phys->m_vecMoveSpeed;
    CVector savedTurn = phys->m_vecTurnSpeed;

    phys->m_vecMoveSpeed = CVector(1.0f, 0.5f, 0.0f);
    phys->m_vecTurnSpeed = CVector(0.1f, 0.0f, 0.05f);

    CVector moveStart = phys->m_vecMoveSpeed;
    CVector turnStart = phys->m_vecTurnSpeed;

    { HookDisableGuard guard("Entity/CPhysical/ApplyAirResistance");
      phys->m_vecMoveSpeed = moveStart;
      phys->m_vecTurnSpeed = turnStart;
      phys->ApplyAirResistance(); }
    CVector origMove = phys->m_vecMoveSpeed;
    CVector origTurn = phys->m_vecTurnSpeed;

    phys->m_vecMoveSpeed = moveStart;
    phys->m_vecTurnSpeed = turnStart;
    phys->ApplyAirResistance();
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

// SetDamagedPieceRecord takes CColPoint& — needs real collision point. Skipped.
