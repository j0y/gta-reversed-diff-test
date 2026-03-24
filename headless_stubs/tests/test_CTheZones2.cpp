// test_CTheZones2.cpp — Extended differential tests for CTheZones.
// Hook paths: Global/CTheZones/<fn>
//
// Tests zone geometry, lookups, and label search across major SA locations.

#include "StdInc.h"
#include "TestFramework.h"
#include "TheZones.h"

// --- FindZoneByLabel ---

GAME_DIFF_TEST(CTheZones2, FindZoneByLabel_KnownZones) {
    const char* labels[] = { "GANTON", "DOWNT", "DILLI", "UNITY", "STARFI" };
    for (auto* label : labels) {
        int16 orig, rev;
        { HookDisableGuard guard("Global/CTheZones/FindZoneByLabel");
          orig = CTheZones::FindZoneByLabel(label, ZONE_TYPE_NAVI); }
        rev = CTheZones::FindZoneByLabel(label, ZONE_TYPE_NAVI);
        EXPECT_EQ(orig, rev);
    }
}

// FindZoneByLabel with invalid name crashes original code (exception).
// Skipped — not a reversal bug, just undefined behavior in original.

GAME_DIFF_TEST(CTheZones2, FindZoneByLabel_InfoZones) {
    const char* labels[] = { "GANTON", "DOWNT" };
    for (auto* label : labels) {
        int16 orig, rev;
        { HookDisableGuard guard("Global/CTheZones/FindZoneByLabel");
          orig = CTheZones::FindZoneByLabel(label, ZONE_TYPE_INFO); }
        rev = CTheZones::FindZoneByLabel(label, ZONE_TYPE_INFO);
        EXPECT_EQ(orig, rev);
    }
}

// --- FindSmallestZoneForPosition ---

GAME_DIFF_TEST(CTheZones2, FindSmallestZone_MultiplePositions) {
    CVector positions[] = {
        {2488.f, -1666.f, 13.f},    // Grove Street LS
        {-2170.f, 250.f, 30.f},     // San Fierro
        {2027.f, 1008.f, 20.f},     // Las Venturas
        {0.f, 0.f, 0.f},            // Origin
        {-500.f, -500.f, 10.f},     // Countryside
    };
    for (auto& pos : positions) {
        CZone* orig;
        CZone* rev;
        { HookDisableGuard guard("Global/CTheZones/FindSmallestZoneForPosition");
          orig = CTheZones::FindSmallestZoneForPosition(pos, false); }
        rev = CTheZones::FindSmallestZoneForPosition(pos, false);
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(CTheZones2, FindSmallestZone_Type0Only) {
    CVector pos(2488.f, -1666.f, 13.f);
    CZone* orig;
    CZone* rev;
    { HookDisableGuard guard("Global/CTheZones/FindSmallestZoneForPosition");
      orig = CTheZones::FindSmallestZoneForPosition(pos, true); }
    rev = CTheZones::FindSmallestZoneForPosition(pos, true);
    EXPECT_EQ(orig, rev);
}

// --- GetNavigationZone / GetMapZone ---

GAME_DIFF_TEST(CTheZones2, GetNavigationZone_ValidIndices) {
    for (uint16 i = 0; i < 10; i++) {
        CZone* orig;
        CZone* rev;
        { HookDisableGuard guard("Global/CTheZones/GetNavigationZone");
          orig = CTheZones::GetNavigationZone(i); }
        rev = CTheZones::GetNavigationZone(i);
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(CTheZones2, GetMapZone_ValidIndices) {
    for (uint16 i = 0; i < 10; i++) {
        CZone* orig;
        CZone* rev;
        { HookDisableGuard guard("Global/CTheZones/GetMapZone");
          orig = CTheZones::GetMapZone(i); }
        rev = CTheZones::GetMapZone(i);
        EXPECT_EQ(orig, rev);
    }
}

// --- GetLevelFromPosition (extended coverage) ---

GAME_DIFF_TEST(CTheZones2, GetLevelFromPosition_Boundaries) {
    // Test near city boundary regions
    CVector positions[] = {
        {-1000.f, -500.f, 10.f},   // LS/SF boundary area
        {1000.f, 800.f, 10.f},     // LS/LV boundary area
        {-500.f, 1500.f, 10.f},    // SF/LV boundary area
        {-3000.f, -3000.f, 0.f},   // Far southwest (ocean)
        {3000.f, 3000.f, 0.f},     // Far northeast (ocean)
    };
    for (auto& pos : positions) {
        eLevelName orig, rev;
        { HookDisableGuard guard("Global/CTheZones/GetLevelFromPosition");
          orig = CTheZones::GetLevelFromPosition(pos); }
        rev = CTheZones::GetLevelFromPosition(pos);
        EXPECT_EQ((int)orig, (int)rev);
    }
}

// --- PointLiesWithinZone ---

GAME_DIFF_TEST(CTheZones2, PointLiesWithinZone_NaviZones) {
    CVector pos(2488.f, -1666.f, 13.f); // Grove Street
    // Get zone 0 and test against it
    CZone* zone = CTheZones::GetNavigationZone(0);
    if (!zone) return;
    bool orig, rev;
    { HookDisableGuard guard("Global/CTheZones/PointLiesWithinZone");
      orig = CTheZones::PointLiesWithinZone(&pos, zone); }
    rev = CTheZones::PointLiesWithinZone(&pos, zone);
    EXPECT_EQ(orig, rev);
}

// --- ZoneIsEntirelyContainedWithinOtherZone ---

GAME_DIFF_TEST(CTheZones2, ZoneContainment_NaviZones) {
    // Test a few pairs of navi zones
    for (uint16 i = 0; i < 5; i++) {
        CZone* z1 = CTheZones::GetNavigationZone(i);
        CZone* z2 = CTheZones::GetNavigationZone(0);
        if (!z1 || !z2) continue;
        bool orig, rev;
        { HookDisableGuard guard("Global/CTheZones/ZoneIsEntirelyContainedWithinOtherZone");
          orig = CTheZones::ZoneIsEntirelyContainedWithinOtherZone(z1, z2); }
        rev = CTheZones::ZoneIsEntirelyContainedWithinOtherZone(z1, z2);
        EXPECT_EQ(orig, rev);
    }
}

// --- Calc2DDistanceBetween2Zones ---

GAME_DIFF_TEST(CTheZones2, Calc2DDistance_ZonePairs) {
    for (uint16 i = 0; i < 5; i++) {
        for (uint16 j = i + 1; j < 5; j++) {
            CZone* z1 = CTheZones::GetNavigationZone(i);
            CZone* z2 = CTheZones::GetNavigationZone(j);
            if (!z1 || !z2) continue;
            float orig, rev;
            { HookDisableGuard guard("Global/CTheZones/Calc2DDistanceBetween2Zones");
              orig = CTheZones::Calc2DDistanceBetween2Zones(z1, z2); }
            rev = CTheZones::Calc2DDistanceBetween2Zones(z1, z2);
            EXPECT_NEAR(orig, rev, 1e-3f);
        }
    }
}
