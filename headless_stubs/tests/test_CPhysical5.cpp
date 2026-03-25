// test_CPhysical5.cpp — More CPhysical differential tests on spawned vehicles.
// Hook paths: Entity/CPhysical/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "Entity/Physical.h"
#include "Entity/Ped/Ped.h"
#include "Entity/Vehicle/Vehicle.h"
#include "ScenarioHelpers.h"

static Scenario::TestVehicle* s_p5Veh = nullptr;
static bool s_triedP5 = false;

static CPhysical* GetP5Veh() {
    if (!s_triedP5) {
        s_triedP5 = true;
        int32 model = Scenario::FindLoadedVehicleModel();
        if (model >= 0)
            s_p5Veh = new Scenario::TestVehicle(model, CVector(2455.f, -1640.f, 13.5f));
    }
    return s_p5Veh ? static_cast<CPhysical*>(s_p5Veh->AsVehicle()) : nullptr;
}

// --- GetLightingTotal ---

GAME_DIFF_TEST(CPhysical5, GetLightingTotal_Vehicle) {
    auto* phys = GetP5Veh();
    if (!phys) return;
    float orig, rev;
    { HookDisableGuard guard("Entity/CPhysical/GetLightingTotal");
      orig = phys->GetLightingTotal(); }
    rev = phys->GetLightingTotal();
    EXPECT_NEAR(orig, rev, 1e-5f);
}

GAME_DIFF_TEST(CPhysical5, GetLightingTotal_Player) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    float orig, rev;
    { HookDisableGuard guard("Entity/CPhysical/GetLightingTotal");
      orig = player->GetLightingTotal(); }
    rev = player->GetLightingTotal();
    EXPECT_NEAR(orig, rev, 1e-5f);
}

// --- GetLightingFromCol ---

GAME_DIFF_TEST(CPhysical5, GetLightingFromCol_Vehicle_Interior) {
    auto* phys = GetP5Veh();
    if (!phys) return;
    float orig, rev;
    { HookDisableGuard guard("Entity/CPhysical/GetLightingFromCol");
      orig = phys->GetLightingFromCol(true); }
    rev = phys->GetLightingFromCol(true);
    EXPECT_NEAR(orig, rev, 1e-5f);
}

GAME_DIFF_TEST(CPhysical5, GetLightingFromCol_Vehicle_Exterior) {
    auto* phys = GetP5Veh();
    if (!phys) return;
    float orig, rev;
    { HookDisableGuard guard("Entity/CPhysical/GetLightingFromCol");
      orig = phys->GetLightingFromCol(false); }
    rev = phys->GetLightingFromCol(false);
    EXPECT_NEAR(orig, rev, 1e-5f);
}

GAME_DIFF_TEST(CPhysical5, GetLightingFromCol_Player) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    float orig, rev;
    { HookDisableGuard guard("Entity/CPhysical/GetLightingFromCol");
      orig = player->GetLightingFromCol(false); }
    rev = player->GetLightingFromCol(false);
    EXPECT_NEAR(orig, rev, 1e-5f);
}

// --- AddToMovingList / RemoveFromMovingList ---
// These modify m_pMovingList. Test round-trip behavior on a vehicle.

GAME_DIFF_TEST(CPhysical5, RemoveFromMovingList_Vehicle) {
    auto* phys = GetP5Veh();
    if (!phys) return;
    // Only test if the vehicle is currently on the moving list
    if (!phys->m_pMovingList) return;

    { HookDisableGuard guard("Entity/CPhysical/RemoveFromMovingList");
      phys->RemoveFromMovingList(); }
    auto* origPtr = phys->m_pMovingList;

    // Re-add so we can test again with reversed code
    phys->AddToMovingList();
    phys->RemoveFromMovingList();
    auto* revPtr = phys->m_pMovingList;

    EXPECT_EQ(origPtr, revPtr);
    // Re-add to restore state
    phys->AddToMovingList();
}

GAME_DIFF_TEST(CPhysical5, AddToMovingList_Vehicle) {
    auto* phys = GetP5Veh();
    if (!phys) return;
    // Remove first so we can test AddToMovingList
    if (phys->m_pMovingList) {
        phys->RemoveFromMovingList();
    }

    { HookDisableGuard guard("Entity/CPhysical/AddToMovingList");
      phys->AddToMovingList(); }
    bool origOnList = (phys->m_pMovingList != nullptr);

    phys->RemoveFromMovingList();
    phys->AddToMovingList();
    bool revOnList = (phys->m_pMovingList != nullptr);

    EXPECT_EQ(origOnList, revOnList);
}

// --- GetLightingTotal on NPC ---

GAME_DIFF_TEST(CPhysical5, GetLightingTotal_NPC) {
    int32 model = Scenario::FindLoadedPedModel();
    if (model < 0) return;
    Scenario::TestPed npc(PED_TYPE_CIVMALE, model, CVector(2458.f, -1640.f, 13.5f));
    if (!npc) return;
    float orig, rev;
    { HookDisableGuard guard("Entity/CPhysical/GetLightingTotal");
      orig = npc->GetLightingTotal(); }
    rev = npc->GetLightingTotal();
    EXPECT_NEAR(orig, rev, 1e-5f);
}

// --- GetLightingFromCol on NPC ---

GAME_DIFF_TEST(CPhysical5, GetLightingFromCol_NPC) {
    int32 model = Scenario::FindLoadedPedModel();
    if (model < 0) return;
    Scenario::TestPed npc(PED_TYPE_CIVMALE, model, CVector(2460.f, -1640.f, 13.5f));
    if (!npc) return;
    float orig, rev;
    { HookDisableGuard guard("Entity/CPhysical/GetLightingFromCol");
      orig = npc->GetLightingFromCol(false); }
    rev = npc->GetLightingFromCol(false);
    EXPECT_NEAR(orig, rev, 1e-5f);
}
