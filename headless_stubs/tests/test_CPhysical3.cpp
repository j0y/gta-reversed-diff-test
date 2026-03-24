// test_CPhysical3.cpp — Extended differential tests for CPhysical.
// Hook paths: Entity/CPhysical/<fn>
//
// Tests additional CPhysical hooks on player ped and spawned vehicle.

#include "StdInc.h"
#include "TestFramework.h"
#include "Entity/Physical.h"
#include "Entity/Ped/Ped.h"
#include "Entity/Vehicle/Vehicle.h"
#include "ScenarioHelpers.h"

// --- ApplyAirResistance on player ---

GAME_DIFF_TEST(CPhysical3, ApplyGravity_Player) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    // Save state before test
    CVector origSpeed = player->m_vecMoveSpeed;
    // Call with hook disabled (original)
    { HookDisableGuard guard("Entity/CPhysical/ApplyGravity");
      player->m_vecMoveSpeed = origSpeed;
      player->ApplyGravity(); }
    CVector afterOrig = player->m_vecMoveSpeed;
    // Call with hook enabled (reversed)
    player->m_vecMoveSpeed = origSpeed;
    player->ApplyGravity();
    CVector afterRev = player->m_vecMoveSpeed;
    // Compare
    EXPECT_NEAR(afterOrig.x, afterRev.x, 1e-5f);
    EXPECT_NEAR(afterOrig.y, afterRev.y, 1e-5f);
    EXPECT_NEAR(afterOrig.z, afterRev.z, 1e-5f);
    // Restore
    player->m_vecMoveSpeed = origSpeed;
}

// --- SkipPhysics (void return — test it doesn't crash) ---

GAME_DIFF_TEST(CPhysical3, SkipPhysics_Player) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    CVector savedSpeed = player->m_vecMoveSpeed;
    CVector savedTurn = player->m_vecTurnSpeed;
    { HookDisableGuard guard("Entity/CPhysical/SkipPhysics");
      player->SkipPhysics(); }
    CVector afterOrig = player->m_vecMoveSpeed;
    player->m_vecMoveSpeed = savedSpeed;
    player->m_vecTurnSpeed = savedTurn;
    player->SkipPhysics();
    CVector afterRev = player->m_vecMoveSpeed;
    EXPECT_NEAR(afterOrig.x, afterRev.x, 1e-5f);
    EXPECT_NEAR(afterOrig.y, afterRev.y, 1e-5f);
    EXPECT_NEAR(afterOrig.z, afterRev.z, 1e-5f);
    player->m_vecMoveSpeed = savedSpeed;
    player->m_vecTurnSpeed = savedTurn;
}

// --- GetHasCollidedWith ---

GAME_DIFF_TEST(CPhysical3, GetHasCollidedWith_PlayerSelf) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    bool orig, rev;
    { HookDisableGuard guard("Entity/CPhysical/GetHasCollidedWith");
      orig = player->GetHasCollidedWith(player); }
    rev = player->GetHasCollidedWith(player);
    EXPECT_EQ(orig, rev);
}

// --- Spawned vehicle tests ---

static Scenario::TestVehicle* s_pv3 = nullptr;
static bool s_pv3Tried = false;

static CPhysical* GetPV3() {
    if (!s_pv3Tried) {
        s_pv3Tried = true;
        int32 model = Scenario::FindLoadedVehicleModel();
        if (model >= 0)
            s_pv3 = new Scenario::TestVehicle(model, CVector(2406.f, -1670.f, 13.5f));
    }
    return s_pv3 ? static_cast<CPhysical*>(s_pv3->AsVehicle()) : nullptr;
}

GAME_DIFF_TEST(CPhysical3, SkipPhysics_Vehicle) {
    auto* phys = GetPV3();
    if (!phys) return;
    CVector savedSpeed = phys->m_vecMoveSpeed;
    CVector savedTurn = phys->m_vecTurnSpeed;
    { HookDisableGuard guard("Entity/CPhysical/SkipPhysics");
      phys->SkipPhysics(); }
    CVector afterOrig = phys->m_vecMoveSpeed;
    phys->m_vecMoveSpeed = savedSpeed;
    phys->m_vecTurnSpeed = savedTurn;
    phys->SkipPhysics();
    CVector afterRev = phys->m_vecMoveSpeed;
    EXPECT_NEAR(afterOrig.x, afterRev.x, 1e-5f);
    EXPECT_NEAR(afterOrig.y, afterRev.y, 1e-5f);
    EXPECT_NEAR(afterOrig.z, afterRev.z, 1e-5f);
    phys->m_vecMoveSpeed = savedSpeed;
    phys->m_vecTurnSpeed = savedTurn;
}

GAME_DIFF_TEST(CPhysical3, ApplyGravity_Vehicle) {
    auto* phys = GetPV3();
    if (!phys) return;
    CVector origSpeed = phys->m_vecMoveSpeed;
    { HookDisableGuard guard("Entity/CPhysical/ApplyGravity");
      phys->m_vecMoveSpeed = origSpeed;
      phys->ApplyGravity(); }
    CVector afterOrig = phys->m_vecMoveSpeed;
    phys->m_vecMoveSpeed = origSpeed;
    phys->ApplyGravity();
    CVector afterRev = phys->m_vecMoveSpeed;
    EXPECT_NEAR(afterOrig.x, afterRev.x, 1e-5f);
    EXPECT_NEAR(afterOrig.y, afterRev.y, 1e-5f);
    EXPECT_NEAR(afterOrig.z, afterRev.z, 1e-5f);
    phys->m_vecMoveSpeed = origSpeed;
}
