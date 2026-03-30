// test_CPedIK2.cpp — Differential tests for CPedIK functions.
// Hook paths: Global/CPedIK/<fn>
//
// Previously tested: MoveLimb (both overloads)
//
// New targets:
//   GetWorldMatrix      — matrix chain computation (walks RwFrame hierarchy)
//   PointGunInDirection — angle math (LimitRadianAngle + MoveLimb + bone lookup)
//   RotateTorsoForArm   — angle wrapping with DegreesToRadians conversions
//   PointGunAtPosition  — position-to-angle conversion + PointGunInDirection
//   PitchForSlope       — slope angle from ground normal

#include "StdInc.h"
#include "TestFramework.h"
#include "PedIK.h"

// ── GetWorldMatrix ──
// Walks RwFrame parent chain, composing matrices. Pure calculation.
// Test on player ped's head bone frame.

GAME_DIFF_TEST(CPedIK2, Diff_GetWorldMatrix_PlayerHead) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    if (!player->m_pRwClump) return;
    CPedIK* ik = &player->m_pedIK;

    // Get the RwFrame for the clump root
    auto* frame = RpClumpGetFrame(player->m_pRwClump);
    if (!frame) return;

    RwMatrix origMat{}, revMat{};
    { HookDisableGuard guard("Global/CPedIK/GetWorldMatrix");
      ik->GetWorldMatrix(frame, &origMat); }
    ik->GetWorldMatrix(frame, &revMat);

    EXPECT_NEAR(origMat.pos.x, revMat.pos.x, 1e-3f);
    EXPECT_NEAR(origMat.pos.y, revMat.pos.y, 1e-3f);
    EXPECT_NEAR(origMat.pos.z, revMat.pos.z, 1e-3f);
    EXPECT_NEAR(origMat.right.x, revMat.right.x, 1e-3f);
    EXPECT_NEAR(origMat.up.x, revMat.up.x, 1e-3f);
    EXPECT_NEAR(origMat.at.x, revMat.at.x, 1e-3f);
}

// ── PointGunInDirection ──
// Core aiming function: computes limb orientation from angle + distance.
// Uses LimitRadianAngle, bone hierarchy lookups, and MoveLimb.
// Mutates m_TorsoOrient and bGunReachedTarget/bTorsoUsed flags.

GAME_DIFF_TEST(CPedIK2, Diff_PointGunInDirection_Forward) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    CPedIK* ik = &player->m_pedIK;

    // Save state
    LimbOrientation savedOrient = ik->m_TorsoOrient;
    bool savedGun = ik->bGunReachedTarget;
    bool savedTorso = ik->bTorsoUsed;

    // Aim forward (same as ped rotation) at level height
    float zAngle = player->m_fCurrentRotation;
    float dist = 0.0f;

    ik->m_TorsoOrient = {};
    bool origRet;
    { HookDisableGuard guard("Global/CPedIK/PointGunInDirection");
      origRet = ik->PointGunInDirection(zAngle, dist, false, -1.0f); }
    LimbOrientation origOrient = ik->m_TorsoOrient;

    ik->m_TorsoOrient = {};
    bool revRet = ik->PointGunInDirection(zAngle, dist, false, -1.0f);
    LimbOrientation revOrient = ik->m_TorsoOrient;

    EXPECT_EQ(origRet, revRet);
    EXPECT_NEAR(origOrient.m_fYaw, revOrient.m_fYaw, 1e-3f);
    EXPECT_NEAR(origOrient.m_fPitch, revOrient.m_fPitch, 1e-3f);

    // Restore
    ik->m_TorsoOrient = savedOrient;
    ik->bGunReachedTarget = savedGun;
    ik->bTorsoUsed = savedTorso;
}

GAME_DIFF_TEST(CPedIK2, Diff_PointGunInDirection_Angled) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    CPedIK* ik = &player->m_pedIK;

    LimbOrientation savedOrient = ik->m_TorsoOrient;
    bool savedGun = ik->bGunReachedTarget;
    bool savedTorso = ik->bTorsoUsed;

    // Aim 45 degrees to the right, slightly upward
    float zAngle = player->m_fCurrentRotation + DegreesToRadians(45.0f);
    float dist = DegreesToRadians(-15.0f);

    ik->m_TorsoOrient = {};
    bool origRet;
    { HookDisableGuard guard("Global/CPedIK/PointGunInDirection");
      origRet = ik->PointGunInDirection(zAngle, dist, false, -1.0f); }
    LimbOrientation origOrient = ik->m_TorsoOrient;

    ik->m_TorsoOrient = {};
    bool revRet = ik->PointGunInDirection(zAngle, dist, false, -1.0f);
    LimbOrientation revOrient = ik->m_TorsoOrient;

    EXPECT_EQ(origRet, revRet);
    EXPECT_NEAR(origOrient.m_fYaw, revOrient.m_fYaw, 1e-3f);
    EXPECT_NEAR(origOrient.m_fPitch, revOrient.m_fPitch, 1e-3f);

    ik->m_TorsoOrient = savedOrient;
    ik->bGunReachedTarget = savedGun;
    ik->bTorsoUsed = savedTorso;
}

GAME_DIFF_TEST(CPedIK2, Diff_PointGunInDirection_WithNormalize) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    CPedIK* ik = &player->m_pedIK;

    LimbOrientation savedOrient = ik->m_TorsoOrient;
    bool savedGun = ik->bGunReachedTarget;
    bool savedTorso = ik->bTorsoUsed;

    float zAngle = player->m_fCurrentRotation + DegreesToRadians(30.0f);
    float dist = DegreesToRadians(-10.0f);

    // Test with positive normalize (uses the normalize overload of MoveLimb)
    ik->m_TorsoOrient = {};
    bool origRet;
    { HookDisableGuard guard("Global/CPedIK/PointGunInDirection");
      origRet = ik->PointGunInDirection(zAngle, dist, false, 2.0f); }
    LimbOrientation origOrient = ik->m_TorsoOrient;

    ik->m_TorsoOrient = {};
    bool revRet = ik->PointGunInDirection(zAngle, dist, false, 2.0f);
    LimbOrientation revOrient = ik->m_TorsoOrient;

    EXPECT_EQ(origRet, revRet);
    EXPECT_NEAR(origOrient.m_fYaw, revOrient.m_fYaw, 1e-3f);
    EXPECT_NEAR(origOrient.m_fPitch, revOrient.m_fPitch, 1e-3f);

    ik->m_TorsoOrient = savedOrient;
    ik->bGunReachedTarget = savedGun;
    ik->bTorsoUsed = savedTorso;
}

// ── RotateTorsoForArm ──
// Angle wrapping + threshold logic. Takes a world position, computes torso
// rotation needed to face it. Has DegreesToRadians conversions that could
// have the same RadiansToDegrees bug as GetLocalDirection.

GAME_DIFF_TEST(CPedIK2, Diff_RotateTorsoForArm_InFront) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    CPedIK* ik = &player->m_pedIK;

    // Target in front of player
    CVector target = player->GetPosition() + player->GetForwardVector() * 5.0f;

    LimbOrientation savedOrient = ik->m_TorsoOrient;
    auto savedMatrix = *player->m_matrix;

    { HookDisableGuard guard("Global/CPedIK/RotateTorsoForArm");
      ik->RotateTorsoForArm(target); }
    auto origMatrix = *player->m_matrix;

    *player->m_matrix = savedMatrix;
    ik->m_TorsoOrient = savedOrient;
    ik->RotateTorsoForArm(target);
    auto revMatrix = *player->m_matrix;

    // Compare rotation result
    EXPECT_NEAR(origMatrix.GetForward().x, revMatrix.GetForward().x, 1e-3f);
    EXPECT_NEAR(origMatrix.GetForward().y, revMatrix.GetForward().y, 1e-3f);

    // Restore
    *player->m_matrix = savedMatrix;
    ik->m_TorsoOrient = savedOrient;
}

GAME_DIFF_TEST(CPedIK2, Diff_RotateTorsoForArm_ToSide) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    CPedIK* ik = &player->m_pedIK;

    // Target to the right of player
    CVector target = player->GetPosition() + player->GetRightVector() * 5.0f;

    LimbOrientation savedOrient = ik->m_TorsoOrient;
    auto savedMatrix = *player->m_matrix;

    { HookDisableGuard guard("Global/CPedIK/RotateTorsoForArm");
      ik->RotateTorsoForArm(target); }
    auto origMatrix = *player->m_matrix;

    *player->m_matrix = savedMatrix;
    ik->m_TorsoOrient = savedOrient;
    ik->RotateTorsoForArm(target);
    auto revMatrix = *player->m_matrix;

    EXPECT_NEAR(origMatrix.GetForward().x, revMatrix.GetForward().x, 1e-3f);
    EXPECT_NEAR(origMatrix.GetForward().y, revMatrix.GetForward().y, 1e-3f);

    *player->m_matrix = savedMatrix;
    ik->m_TorsoOrient = savedOrient;
}

// ── PointGunAtPosition ──
// Converts world position to angle/distance, then calls PointGunInDirection.

GAME_DIFF_TEST(CPedIK2, Diff_PointGunAtPosition) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    CPedIK* ik = &player->m_pedIK;

    LimbOrientation savedOrient = ik->m_TorsoOrient;
    bool savedGun = ik->bGunReachedTarget;
    bool savedTorso = ik->bTorsoUsed;

    // Aim at a point 10 units ahead, 2 units up
    CVector target = player->GetPosition() + CVector(10.0f, 0.0f, 2.0f);

    ik->m_TorsoOrient = {};
    { HookDisableGuard guard("Global/CPedIK/PointGunAtPosition");
      ik->PointGunAtPosition(target, -1.0f); }
    LimbOrientation origOrient = ik->m_TorsoOrient;
    bool origGun = ik->bGunReachedTarget;

    ik->m_TorsoOrient = {};
    ik->PointGunAtPosition(target, -1.0f);
    LimbOrientation revOrient = ik->m_TorsoOrient;
    bool revGun = ik->bGunReachedTarget;

    EXPECT_EQ(origGun, revGun);
    EXPECT_NEAR(origOrient.m_fYaw, revOrient.m_fYaw, 1e-3f);
    EXPECT_NEAR(origOrient.m_fPitch, revOrient.m_fPitch, 1e-3f);

    ik->m_TorsoOrient = savedOrient;
    ik->bGunReachedTarget = savedGun;
    ik->bTorsoUsed = savedTorso;
}
