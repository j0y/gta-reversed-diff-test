// test_MiscQueries.cpp — Differential tests for misc query/calculation functions.
// All use Global hook paths.
//
// Targets:
//   CSprite::CalcScreenCoors      — world-to-screen projection (matrix * reciprocal depth)
//   CSprite::CalcHorizonCoors     — horizon Y position on screen
//   CCrime::FindImmediateDetectionRange — crime type → detection radius lookup
//   CGangWars::DoesPlayerControlThisZone — zone gang density comparison
//   CGangWars::CanPlayerStartAGangWarHere — zone info check
//   CWindModifiers::FindWindModifier — spatial distance + interpolation
//   CHud::GetYPosBasedOnHealth    — health-based Y offset
//   CHud::HelpMessageDisplayed    — state query

#include "StdInc.h"
#include "TestFramework.h"
#include "Sprite.h"
#include "TheZones.h"
#include "GangWars.h"
#include "WindModifiers.h"
#include "Hud.h"
#include "Crime.h"

// ── CSprite::CalcScreenCoors ──
// Projects a world position to screen coordinates using camera view matrix.
// Pure calculation: matrix transform + reciprocal depth + screen scaling.

GAME_DIFF_TEST(MiscQueries, Diff_CalcScreenCoors_PlayerPos) {
    auto* player = FindPlayerPed(0);
    if (!player) return;

    // Project a point slightly in front of the player
    RwV3d worldPos = { player->GetPosition().x + 5.f,
                       player->GetPosition().y,
                       player->GetPosition().z + 1.f };
    RwV3d origOut{}, revOut{};
    float origW, origH, revW, revH;

    bool origRet, revRet;
    { HookDisableGuard guard("Global/CSprite/CalcScreenCoors");
      origRet = CSprite::CalcScreenCoors(worldPos, &origOut, &origW, &origH, true, true); }
    revRet = CSprite::CalcScreenCoors(worldPos, &revOut, &revW, &revH, true, true);

    EXPECT_EQ(origRet, revRet);
    if (origRet && revRet) {
        EXPECT_NEAR(origOut.x, revOut.x, 1e-2f);
        EXPECT_NEAR(origOut.y, revOut.y, 1e-2f);
        EXPECT_NEAR(origOut.z, revOut.z, 1e-2f);
        EXPECT_NEAR(origW, revW, 1e-2f);
        EXPECT_NEAR(origH, revH, 1e-2f);
    }
}

GAME_DIFF_TEST(MiscQueries, Diff_CalcScreenCoors_Positions) {
    CVector positions[] = {
        CVector(2500.f, -1660.f, 15.f),
        CVector(2480.f, -1680.f, 20.f),
        CVector(2520.f, -1640.f, 10.f),
    };

    for (auto& pos : positions) {
        RwV3d worldPos = { pos.x, pos.y, pos.z };
        RwV3d origOut{}, revOut{};
        float origW, origH, revW, revH;

        { HookDisableGuard guard("Global/CSprite/CalcScreenCoors");
          CSprite::CalcScreenCoors(worldPos, &origOut, &origW, &origH, false, false); }
        CSprite::CalcScreenCoors(worldPos, &revOut, &revW, &revH, false, false);

        EXPECT_NEAR(origOut.x, revOut.x, 1e-1f);
        EXPECT_NEAR(origOut.y, revOut.y, 1e-1f);
        EXPECT_NEAR(origOut.z, revOut.z, 1e-1f);
    }
}

// ── CSprite::CalcHorizonCoors ──
// Computes horizon Y coordinate on screen from camera orientation.

GAME_DIFF_TEST(MiscQueries, Diff_CalcHorizonCoors) {
    float orig, rev;
    { HookDisableGuard guard("Global/CSprite/CalcHorizonCoors");
      orig = CSprite::CalcHorizonCoors(); }
    rev = CSprite::CalcHorizonCoors();
    EXPECT_NEAR(orig, rev, 1e-1f);
}

// ── CCrime::FindImmediateDetectionRange ──
// Pure lookup: crime type → float radius. Switch statement.

GAME_DIFF_TEST(MiscQueries, Diff_FindImmediateDetectionRange) {
    eCrimeType crimes[] = {
        CRIME_NONE, CRIME_FIRE_WEAPON, CRIME_DESTROY_VEHICLE,
        CRIME_DESTROY_HELI, CRIME_DESTROY_PLANE, CRIME_EXPLOSION,
        CRIME_DAMAGED_PED, CRIME_DAMAGED_COP, CRIME_STAB_PED,
        CRIME_CAR_STEAL, CRIME_AIM_GUN,
    };

    for (auto crime : crimes) {
        float orig, rev;
        { HookDisableGuard guard("Global/CCrime/FindImmediateDetectionRange");
          orig = CCrime::FindImmediateDetectionRange(crime); }
        rev = CCrime::FindImmediateDetectionRange(crime);
        EXPECT_NEAR(orig, rev, 1e-5f);
    }
}

// ── CGangWars::DoesPlayerControlThisZone ──
// Compares grove density vs enemy density in zone info.

GAME_DIFF_TEST(MiscQueries, Diff_DoesPlayerControlThisZone) {
    // Test on several zone infos
    for (int32 i = 0; i < std::min(10, (int32)CTheZones::TotalNumberOfNavigationZones); i++) {
        auto* zoneInfo = CTheZones::GetZoneInfo(&CTheZones::NavigationZoneArray[i]);
        if (!zoneInfo) continue;

        bool orig, rev;
        { HookDisableGuard guard("Global/CGangWars/DoesPlayerControlThisZone");
          orig = CGangWars::DoesPlayerControlThisZone(zoneInfo); }
        rev = CGangWars::DoesPlayerControlThisZone(zoneInfo);
        EXPECT_EQ(orig, rev);
    }
}

// ── CGangWars::CanPlayerStartAGangWarHere ──

GAME_DIFF_TEST(MiscQueries, Diff_CanPlayerStartAGangWarHere) {
    for (int32 i = 0; i < std::min(10, (int32)CTheZones::TotalNumberOfNavigationZones); i++) {
        auto* zoneInfo = CTheZones::GetZoneInfo(&CTheZones::NavigationZoneArray[i]);
        if (!zoneInfo) continue;

        bool orig, rev;
        { HookDisableGuard guard("Global/CGangWars/CanPlayerStartAGangWarHere");
          orig = CGangWars::CanPlayerStartAGangWarHere(zoneInfo); }
        rev = CGangWars::CanPlayerStartAGangWarHere(zoneInfo);
        EXPECT_EQ(orig, rev);
    }
}

// ── CWindModifiers::FindWindModifier ──
// Spatial query: finds wind modifier at position, interpolates by distance.

GAME_DIFF_TEST(MiscQueries, Diff_FindWindModifier_Positions) {
    CVector positions[] = {
        CVector(2488.f, -1665.f, 13.5f),
        CVector(0.f, 0.f, 50.f),
        CVector(-2200.f, 200.f, 35.f),
    };

    for (auto& pos : positions) {
        float origX = 0.f, origY = 0.f, revX = 0.f, revY = 0.f;
        bool origRet, revRet;
        { HookDisableGuard guard("Global/CWindModifiers/FindWindModifier");
          origRet = CWindModifiers::FindWindModifier(pos, &origX, &origY); }
        revRet = CWindModifiers::FindWindModifier(pos, &revX, &revY);
        EXPECT_EQ(origRet, revRet);
        if (origRet && revRet) {
            EXPECT_NEAR(origX, revX, 1e-3f);
            EXPECT_NEAR(origY, revY, 1e-3f);
        }
    }
}

// CHud::GetYPosBasedOnHealth and HelpMessageDisplayed skipped:
// CHud hooks registered but paths not found at runtime (known limitation).
