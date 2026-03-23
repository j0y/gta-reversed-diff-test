// test_CPedGroupMembership.cpp — Deep differential tests.
// Hook paths: Global/CPedGroupMembership/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "PedGroupMembership.h"
#include "PedGroup.h"
#include "Entity/Ped/Ped.h"

static CPedGroupMembership* GetPlayerGroupMembership() {
    auto* player = FindPlayerPed(0);
    if (!player) return nullptr;
    auto* group = CPedGroups::GetPedsGroup(player);
    if (!group) return nullptr;
    return &group->GetMembership();
}

// --- CountMembers ---

GAME_DIFF_TEST(CPedGroupMembership, CountMembers) {
    auto* mem = GetPlayerGroupMembership();
    if (!mem) return;
    int32 orig, rev;
    { HookDisableGuard guard("Global/CPedGroupMembership/CountMembers"); orig = mem->CountMembers(); }
    rev = mem->CountMembers();
    EXPECT_EQ(orig, rev);
}

// --- CountMembersExcludingLeader ---

GAME_DIFF_TEST(CPedGroupMembership, CountMembersExcludingLeader) {
    auto* mem = GetPlayerGroupMembership();
    if (!mem) return;
    int32 orig, rev;
    { HookDisableGuard guard("Global/CPedGroupMembership/CountMembersExcludingLeader"); orig = mem->CountMembersExcludingLeader(); }
    rev = mem->CountMembersExcludingLeader();
    EXPECT_EQ(orig, rev);
}

// --- GetLeader ---

GAME_DIFF_TEST(CPedGroupMembership, GetLeader) {
    auto* mem = GetPlayerGroupMembership();
    if (!mem) return;
    CPed* orig;
    CPed* rev;
    { HookDisableGuard guard("Global/CPedGroupMembership/GetLeader"); orig = mem->GetLeader(); }
    rev = mem->GetLeader();
    EXPECT_EQ((uintptr_t)orig, (uintptr_t)rev);
}

// --- IsLeader ---

GAME_DIFF_TEST(CPedGroupMembership, IsLeader_Player) {
    auto* mem = GetPlayerGroupMembership();
    if (!mem) return;
    auto* player = FindPlayerPed(0);
    if (!player) return;
    bool orig, rev;
    { HookDisableGuard guard("Global/CPedGroupMembership/IsLeader"); orig = mem->IsLeader(player); }
    rev = mem->IsLeader(player);
    EXPECT_EQ(orig, rev);
}

// --- IsMember ---

GAME_DIFF_TEST(CPedGroupMembership, IsMember_Player) {
    auto* mem = GetPlayerGroupMembership();
    if (!mem) return;
    auto* player = FindPlayerPed(0);
    if (!player) return;
    bool orig, rev;
    { HookDisableGuard guard("Global/CPedGroupMembership/IsMember"); orig = mem->IsMember(player); }
    rev = mem->IsMember(player);
    EXPECT_EQ(orig, rev);
}

// --- IsFollower ---

GAME_DIFF_TEST(CPedGroupMembership, IsFollower_Player) {
    auto* mem = GetPlayerGroupMembership();
    if (!mem) return;
    auto* player = FindPlayerPed(0);
    if (!player) return;
    bool orig, rev;
    { HookDisableGuard guard("Global/CPedGroupMembership/IsFollower"); orig = mem->IsFollower(player); }
    rev = mem->IsFollower(player);
    EXPECT_EQ(orig, rev);
}
