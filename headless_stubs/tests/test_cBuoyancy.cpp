// test_cBuoyancy.cpp — Differential tests for cBuoyancy.
// Hook paths: Global/cBuoyancy/<fn>
//
// Testable hooks:
//   PreCalcSetup       — sets up bounding box, center offset, etc. from entity
//   CalcBuoyancyForce  — computes buoyancy force vector from internal state
//   FindWaterLevel     — finds water level at offset point relative to entity
//   FindWaterLevelNorm — same as above + wave normal
//   SimpleSumBuoyancyData — accumulates buoyancy contribution from one sample point
//   ProcessBuoyancy    — full buoyancy pipeline for an entity
//
// Skipped:
//   AddSplashParticles   — depends on g_fx particle system + audio
//   ProcessBuoyancyBoat  — needs spawned boat in water, complex setup
//   SimpleCalcBuoyancy   — calls AddSplashParticles internally

#include "StdInc.h"
#include "TestFramework.h"
#include "Buoyancy.h"
#include "ScenarioHelpers.h"

extern cBuoyancy& mod_Buoyancy;

// Save/restore helper for the global cBuoyancy state
struct BuoyancyStateSnapshot {
    cBuoyancy state;
    float pointVolMult;
    CBuoyancyCalcStruct calcStruct;

    void Save() {
        memcpy(&state, &mod_Buoyancy, sizeof(cBuoyancy));
        pointVolMult = cBuoyancy::fPointVolMultiplier;
        memcpy(&calcStruct, &cBuoyancy::calcStruct, sizeof(CBuoyancyCalcStruct));
    }
    void Restore() {
        memcpy(&mod_Buoyancy, &state, sizeof(cBuoyancy));
        cBuoyancy::fPointVolMultiplier = pointVolMult;
        memcpy(&cBuoyancy::calcStruct, &calcStruct, sizeof(CBuoyancyCalcStruct));
    }
};

// ── PreCalcSetup ──
// Sets up internal bounding box, center offset, and buoyancy params from entity.
// Compare the resulting internal state between original and reversed.

GAME_DIFF_TEST(cBuoyancy, Diff_PreCalcSetup_Player) {
    auto* player = FindPlayerPed(0);
    if (!player) return;

    BuoyancyStateSnapshot snap;
    float buoyancy = 1.0f;

    // Need a valid water level for PreCalcSetup
    mod_Buoyancy.m_fWaterLevel = 0.0f;
    mod_Buoyancy.m_fUnkn2 = 0.0f;
    mod_Buoyancy.m_EntityMatrix = *player->m_matrix;

    snap.Save();
    { HookDisableGuard guard("Global/cBuoyancy/PreCalcSetup");
      mod_Buoyancy.PreCalcSetup(player, buoyancy); }
    CVector origBMin = mod_Buoyancy.m_vecBoundingMin;
    CVector origBMax = mod_Buoyancy.m_vecBoundingMax;
    CVector origCOff = mod_Buoyancy.m_vecCenterOffset;
    CVector origNCOff = mod_Buoyancy.m_vecNormalizedCenterOffset;
    bool    origBoat = mod_Buoyancy.m_bProcessingBoat;

    snap.Restore();
    mod_Buoyancy.m_fWaterLevel = 0.0f;
    mod_Buoyancy.m_fUnkn2 = 0.0f;
    mod_Buoyancy.m_EntityMatrix = *player->m_matrix;
    mod_Buoyancy.PreCalcSetup(player, buoyancy);
    CVector revBMin = mod_Buoyancy.m_vecBoundingMin;
    CVector revBMax = mod_Buoyancy.m_vecBoundingMax;
    CVector revCOff = mod_Buoyancy.m_vecCenterOffset;
    CVector revNCOff = mod_Buoyancy.m_vecNormalizedCenterOffset;
    bool    revBoat = mod_Buoyancy.m_bProcessingBoat;

    EXPECT_EQ(origBoat, revBoat);
    EXPECT_NEAR(origBMin.x, revBMin.x, 1e-5f);
    EXPECT_NEAR(origBMin.y, revBMin.y, 1e-5f);
    EXPECT_NEAR(origBMin.z, revBMin.z, 1e-5f);
    EXPECT_NEAR(origBMax.x, revBMax.x, 1e-5f);
    EXPECT_NEAR(origBMax.y, revBMax.y, 1e-5f);
    EXPECT_NEAR(origBMax.z, revBMax.z, 1e-5f);
    EXPECT_NEAR(origCOff.x, revCOff.x, 1e-5f);
    EXPECT_NEAR(origCOff.y, revCOff.y, 1e-5f);
    EXPECT_NEAR(origCOff.z, revCOff.z, 1e-5f);
    EXPECT_NEAR(origNCOff.x, revNCOff.x, 1e-5f);
    EXPECT_NEAR(origNCOff.y, revNCOff.y, 1e-5f);
    EXPECT_NEAR(origNCOff.z, revNCOff.z, 1e-5f);

    snap.Restore();
}

GAME_DIFF_TEST(cBuoyancy, Diff_PreCalcSetup_Vehicle) {
    int32 model = Scenario::FindLoadedVehicleModel();
    if (model < 0) return;
    Scenario::TestVehicle veh(model, CVector(2525.f, -1640.f, 13.5f));
    auto* vehicle = veh.AsVehicle();

    BuoyancyStateSnapshot snap;
    float buoyancy = 0.5f;

    mod_Buoyancy.m_fWaterLevel = 0.0f;
    mod_Buoyancy.m_fUnkn2 = 0.0f;
    mod_Buoyancy.m_EntityMatrix = *vehicle->m_matrix;

    snap.Save();
    { HookDisableGuard guard("Global/cBuoyancy/PreCalcSetup");
      mod_Buoyancy.PreCalcSetup(vehicle, buoyancy); }
    CVector origBMin = mod_Buoyancy.m_vecBoundingMin;
    CVector origBMax = mod_Buoyancy.m_vecBoundingMax;
    CVector origCOff = mod_Buoyancy.m_vecCenterOffset;
    float   origBuoy = mod_Buoyancy.m_fBuoyancy;
    bool    origBoat = mod_Buoyancy.m_bProcessingBoat;

    snap.Restore();
    mod_Buoyancy.m_fWaterLevel = 0.0f;
    mod_Buoyancy.m_fUnkn2 = 0.0f;
    mod_Buoyancy.m_EntityMatrix = *vehicle->m_matrix;
    mod_Buoyancy.PreCalcSetup(vehicle, buoyancy);
    CVector revBMin = mod_Buoyancy.m_vecBoundingMin;
    CVector revBMax = mod_Buoyancy.m_vecBoundingMax;
    CVector revCOff = mod_Buoyancy.m_vecCenterOffset;
    float   revBuoy = mod_Buoyancy.m_fBuoyancy;
    bool    revBoat = mod_Buoyancy.m_bProcessingBoat;

    EXPECT_EQ(origBoat, revBoat);
    EXPECT_NEAR(origBMin.x, revBMin.x, 1e-5f);
    EXPECT_NEAR(origBMin.y, revBMin.y, 1e-5f);
    EXPECT_NEAR(origBMin.z, revBMin.z, 1e-5f);
    EXPECT_NEAR(origBMax.x, revBMax.x, 1e-5f);
    EXPECT_NEAR(origBMax.y, revBMax.y, 1e-5f);
    EXPECT_NEAR(origBMax.z, revBMax.z, 1e-5f);
    EXPECT_NEAR(origCOff.x, revCOff.x, 1e-5f);
    EXPECT_NEAR(origCOff.y, revCOff.y, 1e-5f);
    EXPECT_NEAR(origCOff.z, revCOff.z, 1e-5f);
    EXPECT_NEAR(origBuoy, revBuoy, 1e-5f);

    snap.Restore();
}

// ── CalcBuoyancyForce ──
// Reads internal state (m_bInWater, m_fEntityWaterImmersion, m_fBuoyancy, m_EntityMatrix)
// and writes output vectors. Returns true if entity is in water.

GAME_DIFF_TEST(cBuoyancy, Diff_CalcBuoyancyForce_InWater) {
    auto* player = FindPlayerPed(0);
    if (!player) return;

    BuoyancyStateSnapshot snap;

    // Set up known state: entity is in water
    mod_Buoyancy.m_bInWater = true;
    mod_Buoyancy.m_fEntityWaterImmersion = 0.6f;
    mod_Buoyancy.m_fBuoyancy = 1.0f;
    mod_Buoyancy.m_EntityMatrix = *player->m_matrix;
    mod_Buoyancy.m_vecTurnPoint = CVector(0.1f, 0.2f, -0.3f);

    CVector origTurn, origForce;
    bool origRet;
    snap.Save();
    { HookDisableGuard guard("Global/cBuoyancy/CalcBuoyancyForce");
      origRet = mod_Buoyancy.CalcBuoyancyForce(player, &origTurn, &origForce); }

    snap.Restore();
    mod_Buoyancy.m_bInWater = true;
    mod_Buoyancy.m_fEntityWaterImmersion = 0.6f;
    mod_Buoyancy.m_fBuoyancy = 1.0f;
    mod_Buoyancy.m_EntityMatrix = *player->m_matrix;
    mod_Buoyancy.m_vecTurnPoint = CVector(0.1f, 0.2f, -0.3f);

    CVector revTurn, revForce;
    bool revRet = mod_Buoyancy.CalcBuoyancyForce(player, &revTurn, &revForce);

    EXPECT_EQ(origRet, revRet);
    EXPECT_NEAR(origTurn.x, revTurn.x, 1e-4f);
    EXPECT_NEAR(origTurn.y, revTurn.y, 1e-4f);
    EXPECT_NEAR(origTurn.z, revTurn.z, 1e-4f);
    EXPECT_NEAR(origForce.x, revForce.x, 1e-4f);
    EXPECT_NEAR(origForce.y, revForce.y, 1e-4f);
    EXPECT_NEAR(origForce.z, revForce.z, 1e-4f);

    snap.Restore();
}

GAME_DIFF_TEST(cBuoyancy, Diff_CalcBuoyancyForce_NotInWater) {
    auto* player = FindPlayerPed(0);
    if (!player) return;

    BuoyancyStateSnapshot snap;
    snap.Save();

    mod_Buoyancy.m_bInWater = false;
    CVector origTurn, origForce;
    bool origRet;
    { HookDisableGuard guard("Global/cBuoyancy/CalcBuoyancyForce");
      origRet = mod_Buoyancy.CalcBuoyancyForce(player, &origTurn, &origForce); }

    mod_Buoyancy.m_bInWater = false;
    CVector revTurn, revForce;
    bool revRet = mod_Buoyancy.CalcBuoyancyForce(player, &revTurn, &revForce);

    EXPECT_EQ(origRet, revRet);
    EXPECT_EQ(false, revRet);

    snap.Restore();
}

GAME_DIFF_TEST(cBuoyancy, Diff_CalcBuoyancyForce_HighImmersion) {
    auto* player = FindPlayerPed(0);
    if (!player) return;

    BuoyancyStateSnapshot snap;

    mod_Buoyancy.m_bInWater = true;
    mod_Buoyancy.m_fEntityWaterImmersion = 1.0f;
    mod_Buoyancy.m_fBuoyancy = 2.0f;
    mod_Buoyancy.m_EntityMatrix = *player->m_matrix;
    mod_Buoyancy.m_vecTurnPoint = CVector(0.0f, 0.0f, 0.0f);

    CVector origTurn, origForce;
    bool origRet;
    snap.Save();
    { HookDisableGuard guard("Global/cBuoyancy/CalcBuoyancyForce");
      origRet = mod_Buoyancy.CalcBuoyancyForce(player, &origTurn, &origForce); }

    snap.Restore();
    mod_Buoyancy.m_bInWater = true;
    mod_Buoyancy.m_fEntityWaterImmersion = 1.0f;
    mod_Buoyancy.m_fBuoyancy = 2.0f;
    mod_Buoyancy.m_EntityMatrix = *player->m_matrix;
    mod_Buoyancy.m_vecTurnPoint = CVector(0.0f, 0.0f, 0.0f);

    CVector revTurn, revForce;
    bool revRet = mod_Buoyancy.CalcBuoyancyForce(player, &revTurn, &revForce);

    EXPECT_EQ(origRet, revRet);
    EXPECT_NEAR(origForce.z, revForce.z, 1e-4f);

    snap.Restore();
}

// ── FindWaterLevel ──
// Takes entity matrix position + offset, returns water level state.
// Depends on m_EntityMatrix, m_vecPos, m_vecBoundingMin/Max, CWaterLevel.

GAME_DIFF_TEST(cBuoyancy, Diff_FindWaterLevel_GroveStreet) {
    auto* player = FindPlayerPed(0);
    if (!player) return;

    BuoyancyStateSnapshot snap;

    // Set up state: entity at Grove Street (inland, above water)
    mod_Buoyancy.m_EntityMatrix = *player->m_matrix;
    mod_Buoyancy.m_vecPos = player->GetPosition();
    mod_Buoyancy.m_vecBoundingMin = CVector(-0.5f, -0.5f, -1.0f);
    mod_Buoyancy.m_vecBoundingMax = CVector(0.5f, 0.5f, 1.0f);

    CVector initZ(0.0f, 0.0f, player->GetPosition().z);
    CVector offset1(0.0f, 0.0f, 0.0f);
    CVector offset2(0.0f, 0.0f, 0.0f);
    tWaterLevel origState, revState;

    snap.Save();
    { HookDisableGuard guard("Global/cBuoyancy/FindWaterLevel");
      mod_Buoyancy.FindWaterLevel(initZ, &offset1, &origState); }

    snap.Restore();
    mod_Buoyancy.m_EntityMatrix = *player->m_matrix;
    mod_Buoyancy.m_vecPos = player->GetPosition();
    mod_Buoyancy.m_vecBoundingMin = CVector(-0.5f, -0.5f, -1.0f);
    mod_Buoyancy.m_vecBoundingMax = CVector(0.5f, 0.5f, 1.0f);
    mod_Buoyancy.FindWaterLevel(initZ, &offset2, &revState);

    EXPECT_EQ((int32)origState, (int32)revState);
    EXPECT_NEAR(offset1.z, offset2.z, 1e-4f);

    snap.Restore();
}

GAME_DIFF_TEST(cBuoyancy, Diff_FindWaterLevel_MultipleOffsets) {
    auto* player = FindPlayerPed(0);
    if (!player) return;

    BuoyancyStateSnapshot snap;

    CVector testOffsets[] = {
        CVector(-0.5f, -0.5f, 0.0f),
        CVector(0.0f, 0.0f, 0.0f),
        CVector(0.5f, 0.5f, 0.0f),
        CVector(-0.5f, 0.5f, 0.0f),
        CVector(0.5f, -0.5f, 0.0f),
    };

    CVector initZ(0.0f, 0.0f, player->GetPosition().z);

    for (auto& baseOff : testOffsets) {
        mod_Buoyancy.m_EntityMatrix = *player->m_matrix;
        mod_Buoyancy.m_vecPos = player->GetPosition();
        mod_Buoyancy.m_vecBoundingMin = CVector(-1.0f, -1.0f, -1.5f);
        mod_Buoyancy.m_vecBoundingMax = CVector(1.0f, 1.0f, 1.5f);

        CVector off1 = baseOff;
        tWaterLevel origState;
        snap.Save();
        { HookDisableGuard guard("Global/cBuoyancy/FindWaterLevel");
          mod_Buoyancy.FindWaterLevel(initZ, &off1, &origState); }

        snap.Restore();
        mod_Buoyancy.m_EntityMatrix = *player->m_matrix;
        mod_Buoyancy.m_vecPos = player->GetPosition();
        mod_Buoyancy.m_vecBoundingMin = CVector(-1.0f, -1.0f, -1.5f);
        mod_Buoyancy.m_vecBoundingMax = CVector(1.0f, 1.0f, 1.5f);

        CVector off2 = baseOff;
        tWaterLevel revState;
        mod_Buoyancy.FindWaterLevel(initZ, &off2, &revState);

        EXPECT_EQ((int32)origState, (int32)revState);
        EXPECT_NEAR(off1.z, off2.z, 1e-4f);
    }

    snap.Restore();
}

// ── FindWaterLevelNorm ──
// Same as FindWaterLevel but also returns wave normal vector.

GAME_DIFF_TEST(cBuoyancy, Diff_FindWaterLevelNorm) {
    auto* player = FindPlayerPed(0);
    if (!player) return;

    BuoyancyStateSnapshot snap;

    mod_Buoyancy.m_EntityMatrix = *player->m_matrix;
    mod_Buoyancy.m_vecPos = player->GetPosition();
    mod_Buoyancy.m_vecBoundingMin = CVector(-0.5f, -0.5f, -1.0f);
    mod_Buoyancy.m_vecBoundingMax = CVector(0.5f, 0.5f, 1.0f);

    CVector initZ(0.0f, 0.0f, player->GetPosition().z);
    CVector offset1(0.0f, 0.0f, 0.0f);
    CVector offset2(0.0f, 0.0f, 0.0f);
    CVector norm1(0.0f, 0.0f, 1.0f);
    CVector norm2(0.0f, 0.0f, 1.0f);
    tWaterLevel origState, revState;

    snap.Save();
    { HookDisableGuard guard("Global/cBuoyancy/FindWaterLevelNorm");
      mod_Buoyancy.FindWaterLevelNorm(initZ, &offset1, &origState, &norm1); }

    snap.Restore();
    mod_Buoyancy.m_EntityMatrix = *player->m_matrix;
    mod_Buoyancy.m_vecPos = player->GetPosition();
    mod_Buoyancy.m_vecBoundingMin = CVector(-0.5f, -0.5f, -1.0f);
    mod_Buoyancy.m_vecBoundingMax = CVector(0.5f, 0.5f, 1.0f);
    mod_Buoyancy.FindWaterLevelNorm(initZ, &offset2, &revState, &norm2);

    EXPECT_EQ((int32)origState, (int32)revState);
    EXPECT_NEAR(offset1.z, offset2.z, 1e-4f);
    EXPECT_NEAR(norm1.x, norm2.x, 1e-4f);
    EXPECT_NEAR(norm1.y, norm2.y, 1e-4f);
    EXPECT_NEAR(norm1.z, norm2.z, 1e-4f);

    snap.Restore();
}

// ── SimpleSumBuoyancyData ──
// Accumulates buoyancy contribution from a sample point.
// Reads: m_vecBoundingMin, m_bProcessingBoat, m_vecNormalizedCenterOffset,
//        m_fNumCheckedPoints, m_vecTurnPoint, m_bFlipUnknVector, fPointVolMultiplier
// Writes: m_fEntityWaterImmersion, m_vecTurnPoint, m_fNumCheckedPoints, m_bInWater

GAME_DIFF_TEST(cBuoyancy, Diff_SimpleSumBuoyancyData_Colliding) {
    BuoyancyStateSnapshot snap;

    // Set up known state
    mod_Buoyancy.m_vecBoundingMin = CVector(-1.0f, -1.0f, -1.0f);
    mod_Buoyancy.m_vecBoundingMax = CVector(1.0f, 1.0f, 1.0f);
    mod_Buoyancy.m_vecNormalizedCenterOffset = CVector(1.0f, 1.0f, 1.0f);
    mod_Buoyancy.m_fNumCheckedPoints = 1.0f;
    mod_Buoyancy.m_vecTurnPoint = CVector(0.0f, 0.0f, 0.0f);
    mod_Buoyancy.m_bFlipUnknVector = false;
    mod_Buoyancy.m_bProcessingBoat = false;
    mod_Buoyancy.m_bInWater = false;
    mod_Buoyancy.m_fEntityWaterImmersion = 0.0f;
    cBuoyancy::fPointVolMultiplier = 1.0f;
    cBuoyancy::calcStruct.bBuoyancyDataSummed = false;

    CVector waterOff(0.3f, 0.4f, 0.5f);
    snap.Save();
    CVector waterOff1 = waterOff;
    float origRet;
    { HookDisableGuard guard("Global/cBuoyancy/SimpleSumBuoyancyData");
      origRet = mod_Buoyancy.SimpleSumBuoyancyData(&waterOff1, tWaterLevel::COLLIDING_WITH_WATER); }
    float origImmersion = mod_Buoyancy.m_fEntityWaterImmersion;
    CVector origTurnPt = mod_Buoyancy.m_vecTurnPoint;
    bool origInWater = mod_Buoyancy.m_bInWater;

    snap.Restore();
    mod_Buoyancy.m_vecBoundingMin = CVector(-1.0f, -1.0f, -1.0f);
    mod_Buoyancy.m_vecBoundingMax = CVector(1.0f, 1.0f, 1.0f);
    mod_Buoyancy.m_vecNormalizedCenterOffset = CVector(1.0f, 1.0f, 1.0f);
    mod_Buoyancy.m_fNumCheckedPoints = 1.0f;
    mod_Buoyancy.m_vecTurnPoint = CVector(0.0f, 0.0f, 0.0f);
    mod_Buoyancy.m_bFlipUnknVector = false;
    mod_Buoyancy.m_bProcessingBoat = false;
    mod_Buoyancy.m_bInWater = false;
    mod_Buoyancy.m_fEntityWaterImmersion = 0.0f;
    cBuoyancy::fPointVolMultiplier = 1.0f;
    cBuoyancy::calcStruct.bBuoyancyDataSummed = false;

    CVector waterOff2 = waterOff;
    float revRet = mod_Buoyancy.SimpleSumBuoyancyData(&waterOff2, tWaterLevel::COLLIDING_WITH_WATER);
    float revImmersion = mod_Buoyancy.m_fEntityWaterImmersion;
    CVector revTurnPt = mod_Buoyancy.m_vecTurnPoint;
    bool revInWater = mod_Buoyancy.m_bInWater;

    EXPECT_NEAR(origRet, revRet, 1e-5f);
    EXPECT_NEAR(origImmersion, revImmersion, 1e-5f);
    EXPECT_NEAR(origTurnPt.x, revTurnPt.x, 1e-5f);
    EXPECT_NEAR(origTurnPt.y, revTurnPt.y, 1e-5f);
    EXPECT_NEAR(origTurnPt.z, revTurnPt.z, 1e-5f);
    EXPECT_EQ(origInWater, revInWater);

    snap.Restore();
}

GAME_DIFF_TEST(cBuoyancy, Diff_SimpleSumBuoyancyData_AboveWater) {
    BuoyancyStateSnapshot snap;

    // Point above water — should return 0 (below bounding min)
    mod_Buoyancy.m_vecBoundingMin = CVector(-1.0f, -1.0f, -1.0f);
    mod_Buoyancy.m_vecBoundingMax = CVector(1.0f, 1.0f, 1.0f);
    mod_Buoyancy.m_vecNormalizedCenterOffset = CVector(1.0f, 1.0f, 1.0f);
    mod_Buoyancy.m_fNumCheckedPoints = 1.0f;
    mod_Buoyancy.m_vecTurnPoint = CVector(0.0f, 0.0f, 0.0f);
    mod_Buoyancy.m_bFlipUnknVector = false;
    mod_Buoyancy.m_bProcessingBoat = false;
    mod_Buoyancy.m_bInWater = false;
    mod_Buoyancy.m_fEntityWaterImmersion = 0.0f;
    cBuoyancy::fPointVolMultiplier = 1.0f;
    cBuoyancy::calcStruct.bBuoyancyDataSummed = false;

    // z offset very close to bounding min => distance near 0 or negative
    CVector waterOff(0.0f, 0.0f, -0.9f);
    snap.Save();
    CVector waterOff1 = waterOff;
    float origRet;
    { HookDisableGuard guard("Global/cBuoyancy/SimpleSumBuoyancyData");
      origRet = mod_Buoyancy.SimpleSumBuoyancyData(&waterOff1, tWaterLevel::COMPLETELY_ABOVE_WATER); }

    snap.Restore();
    mod_Buoyancy.m_vecBoundingMin = CVector(-1.0f, -1.0f, -1.0f);
    mod_Buoyancy.m_vecBoundingMax = CVector(1.0f, 1.0f, 1.0f);
    mod_Buoyancy.m_vecNormalizedCenterOffset = CVector(1.0f, 1.0f, 1.0f);
    mod_Buoyancy.m_fNumCheckedPoints = 1.0f;
    mod_Buoyancy.m_vecTurnPoint = CVector(0.0f, 0.0f, 0.0f);
    mod_Buoyancy.m_bFlipUnknVector = false;
    mod_Buoyancy.m_bProcessingBoat = false;
    mod_Buoyancy.m_bInWater = false;
    mod_Buoyancy.m_fEntityWaterImmersion = 0.0f;
    cBuoyancy::fPointVolMultiplier = 1.0f;
    cBuoyancy::calcStruct.bBuoyancyDataSummed = false;

    CVector waterOff2 = waterOff;
    float revRet = mod_Buoyancy.SimpleSumBuoyancyData(&waterOff2, tWaterLevel::COMPLETELY_ABOVE_WATER);

    EXPECT_NEAR(origRet, revRet, 1e-5f);

    snap.Restore();
}

GAME_DIFF_TEST(cBuoyancy, Diff_SimpleSumBuoyancyData_Boat) {
    BuoyancyStateSnapshot snap;

    // Test with boat mode (uses pow() on contribution)
    mod_Buoyancy.m_vecBoundingMin = CVector(-2.0f, -4.0f, -0.5f);
    mod_Buoyancy.m_vecBoundingMax = CVector(2.0f, 4.0f, 1.0f);
    mod_Buoyancy.m_vecNormalizedCenterOffset = CVector(0.5f, 1.0f, 0.375f);
    mod_Buoyancy.m_fNumCheckedPoints = 3.0f;
    mod_Buoyancy.m_vecTurnPoint = CVector(0.1f, 0.2f, 0.05f);
    mod_Buoyancy.m_bFlipUnknVector = false;
    mod_Buoyancy.m_bProcessingBoat = true;
    mod_Buoyancy.m_bInWater = false;
    mod_Buoyancy.m_fEntityWaterImmersion = 0.5f;
    cBuoyancy::fPointVolMultiplier = 0.8f;
    cBuoyancy::calcStruct.bBuoyancyDataSummed = false;

    CVector waterOff(1.0f, 2.0f, 0.3f);
    snap.Save();
    CVector waterOff1 = waterOff;
    float origRet;
    { HookDisableGuard guard("Global/cBuoyancy/SimpleSumBuoyancyData");
      origRet = mod_Buoyancy.SimpleSumBuoyancyData(&waterOff1, tWaterLevel::COLLIDING_WITH_WATER); }
    float origImmersion = mod_Buoyancy.m_fEntityWaterImmersion;
    CVector origTurnPt = mod_Buoyancy.m_vecTurnPoint;

    snap.Restore();
    mod_Buoyancy.m_vecBoundingMin = CVector(-2.0f, -4.0f, -0.5f);
    mod_Buoyancy.m_vecBoundingMax = CVector(2.0f, 4.0f, 1.0f);
    mod_Buoyancy.m_vecNormalizedCenterOffset = CVector(0.5f, 1.0f, 0.375f);
    mod_Buoyancy.m_fNumCheckedPoints = 3.0f;
    mod_Buoyancy.m_vecTurnPoint = CVector(0.1f, 0.2f, 0.05f);
    mod_Buoyancy.m_bFlipUnknVector = false;
    mod_Buoyancy.m_bProcessingBoat = true;
    mod_Buoyancy.m_bInWater = false;
    mod_Buoyancy.m_fEntityWaterImmersion = 0.5f;
    cBuoyancy::fPointVolMultiplier = 0.8f;
    cBuoyancy::calcStruct.bBuoyancyDataSummed = false;

    CVector waterOff2 = waterOff;
    float revRet = mod_Buoyancy.SimpleSumBuoyancyData(&waterOff2, tWaterLevel::COLLIDING_WITH_WATER);
    float revImmersion = mod_Buoyancy.m_fEntityWaterImmersion;
    CVector revTurnPt = mod_Buoyancy.m_vecTurnPoint;

    EXPECT_NEAR(origRet, revRet, 1e-4f);
    EXPECT_NEAR(origImmersion, revImmersion, 1e-4f);
    EXPECT_NEAR(origTurnPt.x, revTurnPt.x, 1e-4f);
    EXPECT_NEAR(origTurnPt.y, revTurnPt.y, 1e-4f);
    EXPECT_NEAR(origTurnPt.z, revTurnPt.z, 1e-4f);

    snap.Restore();
}

GAME_DIFF_TEST(cBuoyancy, Diff_SimpleSumBuoyancyData_FlipVector) {
    BuoyancyStateSnapshot snap;

    // Test with flipped unknown vector
    mod_Buoyancy.m_vecBoundingMin = CVector(-1.0f, -1.0f, -1.0f);
    mod_Buoyancy.m_vecBoundingMax = CVector(1.0f, 1.0f, 1.0f);
    mod_Buoyancy.m_vecNormalizedCenterOffset = CVector(1.0f, 1.0f, 1.0f);
    mod_Buoyancy.m_fNumCheckedPoints = 2.0f;
    mod_Buoyancy.m_vecTurnPoint = CVector(0.5f, 0.5f, 0.1f);
    mod_Buoyancy.m_bFlipUnknVector = true;
    mod_Buoyancy.m_bProcessingBoat = false;
    mod_Buoyancy.m_bInWater = false;
    mod_Buoyancy.m_fEntityWaterImmersion = 0.0f;
    cBuoyancy::fPointVolMultiplier = 1.0f;
    cBuoyancy::calcStruct.bBuoyancyDataSummed = false;

    CVector waterOff(0.3f, 0.4f, 0.5f);
    snap.Save();
    CVector waterOff1 = waterOff;
    float origRet;
    { HookDisableGuard guard("Global/cBuoyancy/SimpleSumBuoyancyData");
      origRet = mod_Buoyancy.SimpleSumBuoyancyData(&waterOff1, tWaterLevel::COLLIDING_WITH_WATER); }
    CVector origTurnPt = mod_Buoyancy.m_vecTurnPoint;

    snap.Restore();
    mod_Buoyancy.m_vecBoundingMin = CVector(-1.0f, -1.0f, -1.0f);
    mod_Buoyancy.m_vecBoundingMax = CVector(1.0f, 1.0f, 1.0f);
    mod_Buoyancy.m_vecNormalizedCenterOffset = CVector(1.0f, 1.0f, 1.0f);
    mod_Buoyancy.m_fNumCheckedPoints = 2.0f;
    mod_Buoyancy.m_vecTurnPoint = CVector(0.5f, 0.5f, 0.1f);
    mod_Buoyancy.m_bFlipUnknVector = true;
    mod_Buoyancy.m_bProcessingBoat = false;
    mod_Buoyancy.m_bInWater = false;
    mod_Buoyancy.m_fEntityWaterImmersion = 0.0f;
    cBuoyancy::fPointVolMultiplier = 1.0f;
    cBuoyancy::calcStruct.bBuoyancyDataSummed = false;

    CVector waterOff2 = waterOff;
    float revRet = mod_Buoyancy.SimpleSumBuoyancyData(&waterOff2, tWaterLevel::COLLIDING_WITH_WATER);
    CVector revTurnPt = mod_Buoyancy.m_vecTurnPoint;

    EXPECT_NEAR(origRet, revRet, 1e-5f);
    EXPECT_NEAR(origTurnPt.x, revTurnPt.x, 1e-5f);
    EXPECT_NEAR(origTurnPt.y, revTurnPt.y, 1e-5f);
    EXPECT_NEAR(origTurnPt.z, revTurnPt.z, 1e-5f);

    snap.Restore();
}

// ── ProcessBuoyancy (integration) ──
// Full pipeline test on the player ped.

GAME_DIFF_TEST(cBuoyancy, Diff_ProcessBuoyancy_Player) {
    auto* player = FindPlayerPed(0);
    if (!player) return;

    BuoyancyStateSnapshot snap;
    snap.Save();

    CVector origTurn, origForce;
    bool origRet;
    { HookDisableGuard guard("Global/cBuoyancy/ProcessBuoyancy");
      origRet = mod_Buoyancy.ProcessBuoyancy(player, 1.0f, &origTurn, &origForce); }
    float origImmersion = mod_Buoyancy.m_fEntityWaterImmersion;

    snap.Restore();
    CVector revTurn, revForce;
    bool revRet = mod_Buoyancy.ProcessBuoyancy(player, 1.0f, &revTurn, &revForce);
    float revImmersion = mod_Buoyancy.m_fEntityWaterImmersion;

    EXPECT_EQ(origRet, revRet);
    if (origRet && revRet) {
        EXPECT_NEAR(origTurn.x, revTurn.x, 1e-3f);
        EXPECT_NEAR(origTurn.y, revTurn.y, 1e-3f);
        EXPECT_NEAR(origTurn.z, revTurn.z, 1e-3f);
        EXPECT_NEAR(origForce.x, revForce.x, 1e-3f);
        EXPECT_NEAR(origForce.y, revForce.y, 1e-3f);
        EXPECT_NEAR(origForce.z, revForce.z, 1e-3f);
    }
    EXPECT_NEAR(origImmersion, revImmersion, 1e-3f);

    snap.Restore();
}

GAME_DIFF_TEST(cBuoyancy, Diff_ProcessBuoyancy_Vehicle) {
    int32 model = Scenario::FindLoadedVehicleModel();
    if (model < 0) return;
    Scenario::TestVehicle veh(model, CVector(2525.f, -1640.f, 13.5f));
    auto* vehicle = veh.AsVehicle();

    BuoyancyStateSnapshot snap;
    snap.Save();

    CVector origTurn, origForce;
    bool origRet;
    { HookDisableGuard guard("Global/cBuoyancy/ProcessBuoyancy");
      origRet = mod_Buoyancy.ProcessBuoyancy(vehicle, 0.5f, &origTurn, &origForce); }
    float origImmersion = mod_Buoyancy.m_fEntityWaterImmersion;

    snap.Restore();
    CVector revTurn, revForce;
    bool revRet = mod_Buoyancy.ProcessBuoyancy(vehicle, 0.5f, &revTurn, &revForce);
    float revImmersion = mod_Buoyancy.m_fEntityWaterImmersion;

    EXPECT_EQ(origRet, revRet);
    if (origRet && revRet) {
        EXPECT_NEAR(origTurn.x, revTurn.x, 1e-3f);
        EXPECT_NEAR(origTurn.y, revTurn.y, 1e-3f);
        EXPECT_NEAR(origTurn.z, revTurn.z, 1e-3f);
        EXPECT_NEAR(origForce.x, revForce.x, 1e-3f);
        EXPECT_NEAR(origForce.y, revForce.y, 1e-3f);
        EXPECT_NEAR(origForce.z, revForce.z, 1e-3f);
    }
    EXPECT_NEAR(origImmersion, revImmersion, 1e-3f);

    snap.Restore();
}
