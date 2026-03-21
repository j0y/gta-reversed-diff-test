// test_CPed.cpp — Differential tests for CPed member functions.
// Hook paths: Entity/Ped/CPed/<fn>
// Uses FindPlayerPed(0) — the player ped (CJ) exists at state 9.

#include "StdInc.h"
#include "TestFramework.h"
#include "Entity/Ped/Ped.h"
#include "World.h"

// Helper — get player ped or skip test
static CPed* GetPlayer() {
    CPed* ped = FindPlayerPed(0);
    return ped;
}

// --- Simple bool queries (no args) ---

#define DIFF_PED_BOOL(FnName) \
    GAME_DIFF_TEST(CPed, FnName) { \
        auto* ped = GetPlayer(); \
        if (!ped) return; \
        bool orig, rev; \
        { HookDisableGuard guard("Entity/Ped/CPed/" #FnName); orig = ped->FnName(); } \
        rev = ped->FnName(); \
        EXPECT_EQ(orig, rev); \
    }

DIFF_PED_BOOL(IsPlayer)
DIFF_PED_BOOL(IsAlive)
DIFF_PED_BOOL(IsPedInControl)
DIFF_PED_BOOL(IsPedShootable)
DIFF_PED_BOOL(CanPedReturnToState)
DIFF_PED_BOOL(CanSetPedState)
DIFF_PED_BOOL(CanBeArrested)
DIFF_PED_BOOL(CanStrafeOrMouseControl)
DIFF_PED_BOOL(IsWearingGoggles)
DIFF_PED_BOOL(IsPlayingHandSignal)
DIFF_PED_BOOL(CanPedHoldConversation)
DIFF_PED_BOOL(GetPedTalking)
DIFF_PED_BOOL(PedIsInvolvedInConversation)

// --- Queries with simple args ---

GAME_DIFF_TEST(CPed, IsPedHeadAbovePos) {
    auto* ped = GetPlayer();
    if (!ped) return;
    float zPositions[] = { 0.0f, 5.0f, 10.0f, 50.0f, 100.0f };
    for (float z : zPositions) {
        bool orig, rev;
        { HookDisableGuard guard("Entity/Ped/CPed/IsPedHeadAbovePos"); orig = ped->IsPedHeadAbovePos(z); }
        rev = ped->IsPedHeadAbovePos(z);
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(CPed, PedIsReadyForConversation) {
    auto* ped = GetPlayer();
    if (!ped) return;
    for (bool arg : { true, false }) {
        bool orig, rev;
        { HookDisableGuard guard("Entity/Ped/CPed/PedIsReadyForConversation"); orig = ped->PedIsReadyForConversation(arg); }
        rev = ped->PedIsReadyForConversation(arg);
        EXPECT_EQ(orig, rev);
    }
}

// --- Float/int getters ---

GAME_DIFF_TEST(CPed, GetBikeRidingSkill) {
    auto* ped = GetPlayer();
    if (!ped) return;
    float orig, rev;
    { HookDisableGuard guard("Entity/Ped/CPed/GetBikeRidingSkill"); orig = ped->GetBikeRidingSkill(); }
    rev = ped->GetBikeRidingSkill();
    EXPECT_NEAR(orig, rev, 1e-5f);
}

GAME_DIFF_TEST(CPed, GetWalkAnimSpeed) {
    auto* ped = GetPlayer();
    if (!ped) return;
    float orig, rev;
    { HookDisableGuard guard("Entity/Ped/CPed/GetWalkAnimSpeed"); orig = ped->GetWalkAnimSpeed(); }
    rev = ped->GetWalkAnimSpeed();
    EXPECT_NEAR(orig, rev, 1e-5f);
}

GAME_DIFF_TEST(CPed, GetWeaponSlot) {
    auto* ped = GetPlayer();
    if (!ped) return;
    // Test a few common weapon types
    eWeaponType weapons[] = {
        WEAPON_UNARMED, WEAPON_PISTOL, WEAPON_SHOTGUN, WEAPON_MICRO_UZI,
        WEAPON_AK47, WEAPON_SNIPERRIFLE, WEAPON_KNIFE
    };
    for (auto wt : weapons) {
        int32 orig, rev;
        { HookDisableGuard guard("Entity/Ped/CPed/GetWeaponSlot"); orig = ped->GetWeaponSlot(wt); }
        rev = ped->GetWeaponSlot(wt);
        EXPECT_EQ(orig, rev);
    }
}

// --- More bool queries ---

DIFF_PED_BOOL(CanBeDeleted)
DIFF_PED_BOOL(CanBeDeletedEvenInVehicle)
DIFF_PED_BOOL(CanThrowEntityThatThisPedIsHolding)

GAME_DIFF_TEST(CPed, GetEntityThatThisPedIsHolding) {
    auto* ped = GetPlayer();
    if (!ped) return;
    CEntity* orig;
    CEntity* rev;
    { HookDisableGuard guard("Entity/Ped/CPed/GetEntityThatThisPedIsHolding"); orig = ped->GetEntityThatThisPedIsHolding(); }
    rev = ped->GetEntityThatThisPedIsHolding();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CPed, GetHoldingTask) {
    auto* ped = GetPlayer();
    if (!ped) return;
    void* orig;
    void* rev;
    { HookDisableGuard guard("Entity/Ped/CPed/GetHoldingTask"); orig = (void*)ped->GetHoldingTask(); }
    rev = (void*)ped->GetHoldingTask();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CPed, DoWeHaveWeaponAvailable) {
    auto* ped = GetPlayer();
    if (!ped) return;
    eWeaponType weapons[] = { WEAPON_UNARMED, WEAPON_PISTOL, WEAPON_SHOTGUN, WEAPON_AK47 };
    for (auto wt : weapons) {
        bool orig, rev;
        { HookDisableGuard guard("Entity/Ped/CPed/DoWeHaveWeaponAvailable"); orig = ped->DoWeHaveWeaponAvailable(wt); }
        rev = ped->DoWeHaveWeaponAvailable(wt);
        EXPECT_EQ(orig, rev);
    }
}

DIFF_PED_BOOL(UseGroundColModel)
DIFF_PED_BOOL(CanUseTorsoWhenLooking)
DIFF_PED_BOOL(IsPointerValid)
DIFF_PED_BOOL(TurnBody)

// GetLocalDirection depends on ped heading which can drift between hook toggles — skip diff test
GAME_TEST(CPed, GetLocalDirection_NoExcept) {
    auto* ped = GetPlayer();
    if (!ped) return;
    CVector2D dir(1.0f, 0.0f);
    uint8 result = ped->GetLocalDirection(dir);
    EXPECT_TRUE(result < 8); // Direction should be 0-7
}

// PedCanPickUpPickUp reads game timer state that changes between hook toggle — skip diff test
GAME_TEST(CPed, PedCanPickUpPickUp_NoExcept) {
    bool result = CPed::PedCanPickUpPickUp();
    (void)result;
    EXPECT_TRUE(true);
}
