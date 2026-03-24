// test_CPedIntelligence4.cpp — Deep differential tests for CPedIntelligence.
// Hook paths: Global/CPedIntelligence/<fn>
// Task getters return specific CTaskSimple* types — use (void*) cast to compare pointers.

#include "StdInc.h"
#include "TestFramework.h"
#include "PedIntelligence.h"
#include "Entity/Ped/Ped.h"
#include "ScenarioHelpers.h"

// Helper macro for task getter tests — casts to void* to avoid type mismatch
#define DIFF_TASK_TEST(Name, ...) \
GAME_DIFF_TEST(CPedIntelligence4, Name##_Player) { \
    auto* player = FindPlayerPed(0); \
    if (!player || !player->GetIntelligence()) return; \
    void* orig; void* rev; \
    { HookDisableGuard guard("Global/CPedIntelligence/" #Name); \
      orig = (void*)player->GetIntelligence()->Name(__VA_ARGS__); } \
    rev = (void*)player->GetIntelligence()->Name(__VA_ARGS__); \
    EXPECT_EQ(orig, rev); \
}

DIFF_TASK_TEST(GetTaskSwim)
DIFF_TASK_TEST(GetTaskDuck, false)
DIFF_TASK_TEST(GetTaskUseGun)
DIFF_TASK_TEST(GetTaskClimb)
DIFF_TASK_TEST(GetTaskInAir)
DIFF_TASK_TEST(GetTaskHold, false)
DIFF_TASK_TEST(GetTaskFighting)
DIFF_TASK_TEST(GetTaskJetPack)
DIFF_TASK_TEST(GetTaskThrow)

#undef DIFF_TASK_TEST

// --- Boolean queries ---

GAME_DIFF_TEST(CPedIntelligence4, GetUsingParachute_Player) {
    auto* player = FindPlayerPed(0);
    if (!player || !player->GetIntelligence()) return;
    bool orig, rev;
    { HookDisableGuard guard("Global/CPedIntelligence/GetUsingParachute");
      orig = player->GetIntelligence()->GetUsingParachute(); }
    rev = player->GetIntelligence()->GetUsingParachute();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CPedIntelligence4, IsInACarOrEnteringOne_Player) {
    auto* player = FindPlayerPed(0);
    if (!player || !player->GetIntelligence()) return;
    bool orig, rev;
    { HookDisableGuard guard("Global/CPedIntelligence/IsInACarOrEnteringOne");
      orig = player->GetIntelligence()->IsInACarOrEnteringOne(); }
    rev = player->GetIntelligence()->IsInACarOrEnteringOne();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CPedIntelligence4, IsPedGoingForCarDoor_Player) {
    auto* player = FindPlayerPed(0);
    if (!player || !player->GetIntelligence()) return;
    bool orig, rev;
    { HookDisableGuard guard("Global/CPedIntelligence/IsPedGoingForCarDoor");
      orig = player->GetIntelligence()->IsPedGoingForCarDoor(); }
    rev = player->GetIntelligence()->IsPedGoingForCarDoor();
    EXPECT_EQ(orig, rev);
}

// --- Friend/threat queries ---

static Scenario::TestPed* s_pi4Npc = nullptr;
static bool s_triedPi4 = false;

static CPed* GetPi4Npc() {
    if (!s_triedPi4) {
        s_triedPi4 = true;
        int32 model = Scenario::FindLoadedPedModel();
        if (model >= 0)
            s_pi4Npc = new Scenario::TestPed(PED_TYPE_CIVMALE, model, CVector(2482.f, -1662.f, 13.5f));
    }
    return s_pi4Npc ? s_pi4Npc->AsPed() : nullptr;
}

GAME_DIFF_TEST(CPedIntelligence4, AreFriends_PlayerAndNpc) {
    auto* player = FindPlayerPed(0);
    auto* npc = GetPi4Npc();
    if (!player || !npc || !player->GetIntelligence()) return;
    bool orig, rev;
    { HookDisableGuard guard("Global/CPedIntelligence/AreFriends");
      orig = CPedIntelligence::AreFriends(*player, *npc); }
    rev = CPedIntelligence::AreFriends(*player, *npc);
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CPedIntelligence4, IsFriendlyWith_PlayerAndNpc) {
    auto* player = FindPlayerPed(0);
    auto* npc = GetPi4Npc();
    if (!player || !npc || !player->GetIntelligence()) return;
    bool orig, rev;
    { HookDisableGuard guard("Global/CPedIntelligence/IsFriendlyWith");
      orig = player->GetIntelligence()->IsFriendlyWith(*npc); }
    rev = player->GetIntelligence()->IsFriendlyWith(*npc);
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CPedIntelligence4, IsThreatenedBy_PlayerAndNpc) {
    auto* player = FindPlayerPed(0);
    auto* npc = GetPi4Npc();
    if (!player || !npc || !player->GetIntelligence()) return;
    bool orig, rev;
    { HookDisableGuard guard("Global/CPedIntelligence/IsThreatenedBy");
      orig = player->GetIntelligence()->IsThreatenedBy(*npc); }
    rev = player->GetIntelligence()->IsThreatenedBy(*npc);
    EXPECT_EQ(orig, rev);
}
