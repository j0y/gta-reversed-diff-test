// test_CVehicle8.cpp — Differential tests for CVehicle query/calculation hooks.
// Hook paths: Vehicle/CVehicle/<fn>
//
// New targets:
//   HeightAboveCeiling       — altitude threshold calculation with flight model branches
//   ProcessWheelRotation     — dot product based wheel angle calculation
//   CarHasRoof               — handling convertible flag + extras query
//   GetPlaneOrdnanceRateOfFire — model-specific weapon rate lookup
//   GetTowHitchPos           — position from bounding box + matrix transform
//   GetTowBarPos             — model-specific tow bar position

#include "StdInc.h"
#include "TestFramework.h"
#include "ScenarioHelpers.h"

// ── HeightAboveCeiling ──
// Returns distance above the ceiling for a given altitude and flight model.
// Returns -1 when below ceiling. Has different thresholds per flight model.
// Non-static — needs a vehicle instance, but doesn't use `this`.

static CVehicle* GetVeh8() {
    static Scenario::TestVehicle* s_veh = nullptr;
    static bool s_tried = false;
    if (!s_tried) {
        s_tried = true;
        int32 model = Scenario::FindLoadedVehicleModel();
        if (model >= 0)
            s_veh = new Scenario::TestVehicle(model, CVector(2525.f, -1640.f, 13.5f));
    }
    return s_veh ? s_veh->AsVehicle() : nullptr;
}

GAME_DIFF_TEST(CVehicle8, Diff_HeightAboveCeiling_Default) {
    auto* v = GetVeh8();
    if (!v) return;
    // Default flight model: ceiling at 800
    float testHeights[] = { 0.f, 500.f, 799.f, 800.f, 801.f, 1000.f, 1500.f };
    for (float h : testHeights) {
        float orig, rev;
        { HookDisableGuard guard("Vehicle/CVehicle/HeightAboveCeiling");
          orig = v->HeightAboveCeiling(h, eFlightModel::FLIGHT_MODEL_PLANE); }
        rev = v->HeightAboveCeiling(h, eFlightModel::FLIGHT_MODEL_PLANE);
        EXPECT_NEAR(orig, rev, 1e-5f);
    }
}

GAME_DIFF_TEST(CVehicle8, Diff_HeightAboveCeiling_Baron) {
    auto* v = GetVeh8();
    if (!v) return;
    // Baron flight model: complex multi-range ceiling (500, 950, 1500)
    float testHeights[] = { 0.f, 499.f, 500.f, 501.f, 700.f, 949.f, 950.f,
                            951.f, 1200.f, 1499.f, 1500.f, 1501.f, 2000.f };
    for (float h : testHeights) {
        float orig, rev;
        { HookDisableGuard guard("Vehicle/CVehicle/HeightAboveCeiling");
          orig = v->HeightAboveCeiling(h, eFlightModel::FLIGHT_MODEL_BARON); }
        rev = v->HeightAboveCeiling(h, eFlightModel::FLIGHT_MODEL_BARON);
        EXPECT_NEAR(orig, rev, 1e-5f);
    }
}

GAME_DIFF_TEST(CVehicle8, Diff_HeightAboveCeiling_Heli) {
    auto* v = GetVeh8();
    if (!v) return;
    float testHeights[] = { 0.f, 500.f, 799.f, 800.f, 801.f, 1200.f };
    for (float h : testHeights) {
        float orig, rev;
        { HookDisableGuard guard("Vehicle/CVehicle/HeightAboveCeiling");
          orig = v->HeightAboveCeiling(h, eFlightModel::FLIGHT_MODEL_HELI); }
        rev = v->HeightAboveCeiling(h, eFlightModel::FLIGHT_MODEL_HELI);
        EXPECT_NEAR(orig, rev, 1e-5f);
    }
}

// ── ProcessWheelRotation ──
// Returns wheel rotation angle from dot product of two vectors.
// Special cases: SPINNING returns -1.1, FIXED returns 0.

GAME_DIFF_TEST(CVehicle8, Diff_ProcessWheelRotation_Normal) {
    int32 model = Scenario::FindLoadedVehicleModel();
    if (model < 0) return;
    Scenario::TestVehicle veh(model, CVector(2525.f, -1640.f, 13.5f));

    CVector dir1(1.0f, 0.0f, 0.0f);
    CVector dir2(0.7f, 0.7f, 0.0f);
    float radius = 0.35f;

    float orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/ProcessWheelRotation");
      orig = veh.AsVehicle()->ProcessWheelRotation(WHEEL_STATE_NORMAL, dir1, dir2, radius); }
    rev = veh.AsVehicle()->ProcessWheelRotation(WHEEL_STATE_NORMAL, dir1, dir2, radius);
    EXPECT_NEAR(orig, rev, 1e-5f);
}

GAME_DIFF_TEST(CVehicle8, Diff_ProcessWheelRotation_Spinning) {
    int32 model = Scenario::FindLoadedVehicleModel();
    if (model < 0) return;
    Scenario::TestVehicle veh(model, CVector(2525.f, -1640.f, 13.5f));

    CVector dir1(1.0f, 0.0f, 0.0f);
    CVector dir2(0.0f, 1.0f, 0.0f);

    float orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/ProcessWheelRotation");
      orig = veh.AsVehicle()->ProcessWheelRotation(WHEEL_STATE_SPINNING, dir1, dir2, 1.0f); }
    rev = veh.AsVehicle()->ProcessWheelRotation(WHEEL_STATE_SPINNING, dir1, dir2, 1.0f);
    EXPECT_NEAR(orig, rev, 1e-5f);
}

GAME_DIFF_TEST(CVehicle8, Diff_ProcessWheelRotation_Fixed) {
    int32 model = Scenario::FindLoadedVehicleModel();
    if (model < 0) return;
    Scenario::TestVehicle veh(model, CVector(2525.f, -1640.f, 13.5f));

    CVector dir1(1.0f, 0.0f, 0.0f);
    CVector dir2(1.0f, 0.0f, 0.0f);

    float orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/ProcessWheelRotation");
      orig = veh.AsVehicle()->ProcessWheelRotation(WHEEL_STATE_FIXED, dir1, dir2, 1.0f); }
    rev = veh.AsVehicle()->ProcessWheelRotation(WHEEL_STATE_FIXED, dir1, dir2, 1.0f);
    EXPECT_NEAR(orig, rev, 1e-5f);
}

GAME_DIFF_TEST(CVehicle8, Diff_ProcessWheelRotation_Directions) {
    int32 model = Scenario::FindLoadedVehicleModel();
    if (model < 0) return;
    Scenario::TestVehicle veh(model, CVector(2525.f, -1640.f, 13.5f));

    // Test with various direction combinations
    CVector dirs1[] = { CVector(1,0,0), CVector(0,1,0), CVector(0.7f,0.7f,0) };
    CVector dirs2[] = { CVector(0,1,0), CVector(1,0,0), CVector(-0.7f,0.7f,0) };

    for (int i = 0; i < 3; i++) {
        float orig, rev;
        { HookDisableGuard guard("Vehicle/CVehicle/ProcessWheelRotation");
          orig = veh.AsVehicle()->ProcessWheelRotation(WHEEL_STATE_NORMAL, dirs1[i], dirs2[i], 0.5f); }
        rev = veh.AsVehicle()->ProcessWheelRotation(WHEEL_STATE_NORMAL, dirs1[i], dirs2[i], 0.5f);
        EXPECT_NEAR(orig, rev, 1e-5f);
    }
}

// ── CarHasRoof ──

GAME_DIFF_TEST(CVehicle8, Diff_CarHasRoof) {
    int32 model = Scenario::FindLoadedVehicleModel();
    if (model < 0) return;
    Scenario::TestVehicle veh(model, CVector(2525.f, -1640.f, 13.5f));

    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/CarHasRoof");
      orig = veh.AsVehicle()->CarHasRoof(); }
    rev = veh.AsVehicle()->CarHasRoof();
    EXPECT_EQ(orig, rev);
}

// ── GetPlaneOrdnanceRateOfFire ──
// Test both ordnance types on a spawned vehicle (default case).

GAME_DIFF_TEST(CVehicle8, Diff_GetPlaneOrdnanceRateOfFire) {
    int32 model = Scenario::FindLoadedVehicleModel();
    if (model < 0) return;
    Scenario::TestVehicle veh(model, CVector(2525.f, -1640.f, 13.5f));

    for (int type = 0; type < 2; type++) {
        uint32 orig, rev;
        { HookDisableGuard guard("Vehicle/CVehicle/GetPlaneOrdnanceRateOfFire");
          orig = veh.AsVehicle()->GetPlaneOrdnanceRateOfFire(eOrdnanceType(type)); }
        rev = veh.AsVehicle()->GetPlaneOrdnanceRateOfFire(eOrdnanceType(type));
        EXPECT_EQ(orig, rev);
    }
}

// ── GetTowHitchPos ──

GAME_DIFF_TEST(CVehicle8, Diff_GetTowHitchPos) {
    int32 model = Scenario::FindLoadedVehicleModel();
    if (model < 0) return;
    Scenario::TestVehicle veh(model, CVector(2525.f, -1640.f, 13.5f));

    CVector origPos{}, revPos{};
    bool origRet, revRet;
    { HookDisableGuard guard("Vehicle/CVehicle/GetTowHitchPos");
      origRet = veh.AsVehicle()->GetTowHitchPos(origPos, true, nullptr); }
    revRet = veh.AsVehicle()->GetTowHitchPos(revPos, true, nullptr);
    EXPECT_EQ(origRet, revRet);
    if (origRet && revRet) {
        EXPECT_NEAR(origPos.x, revPos.x, 1e-3f);
        EXPECT_NEAR(origPos.y, revPos.y, 1e-3f);
        EXPECT_NEAR(origPos.z, revPos.z, 1e-3f);
    }
}

GAME_DIFF_TEST(CVehicle8, Diff_GetTowHitchPos_False) {
    int32 model = Scenario::FindLoadedVehicleModel();
    if (model < 0) return;
    Scenario::TestVehicle veh(model, CVector(2525.f, -1640.f, 13.5f));

    CVector origPos{}, revPos{};
    bool origRet, revRet;
    { HookDisableGuard guard("Vehicle/CVehicle/GetTowHitchPos");
      origRet = veh.AsVehicle()->GetTowHitchPos(origPos, false, nullptr); }
    revRet = veh.AsVehicle()->GetTowHitchPos(revPos, false, nullptr);
    EXPECT_EQ(origRet, revRet);
}

// ── GetTowBarPos ──

GAME_DIFF_TEST(CVehicle8, Diff_GetTowBarPos) {
    int32 model = Scenario::FindLoadedVehicleModel();
    if (model < 0) return;
    Scenario::TestVehicle veh(model, CVector(2525.f, -1640.f, 13.5f));

    CVector origPos{}, revPos{};
    bool origRet, revRet;
    { HookDisableGuard guard("Vehicle/CVehicle/GetTowBarPos");
      origRet = veh.AsVehicle()->GetTowBarPos(origPos, true, nullptr); }
    revRet = veh.AsVehicle()->GetTowBarPos(revPos, true, nullptr);
    EXPECT_EQ(origRet, revRet);
    if (origRet && revRet) {
        EXPECT_NEAR(origPos.x, revPos.x, 1e-3f);
        EXPECT_NEAR(origPos.y, revPos.y, 1e-3f);
        EXPECT_NEAR(origPos.z, revPos.z, 1e-3f);
    }
}

// ── Behavior: HeightAboveCeiling sanity ──

GAME_TEST(CVehicle8, HeightAboveCeiling_BelowCeiling) {
    auto* v = GetVeh8();
    if (!v) return;
    // Below 800 should return -1 for default flight model
    EXPECT_NEAR(v->HeightAboveCeiling(500.f, eFlightModel::FLIGHT_MODEL_PLANE), -1.f, 1e-5f);
    EXPECT_NEAR(v->HeightAboveCeiling(799.f, eFlightModel::FLIGHT_MODEL_PLANE), -1.f, 1e-5f);
}

GAME_TEST(CVehicle8, HeightAboveCeiling_AboveCeiling) {
    auto* v = GetVeh8();
    if (!v) return;
    // At 900, should return 100 (900-800)
    EXPECT_NEAR(v->HeightAboveCeiling(900.f, eFlightModel::FLIGHT_MODEL_PLANE), 100.f, 1e-5f);
}

GAME_TEST(CVehicle8, HeightAboveCeiling_Baron_Ranges) {
    auto* v = GetVeh8();
    if (!v) return;
    // Baron: below 500 → -1
    EXPECT_NEAR(v->HeightAboveCeiling(400.f, eFlightModel::FLIGHT_MODEL_BARON), -1.f, 1e-5f);
    // Baron: 500-950 → height - 500
    EXPECT_NEAR(v->HeightAboveCeiling(700.f, eFlightModel::FLIGHT_MODEL_BARON), 200.f, 1e-5f);
    // Baron: 950-1500 → -1 (gap)
    EXPECT_NEAR(v->HeightAboveCeiling(1200.f, eFlightModel::FLIGHT_MODEL_BARON), -1.f, 1e-5f);
    // Baron: >=1500 → (height-1000)+500
    EXPECT_NEAR(v->HeightAboveCeiling(1600.f, eFlightModel::FLIGHT_MODEL_BARON), 1100.f, 1e-5f);
}
