// test_CWeather.cpp — Differential tests for CWeather.
// Hook paths: Global/CWeather/<fn>

#include "StdInc.h"
#include "TestFramework.h"

GAME_DIFF_TEST(CWeather, IsRainy) {
    bool orig, rev;
    { HookDisableGuard guard("Global/CWeather/IsRainy"); orig = CWeather::IsRainy(); }
    rev = CWeather::IsRainy();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CWeather, ForceWeather) {
    auto saved = CWeather::ForcedWeatherType;

    { HookDisableGuard guard("Global/CWeather/ForceWeather");
      CWeather::ForceWeather(eWeatherType::WEATHER_SUNNY_LA); }
    auto origVal = CWeather::ForcedWeatherType;
    CWeather::ForcedWeatherType = saved;

    CWeather::ForceWeather(eWeatherType::WEATHER_SUNNY_LA);
    auto revVal = CWeather::ForcedWeatherType;
    CWeather::ForcedWeatherType = saved;

    EXPECT_EQ(origVal, revVal);
}

GAME_DIFF_TEST(CWeather, ForceWeatherNow) {
    auto savedForced = CWeather::ForcedWeatherType;
    auto savedOld = CWeather::OldWeatherType;
    auto savedNew = CWeather::NewWeatherType;

    { HookDisableGuard guard("Global/CWeather/ForceWeatherNow");
      CWeather::ForceWeatherNow(eWeatherType::WEATHER_CLOUDY_LA); }
    auto origForced = CWeather::ForcedWeatherType;
    auto origOld = CWeather::OldWeatherType;
    auto origNew = CWeather::NewWeatherType;

    // Restore
    CWeather::ForcedWeatherType = savedForced;
    CWeather::OldWeatherType = savedOld;
    CWeather::NewWeatherType = savedNew;

    CWeather::ForceWeatherNow(eWeatherType::WEATHER_CLOUDY_LA);
    auto revForced = CWeather::ForcedWeatherType;
    auto revOld = CWeather::OldWeatherType;
    auto revNew = CWeather::NewWeatherType;

    // Restore
    CWeather::ForcedWeatherType = savedForced;
    CWeather::OldWeatherType = savedOld;
    CWeather::NewWeatherType = savedNew;

    EXPECT_EQ(origForced, revForced);
    EXPECT_EQ(origOld, revOld);
    EXPECT_EQ(origNew, revNew);
}

GAME_DIFF_TEST(CWeather, ReleaseWeather) {
    auto saved = CWeather::ForcedWeatherType;
    CWeather::ForcedWeatherType = eWeatherType::WEATHER_SUNNY_LA;

    { HookDisableGuard guard("Global/CWeather/ReleaseWeather"); CWeather::ReleaseWeather(); }
    auto origVal = CWeather::ForcedWeatherType;

    CWeather::ForcedWeatherType = eWeatherType::WEATHER_SUNNY_LA;
    CWeather::ReleaseWeather();
    auto revVal = CWeather::ForcedWeatherType;

    CWeather::ForcedWeatherType = saved;
    EXPECT_EQ(origVal, revVal);
}
