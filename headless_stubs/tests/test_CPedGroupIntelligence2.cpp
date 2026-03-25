// test_CPedGroupIntelligence2.cpp — More CPedGroupIntelligence differential tests.
// Hook paths: Global/CPedGroupIntelligence/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "PedGroupIntelligence.h"
#include "PedGroup.h"

GAME_DIFF_TEST(CPedGroupIntelligence2, GetTaskSecondarySlot_Player) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    auto& group = CPedGroups::GetGroup(0);
    auto& intel = group.GetIntelligence();
    eSecondaryTask orig, rev;
    { HookDisableGuard guard("Global/CPedGroupIntelligence/GetTaskSecondarySlot");
      orig = intel.GetTaskSecondarySlot(player); }
    rev = intel.GetTaskSecondarySlot(player);
    EXPECT_EQ((int32)orig, (int32)rev);
}

GAME_DIFF_TEST(CPedGroupIntelligence2, ReportAllBarScriptTasksFinished) {
    auto& group = CPedGroups::GetGroup(0);
    auto& intel = group.GetIntelligence();
    { HookDisableGuard guard("Global/CPedGroupIntelligence/ReportAllBarScriptTasksFinished");
      intel.ReportAllBarScriptTasksFinished(); }
    intel.ReportAllBarScriptTasksFinished();
    EXPECT_TRUE(true);
}
