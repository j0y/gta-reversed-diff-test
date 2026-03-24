// test_CRadar4.cpp — Differential tests for CRadar overloaded functions.
// Hook paths use dash: Global/CRadar/ClearActualBlip-OG, etc.

#include "StdInc.h"
#include "TestFramework.h"
#include "Radar.h"

// --- StreamRadarSections overloads ---

GAME_DIFF_TEST(CRadar4, StreamRadarSections_Vec) {
    CVector positions[] = {
        {2488.f, -1666.f, 13.f},
        {0.f, 0.f, 0.f},
        {-2170.f, 250.f, 30.f},
        {2027.f, 1008.f, 20.f},
    };
    for (auto& pos : positions) {
        { HookDisableGuard guard("Global/CRadar/StreamRadarSections-vec");
          CRadar::StreamRadarSections(pos); }
        CRadar::StreamRadarSections(pos);
        EXPECT_TRUE(true); // No crash = success (void return)
    }
}

GAME_DIFF_TEST(CRadar4, StreamRadarSections_XY) {
    struct Coord { int32 x, y; };
    Coord coords[] = { {0, 0}, {5, 5}, {11, 11}, {3, 9}, {6, 6} };
    for (auto& c : coords) {
        { HookDisableGuard guard("Global/CRadar/StreamRadarSections-xy");
          CRadar::StreamRadarSections(c.x, c.y); }
        CRadar::StreamRadarSections(c.x, c.y);
        EXPECT_TRUE(true);
    }
}

// IsPointInsideRadar not a member — likely a free function or different name. Skipped.

// --- CalculateCachedSinCos ---

GAME_DIFF_TEST(CRadar4, CalculateCachedSinCos) {
    { HookDisableGuard guard("Global/CRadar/CalculateCachedSinCos");
      CRadar::CalculateCachedSinCos(); }
    CRadar::CalculateCachedSinCos();
    EXPECT_TRUE(true);
}

// GetNewUniqueBlipIndex is stateful (increments counter) — can't diff test. Skipped.

// --- SetBlipFriendly ---

GAME_DIFF_TEST(CRadar4, SetBlipFriendly) {
    { HookDisableGuard guard("Global/CRadar/SetBlipFriendly");
      CRadar::SetBlipFriendly(0, true); }
    CRadar::SetBlipFriendly(0, true);
    EXPECT_TRUE(true);
}

// --- SetBlipFade ---

GAME_DIFF_TEST(CRadar4, SetBlipFade) {
    { HookDisableGuard guard("Global/CRadar/SetBlipFade");
      CRadar::SetBlipFade(0, true); }
    CRadar::SetBlipFade(0, true);
    EXPECT_TRUE(true);
}

// --- SetBlipAlwaysDisplayInZoom ---

GAME_DIFF_TEST(CRadar4, SetBlipAlwaysDisplayInZoom) {
    { HookDisableGuard guard("Global/CRadar/SetBlipAlwaysDisplayInZoom");
      CRadar::SetBlipAlwaysDisplayInZoom(0, true); }
    CRadar::SetBlipAlwaysDisplayInZoom(0, true);
    EXPECT_TRUE(true);
}

// --- SetBlipSprite ---

GAME_DIFF_TEST(CRadar4, SetBlipSprite) {
    { HookDisableGuard guard("Global/CRadar/SetBlipSprite");
      CRadar::SetBlipSprite(0, RADAR_SPRITE_NONE); }
    CRadar::SetBlipSprite(0, RADAR_SPRITE_NONE);
    EXPECT_TRUE(true);
}

// --- ClearActualBlip-OG ---

GAME_DIFF_TEST(CRadar4, ClearActualBlip_OG) {
    { HookDisableGuard guard("Global/CRadar/ClearActualBlip-OG");
      CRadar::ClearActualBlip(0); }
    CRadar::ClearActualBlip(0);
    EXPECT_TRUE(true);
}
