// test_CPedGroupMembership2.cpp — Extended differential tests.
// Hook paths: Global/CPedGroupMembership/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "PedGroupMembership.h"

// --- GetObjectForPedToHold ---

GAME_DIFF_TEST(CPedGroupMembership2, GetObjectForPedToHold) {
    int32 orig, rev;
    { HookDisableGuard guard("Global/CPedGroupMembership/GetObjectForPedToHold");
      orig = (int32)CPedGroupMembership::GetObjectForPedToHold(); }
    rev = (int32)CPedGroupMembership::GetObjectForPedToHold();
    EXPECT_EQ(orig, rev);
}
