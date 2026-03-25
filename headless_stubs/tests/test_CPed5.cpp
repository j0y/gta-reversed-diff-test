// test_CPed5.cpp — More CPed differential tests on player + spawned NPC.
// Hook paths: Entity/Ped/CPed/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "Entity/Ped/Ped.h"
#include "ScenarioHelpers.h"

// --- GetWeaponSkill (no-arg, current weapon) ---

GAME_DIFF_TEST(CPed5, GetWeaponSkill_Current_Player) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    eWeaponSkill orig, rev;
    { HookDisableGuard guard("Entity/Ped/CPed/GetWeaponSkill-Current");
      orig = player->GetWeaponSkill(); }
    rev = player->GetWeaponSkill();
    EXPECT_EQ((int32)orig, (int32)rev);
}

GAME_DIFF_TEST(CPed5, GetWeaponSkill_Current_NPC) {
    int32 model = Scenario::FindLoadedPedModel();
    if (model < 0) return;
    Scenario::TestPed npc(PED_TYPE_CIVMALE, model, CVector(2460.f, -1650.f, 13.5f));
    if (!npc) return;
    eWeaponSkill orig, rev;
    { HookDisableGuard guard("Entity/Ped/CPed/GetWeaponSkill-Current");
      orig = npc->GetWeaponSkill(); }
    rev = npc->GetWeaponSkill();
    EXPECT_EQ((int32)orig, (int32)rev);
}

// --- CanSeeEntity (NPC seeing player) ---

GAME_DIFF_TEST(CPed5, CanSeeEntity_NPCSeesPlayer) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    int32 model = Scenario::FindLoadedPedModel();
    if (model < 0) return;
    Scenario::TestPed npc(PED_TYPE_CIVMALE, model, CVector(2462.f, -1650.f, 13.5f));
    if (!npc) return;
    bool orig, rev;
    { HookDisableGuard guard("Entity/Ped/CPed/CanSeeEntity");
      orig = npc->CanSeeEntity(player, 90.f); }
    rev = npc->CanSeeEntity(player, 90.f);
    EXPECT_EQ(orig, rev);
}

// --- GetEntityThatThisPedIsHolding (NPC) ---

GAME_DIFF_TEST(CPed5, GetEntityThatThisPedIsHolding_NPC) {
    int32 model = Scenario::FindLoadedPedModel();
    if (model < 0) return;
    Scenario::TestPed npc(PED_TYPE_CIVMALE, model, CVector(2464.f, -1650.f, 13.5f));
    if (!npc) return;
    CEntity* orig;
    CEntity* rev;
    { HookDisableGuard guard("Entity/Ped/CPed/GetEntityThatThisPedIsHolding");
      orig = npc->GetEntityThatThisPedIsHolding(); }
    rev = npc->GetEntityThatThisPedIsHolding();
    EXPECT_EQ(orig, rev);
}

// --- UseGroundColModel ---

GAME_DIFF_TEST(CPed5, UseGroundColModel_Player) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    bool orig, rev;
    { HookDisableGuard guard("Entity/Ped/CPed/UseGroundColModel");
      orig = player->UseGroundColModel(); }
    rev = player->UseGroundColModel();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CPed5, UseGroundColModel_NPC) {
    int32 model = Scenario::FindLoadedPedModel();
    if (model < 0) return;
    Scenario::TestPed npc(PED_TYPE_CIVMALE, model, CVector(2466.f, -1650.f, 13.5f));
    if (!npc) return;
    bool orig, rev;
    { HookDisableGuard guard("Entity/Ped/CPed/UseGroundColModel");
      orig = npc->UseGroundColModel(); }
    rev = npc->UseGroundColModel();
    EXPECT_EQ(orig, rev);
}

// --- PedIsReadyForConversation ---

GAME_DIFF_TEST(CPed5, PedIsReadyForConversation_Player) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    bool orig, rev;
    { HookDisableGuard guard("Entity/Ped/CPed/PedIsReadyForConversation");
      orig = player->PedIsReadyForConversation(false); }
    rev = player->PedIsReadyForConversation(false);
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CPed5, PedIsReadyForConversation_NPC) {
    int32 model = Scenario::FindLoadedPedModel();
    if (model < 0) return;
    Scenario::TestPed npc(PED_TYPE_CIVMALE, model, CVector(2468.f, -1650.f, 13.5f));
    if (!npc) return;
    bool orig, rev;
    { HookDisableGuard guard("Entity/Ped/CPed/PedIsReadyForConversation");
      orig = npc->PedIsReadyForConversation(false); }
    rev = npc->PedIsReadyForConversation(false);
    EXPECT_EQ(orig, rev);
}

// --- DoWeHaveWeaponAvailable ---

GAME_DIFF_TEST(CPed5, DoWeHaveWeaponAvailable_Player) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    bool orig, rev;
    { HookDisableGuard guard("Entity/Ped/CPed/DoWeHaveWeaponAvailable");
      orig = player->DoWeHaveWeaponAvailable(WEAPON_PISTOL); }
    rev = player->DoWeHaveWeaponAvailable(WEAPON_PISTOL);
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CPed5, DoWeHaveWeaponAvailable_NPC) {
    int32 model = Scenario::FindLoadedPedModel();
    if (model < 0) return;
    Scenario::TestPed npc(PED_TYPE_CIVMALE, model, CVector(2470.f, -1650.f, 13.5f));
    if (!npc) return;
    bool orig, rev;
    { HookDisableGuard guard("Entity/Ped/CPed/DoWeHaveWeaponAvailable");
      orig = npc->DoWeHaveWeaponAvailable(WEAPON_PISTOL); }
    rev = npc->DoWeHaveWeaponAvailable(WEAPON_PISTOL);
    EXPECT_EQ(orig, rev);
}

// --- GetWeaponSkill (with weapon type arg, on NPC) ---

GAME_DIFF_TEST(CPed5, GetWeaponSkill_WeaponType_NPC) {
    int32 model = Scenario::FindLoadedPedModel();
    if (model < 0) return;
    Scenario::TestPed npc(PED_TYPE_CIVMALE, model, CVector(2456.f, -1650.f, 13.5f));
    if (!npc) return;
    eWeaponSkill orig, rev;
    { HookDisableGuard guard("Entity/Ped/CPed/GetWeaponSkill-WeaponType");
      orig = npc->GetWeaponSkill(WEAPON_PISTOL); }
    rev = npc->GetWeaponSkill(WEAPON_PISTOL);
    EXPECT_EQ((int32)orig, (int32)rev);
}
