// test_CTheZones3.cpp — Behavior tests for CTheZones overloaded functions.
// GetZoneInfo and FindZone are overloaded installs — hook paths include "" suffix
// which makes them hard to toggle. Test as behavior only.

#include "StdInc.h"
#include "TestFramework.h"
#include "TheZones.h"

// --- GetZoneInfo (behavior — verify no crash, returns non-null for known positions) ---

GAME_TEST(CTheZones3, GetZoneInfo_GroveStreet) {
    CVector pos(2488.f, -1666.f, 13.f);
    CZone* zone = nullptr;
    CZoneInfo* info = CTheZones::GetZoneInfo(pos, &zone);
    EXPECT_TRUE(info != nullptr);
    EXPECT_TRUE(zone != nullptr);
}

GAME_TEST(CTheZones3, GetZoneInfo_MultiplePositions) {
    CVector positions[] = {
        {2488.f, -1666.f, 13.f},
        {-2170.f, 250.f, 30.f},
        {2027.f, 1008.f, 20.f},
        {0.f, 0.f, 0.f},
    };
    for (auto& pos : positions) {
        CZone* zone = nullptr;
        CZoneInfo* info = CTheZones::GetZoneInfo(pos, &zone);
        EXPECT_TRUE(info != nullptr);
    }
}
