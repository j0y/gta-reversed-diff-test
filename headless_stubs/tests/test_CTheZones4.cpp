// test_CTheZones4.cpp — Additional CTheZones differential tests.
// Hook path: Global/CTheZones/<fn>
//
// Covers: PointLiesWithinZone, GetNavigationZone, GetMapZone,
//         FindSmallestZoneForPosition, ZoneIsEntirelyContainedWithinOtherZone
// with broader input coverage than test_CTheZones2.

#include "StdInc.h"
#include "TestFramework.h"
#include "TheZones.h"

// --- GetNavigationZone ---

GAME_DIFF_TEST(CTheZones4, GetNavigationZone_Indices0to5) {
    for (uint16 i = 0; i <= 5; i++) {
        CZone* orig;
        CZone* rev;
        { HookDisableGuard guard("Global/CTheZones/GetNavigationZone");
          orig = CTheZones::GetNavigationZone(i); }
        rev = CTheZones::GetNavigationZone(i);
        EXPECT_EQ(orig, rev);
    }
}

// --- GetMapZone ---

GAME_DIFF_TEST(CTheZones4, GetMapZone_Indices0to5) {
    for (uint16 i = 0; i <= 5; i++) {
        CZone* orig;
        CZone* rev;
        { HookDisableGuard guard("Global/CTheZones/GetMapZone");
          orig = CTheZones::GetMapZone(i); }
        rev = CTheZones::GetMapZone(i);
        EXPECT_EQ(orig, rev);
    }
}

// --- PointLiesWithinZone ---

GAME_DIFF_TEST(CTheZones4, PointLiesWithinZone_NavigationZones) {
    CVector positions[] = {
        {2488.f, -1666.f, 13.f},    // Grove Street LS
        {-2170.f, 250.f, 30.f},     // San Fierro
        {2027.f, 1008.f, 20.f},     // Las Venturas
        {0.f, 0.f, 0.f},            // Origin
    };
    for (uint16 zi = 0; zi <= 5; zi++) {
        CZone* zone = CTheZones::GetNavigationZone(zi);
        if (!zone) continue;
        for (auto& pos : positions) {
            bool orig, rev;
            { HookDisableGuard guard("Global/CTheZones/PointLiesWithinZone");
              orig = CTheZones::PointLiesWithinZone(&pos, zone); }
            rev = CTheZones::PointLiesWithinZone(&pos, zone);
            EXPECT_EQ(orig, rev);
        }
    }
}

GAME_DIFF_TEST(CTheZones4, PointLiesWithinZone_MapZones) {
    CVector positions[] = {
        {2488.f, -1666.f, 13.f},    // Grove Street LS
        {-2170.f, 250.f, 30.f},     // San Fierro
        {2027.f, 1008.f, 20.f},     // Las Venturas
    };
    for (uint16 zi = 0; zi <= 5; zi++) {
        CZone* zone = CTheZones::GetMapZone(zi);
        if (!zone) continue;
        for (auto& pos : positions) {
            bool orig, rev;
            { HookDisableGuard guard("Global/CTheZones/PointLiesWithinZone");
              orig = CTheZones::PointLiesWithinZone(&pos, zone); }
            rev = CTheZones::PointLiesWithinZone(&pos, zone);
            EXPECT_EQ(orig, rev);
        }
    }
}

// --- FindSmallestZoneForPosition ---

GAME_DIFF_TEST(CTheZones4, FindSmallestZoneForPosition_PlayerPos) {
    CVector playerPos = FindPlayerCoors(0);
    CZone* orig;
    CZone* rev;
    { HookDisableGuard guard("Global/CTheZones/FindSmallestZoneForPosition");
      orig = CTheZones::FindSmallestZoneForPosition(playerPos, false); }
    rev = CTheZones::FindSmallestZoneForPosition(playerPos, false);
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CTheZones4, FindSmallestZoneForPosition_GroveStreet) {
    CVector groveStreet(2488.f, -1666.f, 13.f);
    CZone* orig;
    CZone* rev;
    { HookDisableGuard guard("Global/CTheZones/FindSmallestZoneForPosition");
      orig = CTheZones::FindSmallestZoneForPosition(groveStreet, false); }
    rev = CTheZones::FindSmallestZoneForPosition(groveStreet, false);
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CTheZones4, FindSmallestZoneForPosition_Type0Only_PlayerPos) {
    CVector playerPos = FindPlayerCoors(0);
    CZone* orig;
    CZone* rev;
    { HookDisableGuard guard("Global/CTheZones/FindSmallestZoneForPosition");
      orig = CTheZones::FindSmallestZoneForPosition(playerPos, true); }
    rev = CTheZones::FindSmallestZoneForPosition(playerPos, true);
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CTheZones4, FindSmallestZoneForPosition_Type0Only_GroveStreet) {
    CVector groveStreet(2488.f, -1666.f, 13.f);
    CZone* orig;
    CZone* rev;
    { HookDisableGuard guard("Global/CTheZones/FindSmallestZoneForPosition");
      orig = CTheZones::FindSmallestZoneForPosition(groveStreet, true); }
    rev = CTheZones::FindSmallestZoneForPosition(groveStreet, true);
    EXPECT_EQ(orig, rev);
}

// --- ZoneIsEntirelyContainedWithinOtherZone ---

GAME_DIFF_TEST(CTheZones4, ZoneContainment_NaviPairs) {
    // Test pairs of navigation zones for containment
    for (uint16 i = 0; i <= 5; i++) {
        for (uint16 j = 0; j <= 5; j++) {
            CZone* z1 = CTheZones::GetNavigationZone(i);
            CZone* z2 = CTheZones::GetNavigationZone(j);
            if (!z1 || !z2) continue;
            bool orig, rev;
            { HookDisableGuard guard("Global/CTheZones/ZoneIsEntirelyContainedWithinOtherZone");
              orig = CTheZones::ZoneIsEntirelyContainedWithinOtherZone(z1, z2); }
            rev = CTheZones::ZoneIsEntirelyContainedWithinOtherZone(z1, z2);
            EXPECT_EQ(orig, rev);
        }
    }
}

GAME_DIFF_TEST(CTheZones4, ZoneContainment_MapPairs) {
    // Test pairs of map zones for containment
    for (uint16 i = 0; i <= 5; i++) {
        for (uint16 j = 0; j <= 5; j++) {
            CZone* z1 = CTheZones::GetMapZone(i);
            CZone* z2 = CTheZones::GetMapZone(j);
            if (!z1 || !z2) continue;
            bool orig, rev;
            { HookDisableGuard guard("Global/CTheZones/ZoneIsEntirelyContainedWithinOtherZone");
              orig = CTheZones::ZoneIsEntirelyContainedWithinOtherZone(z1, z2); }
            rev = CTheZones::ZoneIsEntirelyContainedWithinOtherZone(z1, z2);
            EXPECT_EQ(orig, rev);
        }
    }
}

GAME_DIFF_TEST(CTheZones4, ZoneContainment_NaviVsMap) {
    // Cross-compare navi zone inside map zone
    for (uint16 i = 0; i <= 3; i++) {
        CZone* navi = CTheZones::GetNavigationZone(i);
        CZone* map  = CTheZones::GetMapZone(i);
        if (!navi || !map) continue;
        bool orig, rev;
        { HookDisableGuard guard("Global/CTheZones/ZoneIsEntirelyContainedWithinOtherZone");
          orig = CTheZones::ZoneIsEntirelyContainedWithinOtherZone(navi, map); }
        rev = CTheZones::ZoneIsEntirelyContainedWithinOtherZone(navi, map);
        EXPECT_EQ(orig, rev);
    }
}
