// test_CGameLogic2.cpp — Deep differential tests for CGameLogic.
// Hook paths: Global/CGameLogic/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "GameLogic.h"
#include "Entity/Ped/Ped.h"

// --- FindCityClosestToPoint ---

GAME_DIFF_TEST(CGameLogic, FindCityClosestToPoint_LosSantos) {
    int32 orig, rev;
    { HookDisableGuard guard("Global/CGameLogic/FindCityClosestToPoint");
      orig = (int32)CGameLogic::FindCityClosestToPoint(CVector2D(2495.0f, -1665.0f)); }
    rev = (int32)CGameLogic::FindCityClosestToPoint(CVector2D(2495.0f, -1665.0f));
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CGameLogic, FindCityClosestToPoint_SanFierro) {
    int32 orig, rev;
    { HookDisableGuard guard("Global/CGameLogic/FindCityClosestToPoint");
      orig = (int32)CGameLogic::FindCityClosestToPoint(CVector2D(-2170.0f, 250.0f)); }
    rev = (int32)CGameLogic::FindCityClosestToPoint(CVector2D(-2170.0f, 250.0f));
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CGameLogic, FindCityClosestToPoint_LasVenturas) {
    int32 orig, rev;
    { HookDisableGuard guard("Global/CGameLogic/FindCityClosestToPoint");
      orig = (int32)CGameLogic::FindCityClosestToPoint(CVector2D(2027.0f, 1008.0f)); }
    rev = (int32)CGameLogic::FindCityClosestToPoint(CVector2D(2027.0f, 1008.0f));
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CGameLogic, FindCityClosestToPoint_Countryside) {
    int32 orig, rev;
    { HookDisableGuard guard("Global/CGameLogic/FindCityClosestToPoint");
      orig = (int32)CGameLogic::FindCityClosestToPoint(CVector2D(0.0f, 0.0f)); }
    rev = (int32)CGameLogic::FindCityClosestToPoint(CVector2D(0.0f, 0.0f));
    EXPECT_EQ(orig, rev);
}

// --- IsCoopGameGoingOn ---

GAME_DIFF_TEST(CGameLogic, IsCoopGameGoingOn2) {
    bool orig, rev;
    { HookDisableGuard guard("Global/CGameLogic/IsCoopGameGoingOn");
      orig = CGameLogic::IsCoopGameGoingOn(); }
    rev = CGameLogic::IsCoopGameGoingOn();
    EXPECT_EQ(orig, rev);
}

// --- IsSkipWaitingForScriptToFadeIn ---

GAME_DIFF_TEST(CGameLogic, IsSkipWaitingForScriptToFadeIn) {
    bool orig, rev;
    { HookDisableGuard guard("Global/CGameLogic/IsSkipWaitingForScriptToFadeIn");
      orig = CGameLogic::IsSkipWaitingForScriptToFadeIn(); }
    rev = CGameLogic::IsSkipWaitingForScriptToFadeIn();
    EXPECT_EQ(orig, rev);
}

// --- LaRiotsActiveHere ---

GAME_DIFF_TEST(CGameLogic, LaRiotsActiveHere) {
    bool orig, rev;
    { HookDisableGuard guard("Global/CGameLogic/LaRiotsActiveHere");
      orig = CGameLogic::LaRiotsActiveHere(); }
    rev = CGameLogic::LaRiotsActiveHere();
    EXPECT_EQ(orig, rev);
}

// --- IsPlayerUse2PlayerControls ---

GAME_DIFF_TEST(CGameLogic, IsPlayerUse2PlayerControls) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    bool orig, rev;
    { HookDisableGuard guard("Global/CGameLogic/IsPlayerUse2PlayerControls");
      orig = CGameLogic::IsPlayerUse2PlayerControls(player); }
    rev = CGameLogic::IsPlayerUse2PlayerControls(player);
    EXPECT_EQ(orig, rev);
}

// --- CalcDistanceToForbiddenTrainCrossing ---
// Reversed code has two bugs:
//   1. Uses vecPoint * vecMoveSpeed instead of diff * vecMoveSpeed for direction check
//   2. Sets outDistance = vecPoint instead of the crossing position
//
// These tests pin down exactly where original and reversed diverge.

GAME_DIFF_TEST(CGameLogic, CalcDistToTrainCrossing_ReturnValue) {
    // Test return value (distance) from various positions
    CVector positions[] = {
        CVector(2495.0f, -1665.0f, 13.5f),   // Grove Street (LS)
        CVector(0.0f, 0.0f, 0.0f),            // world origin
        CVector(82.0f, -1021.0f, 10.0f),      // near LS-SF crossing
        CVector(-1568.0f, 537.0f, 10.0f),     // near SF-LV crossing
        CVector(500.0f, 500.0f, 10.0f),       // countryside
    };
    CVector moveSpeed(1.0f, 0.0f, 0.0f);
    for (auto& pos : positions) {
        CVector origOut, revOut;
        float orig, rev;
        { HookDisableGuard guard("Global/CGameLogic/CalcDistanceToForbiddenTrainCrossing");
          orig = CGameLogic::CalcDistanceToForbiddenTrainCrossing(pos, moveSpeed, false, origOut); }
        rev = CGameLogic::CalcDistanceToForbiddenTrainCrossing(pos, moveSpeed, false, revOut);
        EXPECT_NEAR(orig, rev, 1e-3f);
    }
}

GAME_DIFF_TEST(CGameLogic, CalcDistToTrainCrossing_IgnoreMoveSpeed) {
    // With ignoreMoveSpeed=true, direction check is bypassed
    CVector pos(2495.0f, -1665.0f, 13.5f);
    CVector moveSpeed(0.0f, 0.0f, 0.0f); // zero speed
    CVector origOut, revOut;
    float orig, rev;
    { HookDisableGuard guard("Global/CGameLogic/CalcDistanceToForbiddenTrainCrossing");
      orig = CGameLogic::CalcDistanceToForbiddenTrainCrossing(pos, moveSpeed, true, origOut); }
    rev = CGameLogic::CalcDistanceToForbiddenTrainCrossing(pos, moveSpeed, true, revOut);
    EXPECT_NEAR(orig, rev, 1e-3f);
}

GAME_DIFF_TEST(CGameLogic, CalcDistToTrainCrossing_OutDistance) {
    // Test the outDistance parameter — reversed sets it to vecPoint (bug),
    // original likely sets it to the crossing position or the diff vector
    CVector pos(2495.0f, -1665.0f, 13.5f);
    CVector moveSpeed(1.0f, 0.0f, 0.0f);
    CVector origOut{}, revOut{};
    { HookDisableGuard guard("Global/CGameLogic/CalcDistanceToForbiddenTrainCrossing");
      CGameLogic::CalcDistanceToForbiddenTrainCrossing(pos, moveSpeed, true, origOut); }
    CGameLogic::CalcDistanceToForbiddenTrainCrossing(pos, moveSpeed, true, revOut);
    EXPECT_NEAR(origOut.x, revOut.x, 1e-3f);
    EXPECT_NEAR(origOut.y, revOut.y, 1e-3f);
    EXPECT_NEAR(origOut.z, revOut.z, 1e-3f);
}

GAME_DIFF_TEST(CGameLogic, CalcDistToTrainCrossing_MoveSpeedDirections) {
    // Test with different move speed directions to expose the dot product bug
    CVector pos(200.0f, -500.0f, 10.0f);
    CVector speeds[] = {
        CVector(1.0f, 0.0f, 0.0f),   // moving east
        CVector(-1.0f, 0.0f, 0.0f),  // moving west
        CVector(0.0f, 1.0f, 0.0f),   // moving north
        CVector(0.0f, -1.0f, 0.0f),  // moving south
        CVector(0.5f, 0.5f, 0.0f),   // moving NE
    };
    for (auto& spd : speeds) {
        CVector origOut, revOut;
        float orig, rev;
        { HookDisableGuard guard("Global/CGameLogic/CalcDistanceToForbiddenTrainCrossing");
          orig = CGameLogic::CalcDistanceToForbiddenTrainCrossing(pos, spd, false, origOut); }
        rev = CGameLogic::CalcDistanceToForbiddenTrainCrossing(pos, spd, false, revOut);
        EXPECT_NEAR(orig, rev, 1e-3f);
    }
}

GAME_DIFF_TEST(CGameLogic, CalcDistToTrainCrossing_NearCrossing) {
    // Test very close to the LS-SF crossing point (82, -1021)
    CVector positions[] = {
        CVector(80.0f, -1020.0f, 10.0f),     // 2m from crossing
        CVector(82.0f, -1021.0f, 10.0f),     // exactly at crossing
        CVector(100.0f, -1021.0f, 10.0f),    // 18m east
        CVector(82.0f, -1000.0f, 10.0f),     // 21m north
    };
    for (auto& pos : positions) {
        CVector origOut, revOut;
        float orig, rev;
        { HookDisableGuard guard("Global/CGameLogic/CalcDistanceToForbiddenTrainCrossing");
          orig = CGameLogic::CalcDistanceToForbiddenTrainCrossing(pos, CVector(1, 0, 0), true, origOut); }
        rev = CGameLogic::CalcDistanceToForbiddenTrainCrossing(pos, CVector(1, 0, 0), true, revOut);
        EXPECT_NEAR(orig, rev, 1e-3f);
    }
}
