// test_CPedIntelligence.cpp — Differential tests for CPedIntelligence.
// Hook paths: Global/CPedIntelligence/<fn>
// Accessed via FindPlayerPed(0)->GetIntelligence()

#include "StdInc.h"
#include "TestFramework.h"
#include "PedIntelligence.h"
#include "Entity/Ped/Ped.h"
#include "World.h"

static CPedIntelligence* GetPlayerIntel() {
    auto* ped = FindPlayerPed(0);
    if (!ped) return nullptr;
    return ped->GetIntelligence();
}

// --- Task getters (return specific task type pointers, compare as void*) ---

#define DIFF_INTEL_TASK(FnName) \
    GAME_DIFF_TEST(CPedIntelligence, FnName) { \
        auto* intel = GetPlayerIntel(); \
        if (!intel) return; \
        void* orig; \
        void* rev; \
        { HookDisableGuard guard("Global/CPedIntelligence/" #FnName); orig = (void*)intel->FnName(); } \
        rev = (void*)intel->FnName(); \
        EXPECT_EQ(orig, rev); \
    }

DIFF_INTEL_TASK(GetTaskFighting)
DIFF_INTEL_TASK(GetTaskUseGun)
DIFF_INTEL_TASK(GetTaskThrow)
DIFF_INTEL_TASK(GetTaskHold)
DIFF_INTEL_TASK(GetTaskSwim)
DIFF_INTEL_TASK(GetTaskDuck)
DIFF_INTEL_TASK(GetTaskJetPack)
DIFF_INTEL_TASK(GetTaskInAir)
DIFF_INTEL_TASK(GetTaskClimb)
DIFF_INTEL_TASK(GetActivePrimaryTask)

GAME_DIFF_TEST(CPedIntelligence, GetUsingParachute) {
    auto* intel = GetPlayerIntel();
    if (!intel) return;
    bool orig, rev;
    { HookDisableGuard guard("Global/CPedIntelligence/GetUsingParachute"); orig = intel->GetUsingParachute(); }
    rev = intel->GetUsingParachute();
    EXPECT_EQ(orig, rev);
}

// --- Bool queries ---

GAME_DIFF_TEST(CPedIntelligence, IsInACarOrEnteringOne) {
    auto* intel = GetPlayerIntel();
    if (!intel) return;
    bool orig, rev;
    { HookDisableGuard guard("Global/CPedIntelligence/IsInACarOrEnteringOne"); orig = intel->IsInACarOrEnteringOne(); }
    rev = intel->IsInACarOrEnteringOne();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CPedIntelligence, GetEffectInUse) {
    auto* intel = GetPlayerIntel();
    if (!intel) return;
    bool orig, rev;
    { HookDisableGuard guard("Global/CPedIntelligence/GetEffectInUse"); orig = intel->GetEffectInUse(); }
    rev = intel->GetEffectInUse();
    EXPECT_EQ(orig, rev);
}

// Relationship queries — test player ped against itself (safe, no other entity needed)
GAME_DIFF_TEST(CPedIntelligence, IsFriendlyWith) {
    auto* ped = FindPlayerPed(0);
    if (!ped) return;
    auto* intel = ped->GetIntelligence();
    if (!intel) return;
    // Test if player is friendly with itself
    bool orig, rev;
    { HookDisableGuard guard("Global/CPedIntelligence/IsFriendlyWith"); orig = intel->IsFriendlyWith(*ped); }
    rev = intel->IsFriendlyWith(*ped);
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CPedIntelligence, IsThreatenedBy) {
    auto* ped = FindPlayerPed(0);
    if (!ped) return;
    auto* intel = ped->GetIntelligence();
    if (!intel) return;
    bool orig, rev;
    { HookDisableGuard guard("Global/CPedIntelligence/IsThreatenedBy"); orig = intel->IsThreatenedBy(*ped); }
    rev = intel->IsThreatenedBy(*ped);
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CPedIntelligence, Respects) {
    auto* ped = FindPlayerPed(0);
    if (!ped) return;
    auto* intel = ped->GetIntelligence();
    if (!intel) return;
    bool orig, rev;
    { HookDisableGuard guard("Global/CPedIntelligence/Respects"); orig = intel->Respects(ped); }
    rev = intel->Respects(ped);
    EXPECT_EQ(orig, rev);
}

// --- Additional intelligence queries ---

GAME_DIFF_TEST(CPedIntelligence, IsRespondingToEvent) {
    auto* ped = FindPlayerPed(0);
    if (!ped) return;
    auto* intel = ped->GetIntelligence();
    if (!intel) return;
    // Test a few common event types
    int eventTypes[] = { 0, 1, 2, 5, 10, 20, 50 };
    for (int et : eventTypes) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CPedIntelligence/IsRespondingToEvent"); orig = intel->IsRespondingToEvent((eEventType)et); }
        rev = intel->IsRespondingToEvent((eEventType)et);
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(CPedIntelligence, IsPedGoingForCarDoor) {
    auto* ped = FindPlayerPed(0);
    if (!ped) return;
    auto* intel = ped->GetIntelligence();
    if (!intel) return;
    bool orig, rev;
    { HookDisableGuard guard("Global/CPedIntelligence/IsPedGoingForCarDoor"); orig = intel->IsPedGoingForCarDoor(); }
    rev = intel->IsPedGoingForCarDoor();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CPedIntelligence, IsInterestingEntity) {
    auto* ped = FindPlayerPed(0);
    if (!ped) return;
    auto* intel = ped->GetIntelligence();
    if (!intel) return;
    // Test with nullptr and with self
    for (CEntity* entity : { (CEntity*)nullptr, (CEntity*)ped }) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CPedIntelligence/IsInterestingEntity"); orig = intel->IsInterestingEntity(entity); }
        rev = intel->IsInterestingEntity(entity);
        EXPECT_EQ(orig, rev);
    }
}
