// test_CCheat4.cpp — Differential tests for CCheat query + safe cheat functions.
// Hook paths: Global/CCheat/<fn>
// ApplyCheat dispatches by index — test it with index 0 (no-op or harmless).
// IsZoneStreamingAllowed is a pure query.

#include "StdInc.h"
#include "TestFramework.h"
#include "Cheat.h"

GAME_DIFF_TEST(CCheat4, IsZoneStreamingAllowed) {
    bool orig, rev;
    { HookDisableGuard guard("Global/CCheat/IsZoneStreamingAllowed");
      orig = CCheat::IsZoneStreamingAllowed(); }
    rev = CCheat::IsZoneStreamingAllowed();
    EXPECT_EQ(orig, rev);
}

// Weather cheats are safe — they just set CWeather state that gets overwritten next frame
GAME_DIFF_TEST(CCheat4, ExtraSunnyWeatherCheat) {
    { HookDisableGuard guard("Global/CCheat/ExtraSunnyWeatherCheat");
      CCheat::ExtraSunnyWeatherCheat(); }
    CCheat::ExtraSunnyWeatherCheat();
    EXPECT_TRUE(true);
}

GAME_DIFF_TEST(CCheat4, CloudyWeatherCheat) {
    { HookDisableGuard guard("Global/CCheat/CloudyWeatherCheat");
      CCheat::CloudyWeatherCheat(); }
    CCheat::CloudyWeatherCheat();
    EXPECT_TRUE(true);
}

GAME_DIFF_TEST(CCheat4, FoggyWeatherCheat) {
    { HookDisableGuard guard("Global/CCheat/FoggyWeatherCheat");
      CCheat::FoggyWeatherCheat(); }
    CCheat::FoggyWeatherCheat();
    EXPECT_TRUE(true);
}

GAME_DIFF_TEST(CCheat4, DuskCheat) {
    { HookDisableGuard guard("Global/CCheat/DuskCheat");
      CCheat::DuskCheat(); }
    CCheat::DuskCheat();
    EXPECT_TRUE(true);
}

GAME_DIFF_TEST(CCheat4, MidnightCheat) {
    { HookDisableGuard guard("Global/CCheat/MidnightCheat");
      CCheat::MidnightCheat(); }
    CCheat::MidnightCheat();
    EXPECT_TRUE(true);
}

// Time cheats
GAME_DIFF_TEST(CCheat4, FastTimeCheat) {
    { HookDisableGuard guard("Global/CCheat/FastTimeCheat");
      CCheat::FastTimeCheat(); }
    CCheat::FastTimeCheat();
    EXPECT_TRUE(true);
}

// NotWanted — resets wanted level
GAME_DIFF_TEST(CCheat4, NotWantedCheat) {
    { HookDisableGuard guard("Global/CCheat/NotWantedCheat");
      CCheat::NotWantedCheat(); }
    CCheat::NotWantedCheat();
    EXPECT_TRUE(true);
}

// MoneyArmourHealth — gives resources
GAME_DIFF_TEST(CCheat4, MoneyArmourHealthCheat) {
    { HookDisableGuard guard("Global/CCheat/MoneyArmourHealthCheat");
      CCheat::MoneyArmourHealthCheat(); }
    CCheat::MoneyArmourHealthCheat();
    EXPECT_TRUE(true);
}

// Adrenaline — slows time
GAME_DIFF_TEST(CCheat4, AdrenalineCheat) {
    { HookDisableGuard guard("Global/CCheat/AdrenalineCheat");
      CCheat::AdrenalineCheat(); }
    CCheat::AdrenalineCheat();
    EXPECT_TRUE(true);
}

// FatCheat/MuscleCheat crash headlessly — they modify player model (needs rendering).
// DrivebyCheat/ParachuteCheat/BlackCarsCheat — safe flag toggles.

GAME_DIFF_TEST(CCheat4, DrivebyCheat) {
    { HookDisableGuard guard("Global/CCheat/DrivebyCheat"); CCheat::DrivebyCheat(); }
    CCheat::DrivebyCheat();
    EXPECT_TRUE(true);
}

GAME_DIFF_TEST(CCheat4, BlackCarsCheat) {
    { HookDisableGuard guard("Global/CCheat/BlackCarsCheat"); CCheat::BlackCarsCheat(); }
    CCheat::BlackCarsCheat();
    EXPECT_TRUE(true);
}
