// test_CPedGroupMembership3.cpp — More CPedGroupMembership differential tests.
// Hook paths: Global/CPedGroupMembership/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "PedGroupMembership.h"
#include "PedGroup.h"

static CPedGroupMembership* GetPlayerGroupMembership3() {
    auto& group = CPedGroups::GetGroup(0);
    return &group.GetMembership();
}

GAME_DIFF_TEST(CPedGroupMembership3, GetMember_AllSlots) {
    auto* membership = GetPlayerGroupMembership3();
    if (!membership) return;
    for (int32 slot = 0; slot < 8; slot++) {
        CPed* orig;
        CPed* rev;
        { HookDisableGuard guard("Global/CPedGroupMembership/GetMember");
          orig = membership->GetMember(slot); }
        rev = membership->GetMember(slot);
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(CPedGroupMembership3, IsFollower_Nullptr) {
    auto* membership = GetPlayerGroupMembership3();
    if (!membership) return;
    bool orig, rev;
    { HookDisableGuard guard("Global/CPedGroupMembership/IsFollower");
      orig = membership->IsFollower(nullptr); }
    rev = membership->IsFollower(nullptr);
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CPedGroupMembership3, IsMember_Nullptr) {
    auto* membership = GetPlayerGroupMembership3();
    if (!membership) return;
    bool orig, rev;
    { HookDisableGuard guard("Global/CPedGroupMembership/IsMember");
      orig = membership->IsMember(nullptr); }
    rev = membership->IsMember(nullptr);
    EXPECT_EQ(orig, rev);
}
