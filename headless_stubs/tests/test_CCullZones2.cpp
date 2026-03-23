// test_CCullZones2.cpp — Deep differential tests for CCullZones boolean queries.
// Hook paths: Global/CCullZones/<fn>
//
// All these are pure boolean queries on the current zone flags — no args needed.
// Any mismatch means the zone attribute lookup diverges.

#include "StdInc.h"
#include "TestFramework.h"
#include "CullZones.h"

GAME_DIFF_TEST(CCullZones2, CamCloseInForPlayer) {
    bool orig, rev;
    { HookDisableGuard guard("Global/CCullZones/CamCloseInForPlayer"); orig = CCullZones::CamCloseInForPlayer(); }
    rev = CCullZones::CamCloseInForPlayer();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CCullZones2, CamStairsForPlayer) {
    bool orig, rev;
    { HookDisableGuard guard("Global/CCullZones/CamStairsForPlayer"); orig = CCullZones::CamStairsForPlayer(); }
    rev = CCullZones::CamStairsForPlayer();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CCullZones2, Cam1stPersonForPlayer) {
    bool orig, rev;
    { HookDisableGuard guard("Global/CCullZones/Cam1stPersonForPlayer"); orig = CCullZones::Cam1stPersonForPlayer(); }
    rev = CCullZones::Cam1stPersonForPlayer();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CCullZones2, NoPolice) {
    bool orig, rev;
    { HookDisableGuard guard("Global/CCullZones/NoPolice"); orig = CCullZones::NoPolice(); }
    rev = CCullZones::NoPolice();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CCullZones2, PoliceAbandonCars) {
    bool orig, rev;
    { HookDisableGuard guard("Global/CCullZones/PoliceAbandonCars"); orig = CCullZones::PoliceAbandonCars(); }
    rev = CCullZones::PoliceAbandonCars();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CCullZones2, InRoomForAudio) {
    bool orig, rev;
    { HookDisableGuard guard("Global/CCullZones/InRoomForAudio"); orig = CCullZones::InRoomForAudio(); }
    rev = CCullZones::InRoomForAudio();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CCullZones2, FewerCars) {
    bool orig, rev;
    { HookDisableGuard guard("Global/CCullZones/FewerCars"); orig = CCullZones::FewerCars(); }
    rev = CCullZones::FewerCars();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CCullZones2, FewerPeds) {
    bool orig, rev;
    { HookDisableGuard guard("Global/CCullZones/FewerPeds"); orig = CCullZones::FewerPeds(); }
    rev = CCullZones::FewerPeds();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CCullZones2, CamNoRain) {
    bool orig, rev;
    { HookDisableGuard guard("Global/CCullZones/CamNoRain"); orig = CCullZones::CamNoRain(); }
    rev = CCullZones::CamNoRain();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CCullZones2, PlayerNoRain) {
    bool orig, rev;
    { HookDisableGuard guard("Global/CCullZones/PlayerNoRain"); orig = CCullZones::PlayerNoRain(); }
    rev = CCullZones::PlayerNoRain();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CCullZones2, DoExtraAirResistanceForPlayer) {
    bool orig, rev;
    { HookDisableGuard guard("Global/CCullZones/DoExtraAirResistanceForPlayer"); orig = CCullZones::DoExtraAirResistanceForPlayer(); }
    rev = CCullZones::DoExtraAirResistanceForPlayer();
    EXPECT_EQ(orig, rev);
}
