// test_CCullZones3.cpp — Differential tests for CCullZones spatial queries.
// Hook paths: Global/CCullZones/<fn>
//
// Previously tested: InRoomForAudio, CamNoRain, DoWeNeedToLoadCollision, etc.
// (flag queries on CurrentFlags_Player)
//
// New targets:
//   FindAttributesForCoors        — iterates attribute zones, returns combined flags
//   FindTunnelAttributesForCoors  — iterates tunnel zones, returns combined flags
//   FindMirrorAttributesForCoors  — iterates mirror zones, returns pointer or null
//   Cam1stPersonForPlayer         — flag check on CurrentFlags_Player
//   DoINeedToLoadCollision        — flag check on CurrentFlags_Player

#include "StdInc.h"
#include "TestFramework.h"
#include "CullZones.h"

// ── FindAttributesForCoors ──
// Iterates all attribute zones, tests if point is within each, OR's flags.

GAME_DIFF_TEST(CCullZones3, Diff_FindAttributesForCoors_Player) {
    auto* player = FindPlayerPed(0);
    if (!player) return;

    eZoneAttributes orig, rev;
    { HookDisableGuard guard("Global/CCullZones/FindAttributesForCoors");
      orig = CCullZones::FindAttributesForCoors(player->GetPosition()); }
    rev = CCullZones::FindAttributesForCoors(player->GetPosition());
    EXPECT_EQ((int32)orig, (int32)rev);
}

GAME_DIFF_TEST(CCullZones3, Diff_FindAttributesForCoors_Positions) {
    CVector positions[] = {
        CVector(2488.f, -1665.f, 13.5f),     // Grove Street
        CVector(1545.f, -1675.f, 13.5f),     // LS downtown
        CVector(0.f, 0.f, 10.f),             // ocean center
        CVector(-2200.f, 200.f, 35.f),       // San Fierro
        CVector(2000.f, 1500.f, 11.f),       // Las Venturas
        CVector(400.f, -1500.f, 30.f),       // LS industrial
    };

    for (auto& pos : positions) {
        eZoneAttributes orig, rev;
        { HookDisableGuard guard("Global/CCullZones/FindAttributesForCoors");
          orig = CCullZones::FindAttributesForCoors(pos); }
        rev = CCullZones::FindAttributesForCoors(pos);
        EXPECT_EQ((int32)orig, (int32)rev);
    }
}

// ── FindTunnelAttributesForCoors ──

GAME_DIFF_TEST(CCullZones3, Diff_FindTunnelAttributesForCoors_Positions) {
    CVector positions[] = {
        CVector(2488.f, -1665.f, 13.5f),
        CVector(0.f, 0.f, 10.f),
        CVector(1200.f, -800.f, 40.f),       // near LS tunnel area
        CVector(-2200.f, 200.f, 35.f),
    };

    for (auto& pos : positions) {
        eZoneAttributes orig, rev;
        { HookDisableGuard guard("Global/CCullZones/FindTunnelAttributesForCoors");
          orig = CCullZones::FindTunnelAttributesForCoors(pos); }
        rev = CCullZones::FindTunnelAttributesForCoors(pos);
        EXPECT_EQ((int32)orig, (int32)rev);
    }
}

// ── FindMirrorAttributesForCoors ──

GAME_DIFF_TEST(CCullZones3, Diff_FindMirrorAttributesForCoors_Positions) {
    CVector positions[] = {
        CVector(2488.f, -1665.f, 13.5f),
        CVector(0.f, 0.f, 10.f),
        CVector(-2200.f, 200.f, 35.f),
        CVector(2000.f, 1500.f, 11.f),
    };

    for (auto& pos : positions) {
        auto* orig = (void*)nullptr;
        auto* rev = (void*)nullptr;
        { HookDisableGuard guard("Global/CCullZones/FindMirrorAttributesForCoors");
          orig = CCullZones::FindMirrorAttributesForCoors(pos); }
        rev = CCullZones::FindMirrorAttributesForCoors(pos);
        EXPECT_EQ(orig, rev);
    }
}

// ── Cam1stPersonForPlayer ──

GAME_DIFF_TEST(CCullZones3, Diff_Cam1stPersonForPlayer) {
    bool orig, rev;
    { HookDisableGuard guard("Global/CCullZones/Cam1stPersonForPlayer");
      orig = CCullZones::Cam1stPersonForPlayer(); }
    rev = CCullZones::Cam1stPersonForPlayer();
    EXPECT_EQ(orig, rev);
}

// ── DoINeedToLoadCollision ──

GAME_DIFF_TEST(CCullZones3, Diff_DoINeedToLoadCollision) {
    bool orig, rev;
    { HookDisableGuard guard("Global/CCullZones/DoINeedToLoadCollision");
      orig = CCullZones::DoINeedToLoadCollision(); }
    rev = CCullZones::DoINeedToLoadCollision();
    EXPECT_EQ(orig, rev);
}
