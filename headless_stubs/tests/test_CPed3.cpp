// test_CPed3.cpp — Extended CPed query tests.
// Hook paths: Entity/Ped/CPed/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "Entity/Ped/Ped.h"
#include "ScenarioHelpers.h"

#define PED_BOOL_TEST(Name) \
GAME_DIFF_TEST(CPed3, Name##_Player) { \
    auto* player = FindPlayerPed(0); \
    if (!player) return; \
    bool orig, rev; \
    { HookDisableGuard guard("Entity/Ped/CPed/" #Name); orig = player->Name(); } \
    rev = player->Name(); \
    EXPECT_EQ(orig, rev); \
}

PED_BOOL_TEST(IsAlive)
PED_BOOL_TEST(CanBeDeleted)
PED_BOOL_TEST(CanBeDeletedEvenInVehicle)
PED_BOOL_TEST(CanBeArrested)
PED_BOOL_TEST(CanPedReturnToState)
PED_BOOL_TEST(CanStrafeOrMouseControl)
PED_BOOL_TEST(CanUseTorsoWhenLooking)
PED_BOOL_TEST(GetPedTalking)

#undef PED_BOOL_TEST

GAME_DIFF_TEST(CPed3, GetBikeRidingSkill_Player) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    float orig, rev;
    { HookDisableGuard guard("Entity/Ped/CPed/GetBikeRidingSkill"); orig = player->GetBikeRidingSkill(); }
    rev = player->GetBikeRidingSkill();
    EXPECT_NEAR(orig, rev, 1e-5f);
}

GAME_DIFF_TEST(CPed3, GetWalkAnimSpeed_Player) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    float orig, rev;
    { HookDisableGuard guard("Entity/Ped/CPed/GetWalkAnimSpeed"); orig = player->GetWalkAnimSpeed(); }
    rev = player->GetWalkAnimSpeed();
    EXPECT_NEAR(orig, rev, 1e-5f);
}

// GetWeaponSlot takes eWeaponType arg — test with common weapons
GAME_DIFF_TEST(CPed3, GetWeaponSlot_Player) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    eWeaponType weapons[] = { WEAPON_UNARMED, WEAPON_PISTOL, WEAPON_SHOTGUN, WEAPON_AK47 };
    for (auto wt : weapons) {
        int32 orig, rev;
        { HookDisableGuard guard("Entity/Ped/CPed/GetWeaponSlot"); orig = (int32)player->GetWeaponSlot(wt); }
        rev = (int32)player->GetWeaponSlot(wt);
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(CPed3, GetEntityThatThisPedIsHolding_Player) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    CEntity* orig; CEntity* rev;
    { HookDisableGuard guard("Entity/Ped/CPed/GetEntityThatThisPedIsHolding"); orig = player->GetEntityThatThisPedIsHolding(); }
    rev = player->GetEntityThatThisPedIsHolding();
    EXPECT_EQ(orig, rev);
}

// --- NPC ---

static Scenario::TestPed* s_p3Npc = nullptr;
static bool s_triedP3 = false;

static CPed* GetP3Npc() {
    if (!s_triedP3) {
        s_triedP3 = true;
        int32 model = Scenario::FindLoadedPedModel();
        if (model >= 0)
            s_p3Npc = new Scenario::TestPed(PED_TYPE_CIVMALE, model, CVector(2470.f, -1654.f, 13.5f));
    }
    return s_p3Npc ? s_p3Npc->AsPed() : nullptr;
}

GAME_DIFF_TEST(CPed3, IsAlive_NPC) {
    auto* ped = GetP3Npc();
    if (!ped) return;
    bool orig, rev;
    { HookDisableGuard guard("Entity/Ped/CPed/IsAlive"); orig = ped->IsAlive(); }
    rev = ped->IsAlive();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CPed3, CanBeDeleted_NPC) {
    auto* ped = GetP3Npc();
    if (!ped) return;
    bool orig, rev;
    { HookDisableGuard guard("Entity/Ped/CPed/CanBeDeleted"); orig = ped->CanBeDeleted(); }
    rev = ped->CanBeDeleted();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CPed3, CanSeeEntity_PlayerSeesNPC) {
    auto* player = FindPlayerPed(0);
    auto* npc = GetP3Npc();
    if (!player || !npc) return;
    bool orig, rev;
    { HookDisableGuard guard("Entity/Ped/CPed/CanSeeEntity"); orig = player->CanSeeEntity(npc, 90.f); }
    rev = player->CanSeeEntity(npc, 90.f);
    EXPECT_EQ(orig, rev);
}
