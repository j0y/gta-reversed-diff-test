// test_CReplay4.cpp — CReplay pool queries.
// Hook paths: Global/CReplay/<fn>
// FindPoolIndexForPed/Vehicle take int32 pool handle, not pointer.
// FindSizeOfPacket takes eReplayPacket enum.

#include "StdInc.h"
#include "TestFramework.h"
#include "Replay.h"

GAME_DIFF_TEST(CReplay4, FindPoolIndexForPed_Range) {
    for (int32 handle = 0; handle < 10; handle++) {
        int32 orig, rev;
        { HookDisableGuard guard("Global/CReplay/FindPoolIndexForPed");
          orig = CReplay::FindPoolIndexForPed(handle); }
        rev = CReplay::FindPoolIndexForPed(handle);
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(CReplay4, FindPoolIndexForVehicle_Range) {
    for (int32 handle = 0; handle < 10; handle++) {
        int32 orig, rev;
        { HookDisableGuard guard("Global/CReplay/FindPoolIndexForVehicle");
          orig = CReplay::FindPoolIndexForVehicle(handle); }
        rev = CReplay::FindPoolIndexForVehicle(handle);
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(CReplay4, FindSizeOfPacket_AllTypes) {
    for (int32 type = 0; type < 20; type++) {
        uint32 orig, rev;
        { HookDisableGuard guard("Global/CReplay/FindSizeOfPacket");
          orig = CReplay::FindSizeOfPacket(static_cast<eReplayPacket>(type)); }
        rev = CReplay::FindSizeOfPacket(static_cast<eReplayPacket>(type));
        EXPECT_EQ(orig, rev);
    }
}
