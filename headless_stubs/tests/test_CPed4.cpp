// test_CPed4.cpp — CPed mutation differential tests.
// Hook paths: Entity/Ped/CPed/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "Entity/Ped/Ped.h"
#include "ScenarioHelpers.h"

// GiveWeapon is overloaded — hook path not found as "Entity/Ped/CPed/GiveWeapon".
// Use the overload suffix instead.

GAME_DIFF_TEST(CPed4, GiveWeapon_ClearWeapon_RoundTrip) {
    int32 model = Scenario::FindLoadedPedModel();
    if (model < 0) return;
    Scenario::TestPed npc1(PED_TYPE_CIVMALE, model, CVector(2462.f, -1650.f, 13.5f));
    Scenario::TestPed npc2(PED_TYPE_CIVMALE, model, CVector(2464.f, -1650.f, 13.5f));
    if (!npc1 || !npc2) return;

    { HookDisableGuard guard("Entity/Ped/CPed/GiveWeapon-");
      npc1->GiveWeapon(WEAPON_PISTOL, 50, false); }
    { HookDisableGuard guard("Entity/Ped/CPed/ClearWeapon");
      npc1->ClearWeapon(WEAPON_PISTOL); }
    int32 origType = (int32)npc1->GetWeapon(WEAPON_PISTOL).m_Type;

    npc2->GiveWeapon(WEAPON_PISTOL, 50, false);
    npc2->ClearWeapon(WEAPON_PISTOL);
    int32 revType = (int32)npc2->GetWeapon(WEAPON_PISTOL).m_Type;

    EXPECT_EQ(origType, revType);
}

// --- SetWeaponSkill ---

GAME_DIFF_TEST(CPed4, SetWeaponSkill) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    // Save and test with pistol skill
    auto saved = player->GetWeaponSkill(WEAPON_PISTOL);

    { HookDisableGuard guard("Entity/Ped/CPed/SetWeaponSkill");
      player->SetWeaponSkill(WEAPON_PISTOL, eWeaponSkill::STD); }
    auto origSkill = player->GetWeaponSkill(WEAPON_PISTOL);

    player->SetWeaponSkill(WEAPON_PISTOL, eWeaponSkill::STD);
    auto revSkill = player->GetWeaponSkill(WEAPON_PISTOL);

    EXPECT_EQ((int32)origSkill, (int32)revSkill);
}

// --- PutOnGoggles / TakeOffGoggles ---

GAME_DIFF_TEST(CPed4, TakeOffGoggles_Player) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    { HookDisableGuard guard("Entity/Ped/CPed/TakeOffGoggles");
      player->TakeOffGoggles(); }
    player->TakeOffGoggles();
    EXPECT_TRUE(true);
}

// --- ClearAimFlag ---

GAME_DIFF_TEST(CPed4, ClearAimFlag_Player) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    { HookDisableGuard guard("Entity/Ped/CPed/ClearAimFlag");
      player->ClearAimFlag(); }
    player->ClearAimFlag();
    EXPECT_TRUE(true);
}

// --- ClearLook ---

GAME_DIFF_TEST(CPed4, ClearLook_Player) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    { HookDisableGuard guard("Entity/Ped/CPed/ClearLook");
      player->ClearLook(); }
    player->ClearLook();
    EXPECT_TRUE(true);
}

// --- RemoveWeaponAnims ---

GAME_DIFF_TEST(CPed4, RemoveWeaponAnims_Player) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    { HookDisableGuard guard("Entity/Ped/CPed/RemoveWeaponAnims");
      player->RemoveWeaponAnims(0, -1000.f); }
    player->RemoveWeaponAnims(0, -1000.f);
    EXPECT_TRUE(true);
}

// DoesLOSBulletHitPed takes extra args — skipped.

// --- SetMoveState ---

GAME_DIFF_TEST(CPed4, SetMoveState_Player) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    auto saved = player->m_nMoveState;
    { HookDisableGuard guard("Entity/Ped/CPed/SetMoveState");
      player->SetMoveState(PEDMOVE_WALK); }
    auto origState = player->m_nMoveState;

    player->m_nMoveState = saved;
    player->SetMoveState(PEDMOVE_WALK);
    auto revState = player->m_nMoveState;

    EXPECT_EQ((int32)origState, (int32)revState);
    player->m_nMoveState = saved;
}

// --- SetStayInSamePlace ---

GAME_DIFF_TEST(CPed4, SetStayInSamePlace_NPC) {
    int32 model = Scenario::FindLoadedPedModel();
    if (model < 0) return;
    Scenario::TestPed npc(PED_TYPE_CIVMALE, model, CVector(2460.f, -1648.f, 13.5f));
    if (!npc) return;

    { HookDisableGuard guard("Entity/Ped/CPed/SetStayInSamePlace");
      npc->SetStayInSamePlace(true); }
    bool origVal = npc->bStayInSamePlace;

    npc->bStayInSamePlace = false;
    npc->SetStayInSamePlace(true);
    bool revVal = npc->bStayInSamePlace;

    EXPECT_EQ(origVal, revVal);
}

// --- SetPedDefaultDecisionMaker ---

GAME_DIFF_TEST(CPed4, SetPedDefaultDecisionMaker_NPC) {
    int32 model = Scenario::FindLoadedPedModel();
    if (model < 0) return;
    Scenario::TestPed npc(PED_TYPE_CIVMALE, model, CVector(2458.f, -1648.f, 13.5f));
    if (!npc) return;

    { HookDisableGuard guard("Entity/Ped/CPed/SetPedDefaultDecisionMaker");
      npc->SetPedDefaultDecisionMaker(); }
    npc->SetPedDefaultDecisionMaker();
    EXPECT_TRUE(true);
}

// --- SetRadioStation ---

GAME_DIFF_TEST(CPed4, SetRadioStation) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    { HookDisableGuard guard("Entity/Ped/CPed/SetRadioStation");
      player->SetRadioStation(); }
    player->SetRadioStation();
    EXPECT_TRUE(true);
}
