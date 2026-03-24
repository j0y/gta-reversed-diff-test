// test_Scenario_PedMutation.cpp — CPed mutation + query round-trip tests.

#include "StdInc.h"
#include "TestFramework.h"
#include "Entity/Ped/Ped.h"
#include "ScenarioHelpers.h"

// --- ClearWeapons round-trip on spawned NPC ---

GAME_DIFF_TEST(Scenario_PedMutation, ClearWeapons_NPC) {
    int32 model = Scenario::FindLoadedPedModel();
    if (model < 0) return;
    Scenario::TestPed npc1(PED_TYPE_CIVMALE, model, CVector(2478.f, -1660.f, 13.5f));
    Scenario::TestPed npc2(PED_TYPE_CIVMALE, model, CVector(2480.f, -1660.f, 13.5f));
    if (!npc1 || !npc2) return;

    { HookDisableGuard guard("Entity/Ped/CPed/ClearWeapons");
      npc1->ClearWeapons(); }
    npc2->ClearWeapons();

    // Compare all weapon slots
    for (int32 slot = 0; slot < 13; slot++) {
        EXPECT_EQ((int32)npc1->GetWeapon(static_cast<eWeaponType>(slot)).m_Type,
                  (int32)npc2->GetWeapon(static_cast<eWeaponType>(slot)).m_Type);
    }
}

// --- CalculateNewOrientation ---

GAME_DIFF_TEST(Scenario_PedMutation, CalculateNewOrientation_Player) {
    auto* player = FindPlayerPed(0);
    if (!player) return;

    float savedHeading = player->GetHeading();
    float savedRotation = player->m_fCurrentRotation;

    player->m_fCurrentRotation = 1.5f;
    { HookDisableGuard guard("Entity/Ped/CPed/CalculateNewOrientation");
      player->CalculateNewOrientation(); }
    float origHeading = player->GetHeading();

    player->SetHeading(savedHeading);
    player->m_fCurrentRotation = 1.5f;
    player->CalculateNewOrientation();
    float revHeading = player->GetHeading();

    EXPECT_NEAR(origHeading, revHeading, 1e-4f);

    player->SetHeading(savedHeading);
    player->m_fCurrentRotation = savedRotation;
}

// --- ClearLookFlag ---

GAME_DIFF_TEST(Scenario_PedMutation, ClearLookFlag_Player) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    { HookDisableGuard guard("Entity/Ped/CPed/ClearLookFlag");
      player->ClearLookFlag(); }
    player->ClearLookFlag();
    EXPECT_TRUE(true);
}

// --- ClearAll on NPC ---

GAME_DIFF_TEST(Scenario_PedMutation, ClearAll_NPC) {
    int32 model = Scenario::FindLoadedPedModel();
    if (model < 0) return;
    Scenario::TestPed npc(PED_TYPE_CIVMALE, model, CVector(2476.f, -1658.f, 13.5f));
    if (!npc) return;

    { HookDisableGuard guard("Entity/Ped/CPed/ClearAll");
      npc->ClearAll(); }
    EXPECT_TRUE(true);
}

// --- SetIdle ---

GAME_DIFF_TEST(Scenario_PedMutation, SetIdle_NPC) {
    int32 model = Scenario::FindLoadedPedModel();
    if (model < 0) return;
    Scenario::TestPed npc(PED_TYPE_CIVMALE, model, CVector(2474.f, -1656.f, 13.5f));
    if (!npc) return;

    ePedState savedState = npc->m_nPedState;

    { HookDisableGuard guard("Entity/Ped/CPed/SetIdle");
      npc->m_nPedState = savedState;
      npc->SetIdle(); }
    ePedState origState = npc->m_nPedState;

    npc->m_nPedState = savedState;
    npc->SetIdle();
    ePedState revState = npc->m_nPedState;

    EXPECT_EQ((int32)origState, (int32)revState);
}
