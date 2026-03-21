// test_CPedGroup2.cpp — Tier 3 differential tests for CPedGroup with spawned members.
// Hook paths: Global/CPedGroup/<fn>
//
// Spawns 2-3 civilian peds, adds them to a CPedGroup as leader + followers,
// then tests membership, distance, and vehicle queries differentially.

#include "StdInc.h"
#include "TestFramework.h"
#include "ScenarioHelpers.h"
#include "PedGroup.h"
#include "PedGroups.h"

// Helper: remove a ped from a group membership safely
static void SafeRemoveFromGroup(CPedGroupMembership& membership, CPed* ped) {
    if (membership.IsMember(ped)) {
        membership.RemoveMember(ped);
    }
}

// --- Tests with populated group ---

GAME_DIFF_TEST(CPedGroup2, CountMembers) {
    int32 pedModel = Scenario::FindLoadedPedModel();
    if (pedModel < 0) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No loaded ped model"); return; }

    CVector basePos = FindPlayerCoors() + CVector(15.0f, 0.0f, 0.0f);
    Scenario::TestPed ped1(PED_TYPE_CIVMALE, pedModel, basePos);
    Scenario::TestPed ped2(PED_TYPE_CIVMALE, pedModel, basePos + CVector(2.0f, 0.0f, 0.0f));
    if (!ped1 || !ped2) { GetTestContext().RecordFailure(__FILE__, __LINE__, "Failed to spawn peds"); return; }

    CPedGroup group;
    group.GetMembership().SetLeader(ped1.AsPed());
    group.GetMembership().AddFollower(ped2.AsPed());

    // CountMembers
    size_t rev = group.GetMembership().CountMembers();
    EXPECT_EQ(rev, (size_t)2);

    // CountMembersExcludingLeader
    int32 revFollowers = group.GetMembership().CountMembersExcludingLeader();
    EXPECT_EQ(revFollowers, 1);

    // Cleanup — remove from group before peds are destroyed
    SafeRemoveFromGroup(group.GetMembership(), ped2.AsPed());
    SafeRemoveFromGroup(group.GetMembership(), ped1.AsPed());
}

GAME_DIFF_TEST(CPedGroup2, IsLeader_IsFollower_IsMember) {
    int32 pedModel = Scenario::FindLoadedPedModel();
    if (pedModel < 0) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No loaded ped model"); return; }

    CVector basePos = FindPlayerCoors() + CVector(15.0f, 5.0f, 0.0f);
    Scenario::TestPed leader(PED_TYPE_CIVMALE, pedModel, basePos);
    Scenario::TestPed follower(PED_TYPE_CIVMALE, pedModel, basePos + CVector(3.0f, 0.0f, 0.0f));
    if (!leader || !follower) return;

    CPedGroup group;
    group.GetMembership().SetLeader(leader.AsPed());
    group.GetMembership().AddFollower(follower.AsPed());

    // Leader checks
    EXPECT_TRUE(group.GetMembership().IsLeader(leader.AsPed()));
    EXPECT_FALSE(group.GetMembership().IsLeader(follower.AsPed()));
    EXPECT_FALSE(group.GetMembership().IsFollower(leader.AsPed()));

    // Follower checks
    EXPECT_TRUE(group.GetMembership().IsFollower(follower.AsPed()));
    EXPECT_FALSE(group.GetMembership().IsLeader(follower.AsPed()));

    // Member checks (both should be members)
    EXPECT_TRUE(group.GetMembership().IsMember(leader.AsPed()));
    EXPECT_TRUE(group.GetMembership().IsMember(follower.AsPed()));

    // Player should NOT be a member
    EXPECT_FALSE(group.GetMembership().IsMember(FindPlayerPed(0)));

    SafeRemoveFromGroup(group.GetMembership(), follower.AsPed());
    SafeRemoveFromGroup(group.GetMembership(), leader.AsPed());
}

GAME_DIFF_TEST(CPedGroup2, FindDistanceToNearestMember) {
    int32 pedModel = Scenario::FindLoadedPedModel();
    if (pedModel < 0) return;

    CVector basePos = FindPlayerCoors() + CVector(20.0f, 0.0f, 0.0f);
    Scenario::TestPed leader(PED_TYPE_CIVMALE, pedModel, basePos);
    Scenario::TestPed follower1(PED_TYPE_CIVMALE, pedModel, basePos + CVector(5.0f, 0.0f, 0.0f));
    Scenario::TestPed follower2(PED_TYPE_CIVMALE, pedModel, basePos + CVector(10.0f, 0.0f, 0.0f));
    if (!leader || !follower1 || !follower2) return;

    CPedGroup group;
    group.GetMembership().SetLeader(leader.AsPed());
    group.GetMembership().AddFollower(follower1.AsPed());
    group.GetMembership().AddFollower(follower2.AsPed());

    CPed* revNearest = nullptr;
    float rev = group.FindDistanceToNearestMember(&revNearest);
    CPed* origNearest = nullptr;
    float orig;
    { HookDisableGuard guard("Global/CPedGroup/FindDistanceToNearestMember");
      orig = group.FindDistanceToNearestMember(&origNearest); }

    EXPECT_NEAR(rev, orig, 1e-3f);
    // Pointer may differ if two followers are equidistant (tie-breaking order).
    // Both should be valid group members though.
    if (revNearest != origNearest) {
        // At least verify both are members
        EXPECT_TRUE(group.GetMembership().IsMember(revNearest));
        EXPECT_TRUE(group.GetMembership().IsMember(origNearest));
    }

    SafeRemoveFromGroup(group.GetMembership(), follower2.AsPed());
    SafeRemoveFromGroup(group.GetMembership(), follower1.AsPed());
    SafeRemoveFromGroup(group.GetMembership(), leader.AsPed());
}

GAME_DIFF_TEST(CPedGroup2, GetClosestGroupPed) {
    int32 pedModel = Scenario::FindLoadedPedModel();
    if (pedModel < 0) return;

    CVector basePos = FindPlayerCoors() + CVector(25.0f, 0.0f, 0.0f);
    Scenario::TestPed leader(PED_TYPE_CIVMALE, pedModel, basePos);
    Scenario::TestPed follower1(PED_TYPE_CIVMALE, pedModel, basePos + CVector(3.0f, 0.0f, 0.0f));
    Scenario::TestPed follower2(PED_TYPE_CIVMALE, pedModel, basePos + CVector(8.0f, 0.0f, 0.0f));
    if (!leader || !follower1 || !follower2) return;

    CPedGroup group;
    group.GetMembership().SetLeader(leader.AsPed());
    group.GetMembership().AddFollower(follower1.AsPed());
    group.GetMembership().AddFollower(follower2.AsPed());

    // Ask: which group member is closest to the leader?
    float revDist = 0.0f, origDist = 0.0f;
    CPed* rev = group.GetClosestGroupPed(leader.AsPed(), &revDist);
    CPed* orig;
    { HookDisableGuard guard("Global/CPedGroup/GetClosestGroupPed");
      orig = group.GetClosestGroupPed(leader.AsPed(), &origDist); }

    EXPECT_EQ((uintptr_t)rev, (uintptr_t)orig);
    EXPECT_NEAR(revDist, origDist, 1e-3f);

    SafeRemoveFromGroup(group.GetMembership(), follower2.AsPed());
    SafeRemoveFromGroup(group.GetMembership(), follower1.AsPed());
    SafeRemoveFromGroup(group.GetMembership(), leader.AsPed());
}

GAME_DIFF_TEST(CPedGroup2, IsAnyoneUsingCar_WithMembers) {
    int32 pedModel = Scenario::FindLoadedPedModel();
    int32 vehModel = Scenario::FindLoadedVehicleModel();
    if (pedModel < 0 || vehModel < 0) return;

    CVector basePos = FindPlayerCoors() + CVector(30.0f, 0.0f, 0.0f);
    Scenario::TestPed leader(PED_TYPE_CIVMALE, pedModel, basePos);
    Scenario::TestPed follower(PED_TYPE_CIVMALE, pedModel, basePos + CVector(3.0f, 0.0f, 0.0f));
    Scenario::TestVehicle veh(vehModel, basePos + CVector(10.0f, 0.0f, 0.0f));
    if (!leader || !follower || !veh) return;

    CPedGroup group;
    group.GetMembership().SetLeader(leader.AsPed());
    group.GetMembership().AddFollower(follower.AsPed());

    // No one is in the car — should be false
    bool rev = group.IsAnyoneUsingCar(veh.AsVehicle());
    bool orig;
    { HookDisableGuard guard("Global/CPedGroup/IsAnyoneUsingCar");
      orig = group.IsAnyoneUsingCar(veh.AsVehicle()); }
    EXPECT_EQ(rev, orig);
    EXPECT_FALSE(rev);

    SafeRemoveFromGroup(group.GetMembership(), follower.AsPed());
    SafeRemoveFromGroup(group.GetMembership(), leader.AsPed());
}

GAME_DIFF_TEST(CPedGroup2, RemoveAllFollowers) {
    int32 pedModel = Scenario::FindLoadedPedModel();
    if (pedModel < 0) return;

    CVector basePos = FindPlayerCoors() + CVector(35.0f, 0.0f, 0.0f);
    Scenario::TestPed leader(PED_TYPE_CIVMALE, pedModel, basePos);
    Scenario::TestPed follower1(PED_TYPE_CIVMALE, pedModel, basePos + CVector(3.0f, 0.0f, 0.0f));
    Scenario::TestPed follower2(PED_TYPE_CIVMALE, pedModel, basePos + CVector(6.0f, 0.0f, 0.0f));
    if (!leader || !follower1 || !follower2) return;

    CPedGroup group;
    group.GetMembership().SetLeader(leader.AsPed());
    group.GetMembership().AddFollower(follower1.AsPed());
    group.GetMembership().AddFollower(follower2.AsPed());

    EXPECT_EQ(group.GetMembership().CountMembers(), (size_t)3);

    group.RemoveAllFollowers();

    // Leader should remain, followers removed
    EXPECT_TRUE(group.GetMembership().IsLeader(leader.AsPed()));
    EXPECT_EQ(group.GetMembership().CountMembersExcludingLeader(), 0);
    EXPECT_FALSE(group.GetMembership().IsFollower(follower1.AsPed()));
    EXPECT_FALSE(group.GetMembership().IsFollower(follower2.AsPed()));

    SafeRemoveFromGroup(group.GetMembership(), leader.AsPed());
}

// --- Behavior tests ---

GAME_TEST(CPedGroup2, Flush_ClearsAll) {
    int32 pedModel = Scenario::FindLoadedPedModel();
    if (pedModel < 0) return;

    CVector basePos = FindPlayerCoors() + CVector(40.0f, 0.0f, 0.0f);
    Scenario::TestPed ped1(PED_TYPE_CIVMALE, pedModel, basePos);
    if (!ped1) return;

    CPedGroup group;
    group.GetMembership().SetLeader(ped1.AsPed());

    EXPECT_TRUE(group.GetMembership().HasLeader());
    group.Flush();
    EXPECT_FALSE(group.GetMembership().HasLeader());
    EXPECT_EQ(group.GetMembership().CountMembers(), (size_t)0);
}

GAME_TEST(CPedGroup2, GetMemberId_Roundtrip) {
    int32 pedModel = Scenario::FindLoadedPedModel();
    if (pedModel < 0) return;

    CVector basePos = FindPlayerCoors() + CVector(45.0f, 0.0f, 0.0f);
    Scenario::TestPed leader(PED_TYPE_CIVMALE, pedModel, basePos);
    Scenario::TestPed follower(PED_TYPE_CIVMALE, pedModel, basePos + CVector(3.0f, 0.0f, 0.0f));
    if (!leader || !follower) return;

    CPedGroup group;
    group.GetMembership().SetLeader(leader.AsPed());
    group.GetMembership().AddFollower(follower.AsPed());

    // Leader should have ID == LEADER_MEM_ID (7)
    int32 leaderId = group.GetMembership().GetMemberId(leader.AsPed());
    EXPECT_EQ(leaderId, CPedGroupMembership::LEADER_MEM_ID);

    // Follower should have a valid ID 0-6
    int32 followerId = group.GetMembership().GetMemberId(follower.AsPed());
    EXPECT_TRUE(followerId >= 0 && followerId < CPedGroupMembership::LEADER_MEM_ID);

    // GetMember round-trip
    EXPECT_EQ((uintptr_t)group.GetMembership().GetMember(leaderId), (uintptr_t)leader.AsPed());
    EXPECT_EQ((uintptr_t)group.GetMembership().GetMember(followerId), (uintptr_t)follower.AsPed());

    SafeRemoveFromGroup(group.GetMembership(), follower.AsPed());
    SafeRemoveFromGroup(group.GetMembership(), leader.AsPed());
}
