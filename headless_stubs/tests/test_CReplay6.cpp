// test_CReplay6.cpp — CReplay vehicle pool differential tests.
// Hook paths: Global/CReplay/<fn>
// Uses GetVehiclePool() to find live vehicle indices for realistic testing.

#include "StdInc.h"
#include "TestFramework.h"
#include "Replay.h"

// --- CanWeFindPoolIndexForVehicle with live pool index ---

GAME_DIFF_TEST(CReplay6, CanWeFindPoolIndexForVehicle_LivePool) {
    auto* pool = GetVehiclePool();
    for (int32 i = 0; i < pool->GetSize(); i++) {
        if (!pool->GetAt(i)) continue;
        bool orig, rev;
        { HookDisableGuard guard("Global/CReplay/CanWeFindPoolIndexForVehicle");
          orig = CReplay::CanWeFindPoolIndexForVehicle(i); }
        rev = CReplay::CanWeFindPoolIndexForVehicle(i);
        EXPECT_EQ(orig, rev);
        break; // test first valid entry
    }
}

GAME_DIFF_TEST(CReplay6, CanWeFindPoolIndexForVehicle_Invalid) {
    bool orig, rev;
    { HookDisableGuard guard("Global/CReplay/CanWeFindPoolIndexForVehicle");
      orig = CReplay::CanWeFindPoolIndexForVehicle(-1); }
    rev = CReplay::CanWeFindPoolIndexForVehicle(-1);
    EXPECT_EQ(orig, rev);
}

// --- FindPoolIndexForVehicle with live pool index ---

GAME_DIFF_TEST(CReplay6, FindPoolIndexForVehicle_LivePool) {
    auto* pool = GetVehiclePool();
    for (int32 i = 0; i < pool->GetSize(); i++) {
        if (!pool->GetAt(i)) continue;
        int32 orig, rev;
        { HookDisableGuard guard("Global/CReplay/FindPoolIndexForVehicle");
          orig = CReplay::FindPoolIndexForVehicle(i); }
        rev = CReplay::FindPoolIndexForVehicle(i);
        EXPECT_EQ(orig, rev);
        break;
    }
}

GAME_DIFF_TEST(CReplay6, FindPoolIndexForVehicle_Invalid) {
    int32 orig, rev;
    { HookDisableGuard guard("Global/CReplay/FindPoolIndexForVehicle");
      orig = CReplay::FindPoolIndexForVehicle(-1); }
    rev = CReplay::FindPoolIndexForVehicle(-1);
    EXPECT_EQ(orig, rev);
}

// --- IsThisVehicleUsedInRecording with live pool index ---

GAME_DIFF_TEST(CReplay6, IsThisVehicleUsedInRecording_LivePool) {
    auto* pool = GetVehiclePool();
    for (int32 i = 0; i < pool->GetSize(); i++) {
        if (!pool->GetAt(i)) continue;
        bool orig, rev;
        { HookDisableGuard guard("Global/CReplay/IsThisVehicleUsedInRecording");
          orig = CReplay::IsThisVehicleUsedInRecording(i); }
        rev = CReplay::IsThisVehicleUsedInRecording(i);
        EXPECT_EQ(orig, rev);
        break;
    }
}

GAME_DIFF_TEST(CReplay6, IsThisVehicleUsedInRecording_Invalid) {
    bool orig, rev;
    { HookDisableGuard guard("Global/CReplay/IsThisVehicleUsedInRecording");
      orig = CReplay::IsThisVehicleUsedInRecording(-1); }
    rev = CReplay::IsThisVehicleUsedInRecording(-1);
    EXPECT_EQ(orig, rev);
}
