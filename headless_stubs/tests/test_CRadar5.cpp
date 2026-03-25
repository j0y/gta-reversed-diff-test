// test_CRadar5.cpp — More differential tests for CRadar.
// Hook paths: Global/CRadar/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "Radar.h"

// --- CalculateBlipAlpha (0x583420) ---

GAME_DIFF_TEST(CRadar, CalculateBlipAlpha_Distances) {
    float distances[] = { 0.0f, 50.0f, 100.0f, 500.0f, 1000.0f };
    for (float dist : distances) {
        uint8 orig, rev;
        { HookDisableGuard guard("Global/CRadar/CalculateBlipAlpha");
          orig = CRadar::CalculateBlipAlpha(dist); }
        rev = CRadar::CalculateBlipAlpha(dist);
        EXPECT_EQ(orig, rev);
    }
}

// --- GetActualBlipArrayIndex (0x582870) ---

GAME_DIFF_TEST(CRadar, GetActualBlipArrayIndex_Handles) {
    tBlipHandle handles[] = { 0, 1, (tBlipHandle)-1 };
    for (auto handle : handles) {
        int32 orig, rev;
        { HookDisableGuard guard("Global/CRadar/GetActualBlipArrayIndex");
          orig = CRadar::GetActualBlipArrayIndex(handle); }
        rev = CRadar::GetActualBlipArrayIndex(handle);
        EXPECT_EQ(orig, rev);
    }
}

// --- GetRadarTraceColour (0x584770) ---

GAME_DIFF_TEST(CRadar, GetRadarTraceColour_Various) {
    eBlipColour colours[] = {
        BLIP_COLOUR_RED, BLIP_COLOUR_GREEN, BLIP_COLOUR_BLUE, BLIP_COLOUR_WHITE,
        BLIP_COLOUR_YELLOW, BLIP_COLOUR_REDCOPY, BLIP_COLOUR_BLUECOPY,
        BLIP_COLOUR_THREAT, BLIP_COLOUR_DESTINATION
    };
    bool brights[] = { true, false };
    bool friendlies[] = { true, false };
    for (auto colour : colours) {
        for (bool bright : brights) {
            for (bool friendly : friendlies) {
                uint32 orig, rev;
                { HookDisableGuard guard("Global/CRadar/GetRadarTraceColour");
                  orig = CRadar::GetRadarTraceColour(colour, bright, friendly); }
                rev = CRadar::GetRadarTraceColour(colour, bright, friendly);
                EXPECT_EQ(orig, rev);
            }
        }
    }
}

// --- LimitRadarPoint (0x5832F0) ---

GAME_DIFF_TEST(CRadar, LimitRadarPoint_Extended) {
    CVector2D points[] = {
        { 0.0f, 0.0f },       // origin (inside)
        { 0.5f, 0.5f },       // inside
        { 1.0f, 0.0f },       // edge
        { 0.0f, 1.0f },       // edge
        { 2.0f, 2.0f },       // outside
        { -1.5f, 0.5f },      // outside
        { 0.3f, -0.3f },      // inside
        { 10.0f, -10.0f },    // far outside
    };
    for (auto& pt : points) {
        CVector2D origPt = pt;
        CVector2D revPt = pt;
        float orig, rev;
        { HookDisableGuard guard("Global/CRadar/LimitRadarPoint");
          orig = CRadar::LimitRadarPoint(origPt); }
        rev = CRadar::LimitRadarPoint(revPt);
        EXPECT_NEAR(orig, rev, 1e-3f);
        EXPECT_NEAR(origPt.x, revPt.x, 1e-3f);
        EXPECT_NEAR(origPt.y, revPt.y, 1e-3f);
    }
}
