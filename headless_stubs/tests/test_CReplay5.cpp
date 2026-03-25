// test_CReplay5.cpp — Extended CReplay differential tests.
// Hook paths: Global/CReplay/<fn>
// Tests replay query functions beyond what test_CReplay.cpp covers.

#include "StdInc.h"
#include "TestFramework.h"
#include "Replay.h"

GAME_DIFF_TEST(CReplay5, ShouldStandardCameraBeProcessed) {
    bool orig, rev;
    { HookDisableGuard guard("Global/CReplay/ShouldStandardCameraBeProcessed"); orig = CReplay::ShouldStandardCameraBeProcessed(); }
    rev = CReplay::ShouldStandardCameraBeProcessed();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CReplay5, NumberFramesAvailableToPlay) {
    uint32 orig, rev;
    { HookDisableGuard guard("Global/CReplay/NumberFramesAvailableToPlay"); orig = CReplay::NumberFramesAvailableToPlay(); }
    rev = CReplay::NumberFramesAvailableToPlay();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CReplay5, CanWeFindPoolIndexForPed) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    bool orig, rev;
    { HookDisableGuard guard("Global/CReplay/CanWeFindPoolIndexForPed"); orig = CReplay::CanWeFindPoolIndexForPed(GetPedPool()->GetIndex(player)); }
    rev = CReplay::CanWeFindPoolIndexForPed(GetPedPool()->GetIndex(player));
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CReplay5, FindPoolIndexForPed) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    int32 orig, rev;
    { HookDisableGuard guard("Global/CReplay/FindPoolIndexForPed"); orig = CReplay::FindPoolIndexForPed(GetPedPool()->GetIndex(player)); }
    rev = CReplay::FindPoolIndexForPed(GetPedPool()->GetIndex(player));
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CReplay5, IsThisPedUsedInRecording) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    int32 poolIdx = GetPedPool()->GetIndex(player);
    bool orig, rev;
    { HookDisableGuard guard("Global/CReplay/IsThisPedUsedInRecording"); orig = CReplay::IsThisPedUsedInRecording(poolIdx); }
    rev = CReplay::IsThisPedUsedInRecording(poolIdx);
    EXPECT_EQ(orig, rev);
}
