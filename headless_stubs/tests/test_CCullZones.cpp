// test_CCullZones.cpp — Differential tests for CCullZones.
// Hook paths: Global/CCullZones/<fn>
// All static bool getters with no args — read current cull zone flags.

#include "StdInc.h"
#include "TestFramework.h"
#include "CullZones.h"

#define DIFF_CULLZONE_BOOL(FnName) \
    GAME_DIFF_TEST(CCullZones, FnName) { \
        bool orig, rev; \
        { HookDisableGuard guard("Global/CCullZones/" #FnName); orig = CCullZones::FnName(); } \
        rev = CCullZones::FnName(); \
        EXPECT_EQ(orig, rev); \
    }

DIFF_CULLZONE_BOOL(CamCloseInForPlayer)
DIFF_CULLZONE_BOOL(CamStairsForPlayer)
DIFF_CULLZONE_BOOL(Cam1stPersonForPlayer)
DIFF_CULLZONE_BOOL(NoPolice)
DIFF_CULLZONE_BOOL(PoliceAbandonCars)
DIFF_CULLZONE_BOOL(InRoomForAudio)
DIFF_CULLZONE_BOOL(FewerCars)
DIFF_CULLZONE_BOOL(FewerPeds)
DIFF_CULLZONE_BOOL(CamNoRain)
DIFF_CULLZONE_BOOL(PlayerNoRain)
DIFF_CULLZONE_BOOL(DoExtraAirResistanceForPlayer)
DIFF_CULLZONE_BOOL(DoINeedToLoadCollision)
