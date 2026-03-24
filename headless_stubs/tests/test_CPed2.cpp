// test_CPed2.cpp — Extended differential tests for CPed.
// Hook paths: Entity/Ped/CPed/<fn>
//
// Tests CPed functions not covered by test_CPed.cpp. Includes queries
// on both the player ped and a spawned NPC.

#include "StdInc.h"
#include "TestFramework.h"
#include "Entity/Ped/Ped.h"
#include "ScenarioHelpers.h"

// --- Player ped tests ---

GAME_DIFF_TEST(CPed2, IsPointerValid_Player) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    bool orig, rev;
    { HookDisableGuard guard("Entity/Ped/CPed/IsPointerValid");
      orig = player->IsPointerValid(); }
    rev = player->IsPointerValid();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CPed2, IsPedInControl_Player) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    bool orig, rev;
    { HookDisableGuard guard("Entity/Ped/CPed/IsPedInControl");
      orig = player->IsPedInControl(); }
    rev = player->IsPedInControl();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CPed2, PedIsInvolvedInConversation_Player) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    bool orig, rev;
    { HookDisableGuard guard("Entity/Ped/CPed/PedIsInvolvedInConversation");
      orig = player->PedIsInvolvedInConversation(); }
    rev = player->PedIsInvolvedInConversation();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CPed2, CanPedHoldConversation_Player) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    bool orig, rev;
    { HookDisableGuard guard("Entity/Ped/CPed/CanPedHoldConversation");
      orig = player->CanPedHoldConversation(); }
    rev = player->CanPedHoldConversation();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CPed2, IsPedHeadAbovePos_Player) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    float zPos = player->GetPosition().z - 1.0f; // below ped
    bool orig, rev;
    { HookDisableGuard guard("Entity/Ped/CPed/IsPedHeadAbovePos");
      orig = player->IsPedHeadAbovePos(zPos); }
    rev = player->IsPedHeadAbovePos(zPos);
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CPed2, IsPedHeadAbovePos_Player_Above) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    float zPos = player->GetPosition().z + 10.0f; // above ped
    bool orig, rev;
    { HookDisableGuard guard("Entity/Ped/CPed/IsPedHeadAbovePos");
      orig = player->IsPedHeadAbovePos(zPos); }
    rev = player->IsPedHeadAbovePos(zPos);
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CPed2, OurPedCanSeeThisEntity_Player) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    // Test if player can see themselves (edge case)
    bool orig, rev;
    { HookDisableGuard guard("Entity/Ped/CPed/OurPedCanSeeThisEntity");
      orig = player->OurPedCanSeeThisEntity(player, false); }
    rev = player->OurPedCanSeeThisEntity(player, false);
    EXPECT_EQ(orig, rev);
}

// --- Spawned NPC tests ---

static Scenario::TestPed* s_ped2Npc = nullptr;
static bool s_triedPed2Npc = false;

static CPed* GetPed2Npc() {
    if (!s_triedPed2Npc) {
        s_triedPed2Npc = true;
        int32 model = Scenario::FindLoadedPedModel();
        if (model >= 0)
            s_ped2Npc = new Scenario::TestPed(PED_TYPE_CIVMALE, model, CVector(2410.f, -1672.f, 13.5f));
    }
    return s_ped2Npc ? s_ped2Npc->AsPed() : nullptr;
}

GAME_DIFF_TEST(CPed2, IsPointerValid_NPC) {
    auto* ped = GetPed2Npc();
    if (!ped) return;
    bool orig, rev;
    { HookDisableGuard guard("Entity/Ped/CPed/IsPointerValid");
      orig = ped->IsPointerValid(); }
    rev = ped->IsPointerValid();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CPed2, IsPedInControl_NPC) {
    auto* ped = GetPed2Npc();
    if (!ped) return;
    bool orig, rev;
    { HookDisableGuard guard("Entity/Ped/CPed/IsPedInControl");
      orig = ped->IsPedInControl(); }
    rev = ped->IsPedInControl();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CPed2, CanPedHoldConversation_NPC) {
    auto* ped = GetPed2Npc();
    if (!ped) return;
    bool orig, rev;
    { HookDisableGuard guard("Entity/Ped/CPed/CanPedHoldConversation");
      orig = ped->CanPedHoldConversation(); }
    rev = ped->CanPedHoldConversation();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CPed2, PedIsInvolvedInConversation_NPC) {
    auto* ped = GetPed2Npc();
    if (!ped) return;
    bool orig, rev;
    { HookDisableGuard guard("Entity/Ped/CPed/PedIsInvolvedInConversation");
      orig = ped->PedIsInvolvedInConversation(); }
    rev = ped->PedIsInvolvedInConversation();
    EXPECT_EQ(orig, rev);
}

// --- Player can see spawned NPC ---

GAME_DIFF_TEST(CPed2, OurPedCanSeeThisEntity_PlayerSeesNPC) {
    auto* player = FindPlayerPed(0);
    auto* npc = GetPed2Npc();
    if (!player || !npc) return;
    bool orig, rev;
    { HookDisableGuard guard("Entity/Ped/CPed/OurPedCanSeeThisEntity");
      orig = player->OurPedCanSeeThisEntity(npc, false); }
    rev = player->OurPedCanSeeThisEntity(npc, false);
    EXPECT_EQ(orig, rev);
}
