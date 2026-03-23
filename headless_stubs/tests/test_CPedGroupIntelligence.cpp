// test_CPedGroupIntelligence.cpp — Deep differential tests.
// Hook paths: Global/CPedGroupIntelligence/<fn>
//
// Uses the player's ped group (if any) at state 9.

#include "StdInc.h"
#include "TestFramework.h"
#include "PedGroupIntelligence.h"
#include "PedGroup.h"
#include "Entity/Ped/Ped.h"

static CPedGroupIntelligence* GetPlayerGroupIntelligence() {
    auto* player = FindPlayerPed(0);
    if (!player) return nullptr;
    auto* group = CPedGroups::GetPedsGroup(player);
    if (!group) return nullptr;
    return &group->GetIntelligence();
}

// --- IsCurrentEventValid ---

GAME_DIFF_TEST(CPedGroupIntelligence, IsCurrentEventValid) {
    auto* intel = GetPlayerGroupIntelligence();
    if (!intel) return;
    bool orig, rev;
    { HookDisableGuard guard("Global/CPedGroupIntelligence/IsCurrentEventValid"); orig = intel->IsCurrentEventValid(); }
    rev = intel->IsCurrentEventValid();
    EXPECT_EQ(orig, rev);
}

// --- IsGroupResponding ---

GAME_DIFF_TEST(CPedGroupIntelligence, IsGroupResponding) {
    auto* intel = GetPlayerGroupIntelligence();
    if (!intel) return;
    bool orig, rev;
    { HookDisableGuard guard("Global/CPedGroupIntelligence/IsGroupResponding"); orig = intel->IsGroupResponding(); }
    rev = intel->IsGroupResponding();
    EXPECT_EQ(orig, rev);
}

// --- GetTaskMain ---

GAME_DIFF_TEST(CPedGroupIntelligence, GetTaskMain) {
    auto* intel = GetPlayerGroupIntelligence();
    if (!intel) return;
    auto* player = FindPlayerPed(0);
    if (!player) return;
    CTask* orig;
    CTask* rev;
    { HookDisableGuard guard("Global/CPedGroupIntelligence/GetTaskMain"); orig = intel->GetTaskMain(player); }
    rev = intel->GetTaskMain(player);
    EXPECT_EQ((uintptr_t)orig, (uintptr_t)rev);
}

// --- GetTaskDefault ---

GAME_DIFF_TEST(CPedGroupIntelligence, GetTaskDefault) {
    auto* intel = GetPlayerGroupIntelligence();
    if (!intel) return;
    auto* player = FindPlayerPed(0);
    if (!player) return;
    CTask* orig;
    CTask* rev;
    { HookDisableGuard guard("Global/CPedGroupIntelligence/GetTaskDefault"); orig = intel->GetTaskDefault(player); }
    rev = intel->GetTaskDefault(player);
    EXPECT_EQ((uintptr_t)orig, (uintptr_t)rev);
}

// --- GetTaskSecondary ---

GAME_DIFF_TEST(CPedGroupIntelligence, GetTaskSecondary) {
    auto* intel = GetPlayerGroupIntelligence();
    if (!intel) return;
    auto* player = FindPlayerPed(0);
    if (!player) return;
    CTask* orig;
    CTask* rev;
    { HookDisableGuard guard("Global/CPedGroupIntelligence/GetTaskSecondary"); orig = intel->GetTaskSecondary(player); }
    rev = intel->GetTaskSecondary(player);
    EXPECT_EQ((uintptr_t)orig, (uintptr_t)rev);
}

// --- GetTaskScriptCommand ---

GAME_DIFF_TEST(CPedGroupIntelligence, GetTaskScriptCommand) {
    auto* intel = GetPlayerGroupIntelligence();
    if (!intel) return;
    auto* player = FindPlayerPed(0);
    if (!player) return;
    CTask* orig;
    CTask* rev;
    { HookDisableGuard guard("Global/CPedGroupIntelligence/GetTaskScriptCommand"); orig = intel->GetTaskScriptCommand(player); }
    rev = intel->GetTaskScriptCommand(player);
    EXPECT_EQ((uintptr_t)orig, (uintptr_t)rev);
}
